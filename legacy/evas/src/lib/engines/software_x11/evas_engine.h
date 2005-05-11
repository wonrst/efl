#ifndef EVAS_ENGINE_H
#define EVAS_ENGINE_H

/* PERF: 
 * # EVAS BENCH: 4.072
 */

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/extensions/XShm.h>
#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct _Outbuf                Outbuf;
typedef struct _Outbuf_Perf           Outbuf_Perf;
typedef struct _Outbuf_Region         Outbuf_Region;
typedef struct _X_Output_Buffer       X_Output_Buffer;

typedef enum   _Outbuf_Depth          Outbuf_Depth;

enum _Outbuf_Depth
{
   OUTBUF_DEPTH_NONE,
     OUTBUF_DEPTH_INHERIT,
     OUTBUF_DEPTH_RGB_16BPP_565_565_DITHERED,
     OUTBUF_DEPTH_RGB_16BPP_555_555_DITHERED,
     OUTBUF_DEPTH_RGB_16BPP_444_444_DITHERED,
     OUTBUF_DEPTH_RGB_16BPP_565_444_DITHERED,
     OUTBUF_DEPTH_RGB_32BPP_888_8888,
     OUTBUF_DEPTH_LAST
};

struct _Outbuf
{
   Outbuf_Depth    depth;
   int             w, h;
   int             rot;
   Outbuf_Perf    *perf;
   
   struct {
      Convert_Pal *pal;
      struct {
	 Display  *disp;
	 Window    win;
	 Pixmap    mask;
	 Visual   *vis;
	 Colormap  cmap;
	 int       depth;
	 int       shm;
	 GC        gc;
	 GC        gcm;
	 int       swap : 1;
      } x;
      struct {
	 DATA32    r, g, b;
      } mask;
      /* lets not do back buf for now */
      /* RGBA_Image  *back_buf; */

      /* a list of pending regions to write to the target */
      Evas_List   *pending_writes;
      
      int          mask_dither : 1;
      
      int          debug : 1;
   } priv;
};

struct _Outbuf_Perf
{
   struct {
      Display *disp;
      Window   root;
      
      char *display;
      char *vendor;
      int   version;
      int   revision;
      int   release;
      int   w, h;
      int   screen_count;
      int   depth;
      int   screen_num;
   } x;
   struct{
      char *name;
      char *version;
      char *machine;
   } os;
   struct {
      char *info;
   } cpu;
   
   int   min_shm_image_pixel_count;
};

struct _Outbuf_Region
{
   X_Output_Buffer *xob, *mxob;
   int x, y, w, h;
};

struct _X_Output_Buffer
{
   Display         *display;
   XImage          *xim;
   XShmSegmentInfo *shm_info;
   void            *data;
};

/****/
void             evas_software_x11_x_init                        (void);

void             evas_software_x11_x_write_mask_line             (X_Output_Buffer *xob, DATA32 *src, int w, int y);
int              evas_software_x11_x_can_do_shm                  (Display *d);
X_Output_Buffer *evas_software_x11_x_output_buffer_new           (Display *d, Visual *v, int depth, int w, int h, int try_shm, void *data);
void             evas_software_x11_x_output_buffer_free          (X_Output_Buffer *xob, int sync);
void             evas_software_x11_x_output_buffer_paste         (X_Output_Buffer *xob, Drawable d, GC gc, int x, int y, int sync);
DATA8           *evas_software_x11_x_output_buffer_data          (X_Output_Buffer *xob, int *bytes_per_line_ret);
int              evas_software_x11_x_output_buffer_depth         (X_Output_Buffer *xob);
int              evas_software_x11_x_output_buffer_byte_order    (X_Output_Buffer *xob);
    
void             evas_software_x11_x_color_init                  (void);
Convert_Pal     *evas_software_x11_x_color_allocate              (Display *disp, Colormap cmap, Visual *vis, Convert_Pal_Mode colors);
void             evas_software_x11_x_color_deallocate            (Display *disp, Colormap cmap, Visual *vis, Convert_Pal *pal);
    
void             evas_software_x11_outbuf_init                   (void);
void             evas_software_x11_outbuf_free                   (Outbuf *buf);

Outbuf          *evas_software_x11_outbuf_setup_x                (int w, int h, int rot, Outbuf_Depth depth, Display *disp, Drawable draw, Visual *vis, Colormap cmap, int x_depth, Outbuf_Perf *perf, int grayscale, int max_colors, Pixmap mask, int shape_dither);
    
char            *evas_software_x11_outbuf_perf_serialize_x       (Outbuf_Perf *perf);
void             evas_software_x11_outbuf_perf_deserialize_x     (Outbuf_Perf *perf, const char *data);
Outbuf_Perf     *evas_software_x11_outbuf_perf_new_x             (Display *disp, Window draw, Visual *vis, Colormap cmap,  int x_depth);
char            *evas_software_x11_outbuf_perf_serialize_info_x  (Outbuf_Perf *perf);
void             evas_software_x11_outbuf_perf_store_x           (Outbuf_Perf *perf);
Outbuf_Perf     *evas_software_x11_outbuf_perf_restore_x         (Display *disp, Window draw, Visual *vis, Colormap cmap, int x_depth);
void             evas_software_x11_outbuf_perf_free              (Outbuf_Perf *perf);
Outbuf_Perf     *evas_software_x11_outbuf_perf_x                 (Display *disp, Window draw, Visual *vis, Colormap cmap, int x_depth);

RGBA_Image      *evas_software_x11_outbuf_new_region_for_update  (Outbuf *buf, int x, int y, int w, int h, int *cx, int *cy, int *cw, int *ch);
void             evas_software_x11_outbuf_free_region_for_update (Outbuf *buf, RGBA_Image *update);
void             evas_software_x11_outbuf_flush                  (Outbuf *buf);
void             evas_software_x11_outbuf_push_updated_region    (Outbuf *buf, RGBA_Image *update, int x, int y, int w, int h);
void             evas_software_x11_outbuf_reconfigure            (Outbuf *buf, int w, int h, int rot, Outbuf_Depth depth);
int              evas_software_x11_outbuf_get_width              (Outbuf *buf);
int              evas_software_x11_outbuf_get_height             (Outbuf *buf);
Outbuf_Depth     evas_software_x11_outbuf_get_depth              (Outbuf *buf);
int              evas_software_x11_outbuf_get_rot                (Outbuf *buf);
void             evas_software_x11_outbuf_drawable_set           (Outbuf *buf, Drawable draw);
void             evas_software_x11_outbuf_mask_set               (Outbuf *buf, Pixmap mask);
void             evas_software_x11_outbuf_rotation_set           (Outbuf *buf, int rot);
    
void             evas_software_x11_outbuf_debug_set              (Outbuf *buf, int debug);
void             evas_software_x11_outbuf_debug_show             (Outbuf *buf, Drawable draw, int x, int y, int w, int h);
    
#endif
