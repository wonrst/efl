#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <sys/mman.h>

#ifdef EVAS_CSERVE2
# include "evas_cs2_private.h"
#endif

#include "evas_common.h"
#include "evas_macros.h"
#include "evas_engine.h"
#include "evas_swapper.h"

/* local structures */
typedef struct _Wl_Buffer Wl_Buffer;
struct _Wl_Buffer
{
   int w, h;
   struct wl_buffer *buffer;
   void *data;
   int offset;
   size_t size;
   Eina_Bool valid : 1;
};

struct _Wl_Swapper
{
   Wl_Buffer buff[3];
   int w, h, depth;
   int buff_cur, buff_num;
   struct wl_shm *shm;
   struct wl_surface *surface;
   struct wl_shm_pool *pool;
   size_t pool_size;
   size_t used_size;
   void *data;
   Eina_Bool alpha : 1;
   Eina_Bool mapped : 1;
};

/* local function prototypes */
static Eina_Bool _evas_swapper_shm_pool_new(Wl_Swapper *ws);
static Eina_Bool _evas_swapper_buffer_new(Wl_Swapper *ws, Wl_Buffer *wb);
static void _evas_swapper_buffer_free(Wl_Buffer *wb);
static void _evas_swapper_buffer_put(Wl_Swapper *ws, Wl_Buffer *wb, Eina_Rectangle *rects, unsigned int count);
static void _evas_swapper_buffer_release(void *data, struct wl_buffer *buffer);

static const struct wl_buffer_listener _evas_swapper_buffer_listener = 
{
   _evas_swapper_buffer_release
};

/* local variables */

Wl_Swapper *
evas_swapper_setup(int w, int h, Outbuf_Depth depth, Eina_Bool alpha, struct wl_shm *shm, struct wl_surface *surface)
{
   Wl_Swapper *ws;
   int i = 0;
   char *num_buffers;

   /* try to allocate a new swapper */
   if (!(ws = calloc(1, sizeof(Wl_Swapper)))) 
     return NULL;

   /* set some properties */
   ws->w = w;
   ws->h = h;
   ws->depth = depth;
   ws->alpha = alpha;
   ws->shm = shm;
   ws->surface = surface;

   /* double buffer by default */
   ws->buff_num = 2;

   /* check for buffer override number */
   if ((num_buffers = getenv("EVAS_WAYLAND_SHM_BUFFERS")))
     {
        int num = 0;

        num = atoi(num_buffers);

        if (num <= 0) num = 1;
        if (num > 3) num = 3;

        ws->buff_num = num;
     }

   /* create the shm pool */
   if (!_evas_swapper_shm_pool_new(ws))
     {
        evas_swapper_free(ws);
        return NULL;
     }

   for (i = 0; i < ws->buff_num; i++)
     {
        /* try to create new internal Wl_Buffer */
        if (!_evas_swapper_buffer_new(ws, &(ws->buff[i])))
          {
             /* failed to create wl_buffer. free the swapper */
             evas_swapper_free(ws);
             return NULL;
          }
     }

   /* return allocated swapper */
   return ws;
}

void 
evas_swapper_swap(Wl_Swapper *ws, Eina_Rectangle *rects, unsigned int count)
{
   int n = 0;

   /* check for valid swapper */
   if (!ws) return;

   n = ws->buff_cur;
   _evas_swapper_buffer_put(ws, &(ws->buff[n]), rects, count);
   ws->buff[n].valid = EINA_TRUE;
   ws->buff_cur = (ws->buff_cur + 1) % ws->buff_num;
}

void 
evas_swapper_free(Wl_Swapper *ws)
{
   int i = 0;

   /* check for valid swapper */
   if (!ws) return;

   /* loop the swapper's buffers and free them */
   for (i = 0; i < ws->buff_num; i++)
     _evas_swapper_buffer_free(&(ws->buff[i]));

   /* free the allocated structure */
   free(ws);
}

void *
evas_swapper_buffer_map(Wl_Swapper *ws)
{
   /* check for valid swapper */
   if (!ws) return NULL;

   /* set mapped property */
   ws->mapped = EINA_TRUE;

   /* return wl_buffer data */
   return ws->buff[ws->buff_cur].data;
}

void 
evas_swapper_buffer_unmap(Wl_Swapper *ws)
{
   /* check for valid swapper */
   if (!ws) return;

   ws->mapped = EINA_FALSE;
}

int 
evas_swapper_buffer_state_get(Wl_Swapper *ws)
{
   int i = 0, n = 0, count = 0;

   for (i = 0; i < ws->buff_num; i++)
     {
        n = (ws->buff_num + ws->buff_cur - (i)) % ws->buff_num;
        if (ws->buff[n].valid) count++;
        else break;
     }

   if (count == ws->buff_num)
     {
        if (count == 1) return MODE_COPY;
        else if (count == 2) return MODE_DOUBLE;
        else if (count == 3) return MODE_TRIPLE;
     }

   return MODE_FULL;
}

void 
evas_swapper_buffer_idle_flush(Wl_Swapper *ws)
{
   int i = 0;

   /* check for valid swapper */
   if (!ws) return;

   /* loop the swapper's buffers and free them */
   for (i = 0; i < ws->buff_num; i++)
     {
        Wl_Buffer *wb = NULL;

        /* try to get out Wl_Buffer struct */
        if (!(wb = (&(ws->buff[i])))) continue;

        /* if this buffer is not valid, then unmap data */
        if (!wb->valid) _evas_swapper_buffer_free(wb);
     }
}

/* local functions */
static Eina_Bool 
_evas_swapper_shm_pool_new(Wl_Swapper *ws)
{
   char tmp[PATH_MAX];
   size_t size;
   int fd = 0;

   /* make sure swapper has a shm */
   if (!ws->shm) return EINA_FALSE;

   /* calculate new required size */
   size = (((ws->w * sizeof(int)) * ws->h) * ws->buff_num);

   /* check pool size to see if we need to realloc the pool */
   if (size <= ws->pool_size) return EINA_TRUE;

   /* create tmp file
    * 
    * NB: Should this use XDG_RUNTIME_DIR ?? */
   strcpy(tmp, "/tmp/evas-wayland_shm-XXXXXX");

   /* try to create the tmp file */
   if ((fd = mkstemp(tmp)) < 0)
     {
        ERR("Could not create temporary file.");
        return EINA_FALSE;
     }

   /* try to truncate the tmp file to requested size */
   if (ftruncate(fd, size) < 0)
     {
        ERR("Could not truncate temporary file.");
        close(fd);
        return EINA_FALSE;
     }

   /* mem map the file */
   ws->data = mmap(NULL, size, (PROT_READ | PROT_WRITE), MAP_SHARED, fd, 0);

   /* unlink the tmp file */
   unlink(tmp);

   /* if we failed to mem map the file, return an error */
   if (ws->data == MAP_FAILED)
     {
        ERR("Could not mmap temporary file.");
        close(fd);
        return EINA_FALSE;
     }

   /* actually create the shm pool */
   ws->pool = wl_shm_create_pool(ws->shm, fd, size);

   ws->pool_size = size;
   ws->used_size = 0;

   /* close the file */
   close(fd);

   return EINA_TRUE;
}

static Eina_Bool 
_evas_swapper_buffer_new(Wl_Swapper *ws, Wl_Buffer *wb)
{
   unsigned int format = WL_SHM_FORMAT_XRGB8888;
   size_t size;

   /* make sure swapper has a shm */
   if (!ws->shm) return EINA_FALSE;

   wb->w = ws->w;
   wb->h = ws->h;

   /* calculate new required size */
   size = ((wb->w * sizeof(int)) * wb->h);

   /* check pool size to see if we need to realloc the pool */
   if (ws->used_size + size > ws->pool_size) 
     {
        printf("FIXME !!! Pool Size Too Small !!!\n");
        /* FIXME: Here we need to reallocate the pool to a greater size !! */
        return EINA_FALSE;
     }

   /* check if this buffer needs argb and set format */
   if (ws->alpha) format = WL_SHM_FORMAT_ARGB8888;

   /* create actual wl_buffer */
   wb->buffer = 
     wl_shm_pool_create_buffer(ws->pool, ws->used_size, wb->w, wb->h, 
                               (wb->w * sizeof(int)), format);

   /* add wayland buffer listener */
   wl_buffer_add_listener(wb->buffer, &_evas_swapper_buffer_listener, wb);

   wb->data = (char *)ws->data + ws->used_size;
   wb->size = size;

   ws->used_size += size;

   /* return allocated buffer */
   return EINA_TRUE;
}

static void 
_evas_swapper_buffer_free(Wl_Buffer *wb)
{
   /* check for valid buffer */
   if (!wb) return;

   /* kill the wl_buffer */
   if (wb->buffer) wl_buffer_destroy(wb->buffer);
   wb->buffer = NULL;

   /* unmap the buffer data */
   if (wb->data) munmap(wb->data, wb->size);
   wb->data = NULL;
}

static void 
_evas_swapper_buffer_put(Wl_Swapper *ws, Wl_Buffer *wb, Eina_Rectangle *rects, unsigned int count)
{
   Eina_Rectangle *rect;

   /* check for valid swapper */
   if (!ws) return;

   /* make sure swapper has a surface */
   if (!ws->surface) return;

   /* check for valid buffer */
   if (!wb) return;

   /* make sure buffer has mapped data */
   if ((!wb->data) || (!wb->buffer))
     {
        /* call function to mmap buffer data */
        if (!_evas_swapper_buffer_new(ws, wb))
          return;
     }

   rect = eina_rectangle_new(0, 0, 0, 0);
   if (rects)
     {
        unsigned int i = 0;

        for (i = 0; i < count; i++)
          eina_rectangle_union(rect, &rects[i]);
     }
   else
     {
        Eina_Rectangle r;

        r.x = 0; r.y = 0;
        r.w = wb->w; r.h = wb->h;

        eina_rectangle_union(rect, &r);
     }

   /* surface attach */
   wl_surface_attach(ws->surface, wb->buffer, 0, 0);

   /* surface damage */
   /* printf("Damage Surface: %d %d %d %d\n", rect->x, rect->y, rect->w, rect->h); */

   wl_surface_damage(ws->surface, rect->x, rect->y, rect->w, rect->h);
   /* wl_surface_damage(ws->surface, 0, 0, rect->w, rect->h); */

   /* surface commit */
   wl_surface_commit(ws->surface);

   wb->valid = EINA_TRUE;
}

static void 
_evas_swapper_buffer_release(void *data, struct wl_buffer *buffer EINA_UNUSED)
{
   Wl_Buffer *wb = NULL;

   /* try to get out Wl_Buffer struct */
   if (!(wb = data)) return;

   /* invalidate buffer */
   wb->valid = EINA_FALSE;
}
