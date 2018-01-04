#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif

#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_TEXT_PROTECTED
#define ELM_INTERFACE_ATSPI_TEXT_EDITABLE_PROTECTED
#define ELM_LAYOUT_PROTECTED

#define EFL_SELECTION_MANAGER_BETA

#include <Elementary.h>
#include <Elementary_Cursor.h>
#include "elm_priv.h"

typedef struct _Efl_Ui_Dnd_Container_Data Efl_Ui_Dnd_Container_Data;
struct _Efl_Ui_Dnd_Container_Data
{
    unsigned int drag_delay_time;
};

extern int _wl_default_seat_id_get(Evas_Object *obj);

static inline Eo*
_selection_manager_get(Eo *obj)
{
   Eo *top = elm_widget_top_get(obj);
   if (!top)
     {
        top = obj;
     }
   Eo *sel_man = efl_key_data_get(top, "__selection_manager");
   if (!sel_man)
     {
        sel_man = efl_add(EFL_SELECTION_MANAGER_CLASS, top);
        efl_key_data_set(top, "__selection_manager", sel_man);
     }
   return sel_man;
}

EOLIAN static void
_efl_ui_dnd_drag_start(Eo *obj, void *pd EINA_UNUSED, Efl_Selection_Format format, Eina_Slice data,
                       Efl_Selection_Action action, void *icon_func_data, Efl_Dnd_Drag_Icon_Create icon_func, Eina_Free_Cb icon_func_free_cb,
                       unsigned int seat)
{
   ERR("In");
   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_drag_start(sel_man, obj, format, data, action,
                                    icon_func_data, icon_func, icon_func_free_cb,
                                    seat);
}

EOLIAN static void
_efl_ui_dnd_drag_cancel(Eo *obj, void *pd EINA_UNUSED, unsigned int seat)
{
   ERR("In");
   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_drag_cancel(sel_man, obj, seat);
}

EOLIAN static void
_efl_ui_dnd_drag_action_set(Eo *obj, void *pd EINA_UNUSED, Efl_Selection_Action action, unsigned int seat)
{
   ERR("In");
   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_drag_action_set(sel_man, obj, action, seat);
}


EOLIAN static void
_efl_ui_dnd_drop_target_add(Eo *obj, void *pd EINA_UNUSED, Efl_Selection_Format format, unsigned int seat)
{
   ERR("In");

   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_drop_target_add(sel_man, obj, format, seat);
}

EOLIAN static void
_efl_ui_dnd_drop_target_del(Eo *obj, void *pd EINA_UNUSED, Efl_Selection_Format format, unsigned int seat)
{
   ERR("In");
   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_drop_target_del(sel_man, obj, format, seat);
}

EOLIAN static double
_efl_ui_dnd_container_drag_delay_time_get(Eo *obj EINA_UNUSED, Efl_Ui_Dnd_Container_Data *pd)
{
    return pd->drag_delay_time;
}

EOLIAN static void
_efl_ui_dnd_container_drag_delay_time_set(Eo *obj EINA_UNUSED, Efl_Ui_Dnd_Container_Data *pd, double drag_delay_time)
{
    pd->drag_delay_time = drag_delay_time;
}

EOLIAN static void
_efl_ui_dnd_container_drag_item_add(Eo *obj, Efl_Ui_Dnd_Container_Data *pd,
                                    void *data_func_data, Efl_Dnd_Drag_Data_Get data_func, Eina_Free_Cb data_func_free_cb,
                                    void *item_func_data, Efl_Dnd_Item_Get item_func, Eina_Free_Cb item_func_free_cb,
                                    void *icon_func_data, Efl_Dnd_Drag_Icon_Create icon_func, Eina_Free_Cb icon_func_free_cb,
                                    void *icon_list_func_data, Efl_Dnd_Drag_Icon_List_Create icon_list_func, Eina_Free_Cb icon_list_func_free_cb,
                                    unsigned int seat)
{
   ERR("In");
   double drag_delay_time = pd->drag_delay_time;
   double anim_time = elm_config_drag_anim_duration_get();
   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_container_drag_item_add(sel_man, obj, drag_delay_time, anim_time,
                                                 data_func_data, data_func, data_func_free_cb,
                                                 item_func_data, item_func, item_func_free_cb,
                                                 icon_func_data, icon_func, icon_func_free_cb,
                                                 icon_list_func_data, icon_list_func, icon_list_func_free_cb,
                                                 seat);
}

static void
_efl_ui_dnd_container_drag_item_del(Eo *obj, Efl_Ui_Dnd_Container_Data *pd EINA_UNUSED, unsigned int seat)
{
   ERR("In");
   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_container_drag_item_del(sel_man, obj, seat);
}
EOLIAN static void
_efl_ui_dnd_container_drop_item_add(Eo *obj, Efl_Ui_Dnd_Container_Data *pd EINA_UNUSED,
                                    Efl_Selection_Format format,
                                    void *item_func_data, Efl_Dnd_Item_Get item_func, Eina_Free_Cb item_func_free_cb,
                                    unsigned int seat)
{
   ERR("In");
   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_container_drop_item_add(sel_man, obj, format, item_func_data, item_func, item_func_free_cb, seat);
}

EOLIAN static void
_efl_ui_dnd_container_drop_item_del(Eo *obj, Efl_Ui_Dnd_Container_Data *pd EINA_UNUSED, unsigned int seat)
{
   ERR("In");
   Eo *sel_man = _selection_manager_get(obj);
   efl_selection_manager_container_drop_item_del(sel_man, obj, seat);
}


///////////
typedef struct _Dnd_Icon_Create Dnd_Icon_Create;
typedef struct _Dnd_Drag_Pos Dnd_Drag_Pos;
typedef struct _Dnd_Drag_Accept Dnd_Drag_Accept;
typedef struct _Dnd_Drag_Done Dnd_Drag_Done;
typedef struct _Dnd_Drag_State Dnd_Drag_State;
typedef struct _Dnd_Drop Dnd_Drop;
typedef struct _Dnd_Cont_Drag_Pos Dnd_Cont_Drag_Pos;
typedef struct _Dnd_Cont_Drop Dnd_Cont_Drop;
typedef struct _Item_Container_Drag_Info Item_Container_Drag_Info;

struct _Dnd_Icon_Create
{
   void *icon_data;
   Elm_Drag_Icon_Create_Cb icon_cb;
};

struct _Dnd_Drag_Pos
{
   void *pos_data;
   Elm_Drag_Pos pos_cb;
};

struct _Dnd_Drag_Accept
{
   void *accept_data;
   Elm_Drag_Accept accept_cb;
};

struct _Dnd_Drag_Done
{
   void *done_data;
   Elm_Drag_State done_cb;

   //for deleting
   Dnd_Drag_Pos *pos;
   Dnd_Drag_Accept *accept;
};

struct _Dnd_Drag_State
{
   void *state_data;
   Elm_Drag_State state_cb;
};

struct _Dnd_Drop
{
   Efl_Object *obj;
   Elm_Sel_Format format;
   void *drop_data;
   Elm_Drop_Cb drop_cb;

   //for deleting
   Dnd_Drag_State *enter;
   Dnd_Drag_State *leave;
   Dnd_Drag_Pos *pos;
};

struct _Dnd_Cont_Drag_Pos
{
   void *pos_data;
   Elm_Drag_Item_Container_Pos pos_cb;
   Elm_Xy_Item_Get_Cb item_get_cb;
};

struct _Dnd_Cont_Drop
{
   Efl_Object *obj;
   Elm_Sel_Format format;
   void *drop_data;
   Elm_Drop_Item_Container_Cb drop_cb;
   Elm_Xy_Item_Get_Cb item_get_cb;

   //for deleting
   Dnd_Drag_State *enter;
   Dnd_Drag_State *leave;
   Dnd_Cont_Drag_Pos *pos;
};

struct _Item_Container_Drag_Info
{
   Eina_List *icons;
   Elm_Drag_User_Info user_info;
   Elm_Object_Item *it;
   Elm_Item_Container_Data_Get_Cb data_get_cb;
   Elm_Xy_Item_Get_Cb item_get_cb;
};

static Efl_Object *
_dnd_icon_create_cb(void *data, Efl_Object *win, Efl_Object *drag_obj EINA_UNUSED, Eina_Position2D *pos_ret)
{
   Dnd_Icon_Create *ic = data;
   Efl_Object *ret = ic->icon_cb(ic->icon_data, win, &pos_ret->x, &pos_ret->y);

   free(ic);
   return ret;
}

static void
_dnd_drag_pos_cb(void *data, const Efl_Event *event)
{
   Dnd_Drag_Pos *pos = data;
   Efl_Dnd_Drag_Pos *ddata = event->info;
   ERR("dp: %p:: %p %p", pos, pos->pos_cb, pos->pos_data);

   if (pos->pos_cb)
     pos->pos_cb(pos->pos_data, event->object, ddata->pos.x, ddata->pos.y,
                 ddata->action);
}

static void
_dnd_drag_accept_cb(void *data, const Efl_Event *event)
{
   Dnd_Drag_Accept *accept = data;
   ERR("Da: %p: accept: %d", accept, *(Eina_Bool *)event->info);

   if (accept->accept_cb)
     accept->accept_cb(accept->accept_data, event->object, *(Eina_Bool *)event->info);
}

static void
_dnd_drag_done_cb(void *data, const Efl_Event *event)
{
   Dnd_Drag_Done *done = data;

   if (done->done_cb)
     done->done_cb(done->done_data, event->object);

   efl_event_callback_del(event->object, EFL_UI_DND_EVENT_DRAG_POS,
                          _dnd_drag_pos_cb, done->pos);
   efl_event_callback_del(event->object, EFL_UI_DND_EVENT_DRAG_ACCEPT,
                          _dnd_drag_accept_cb, done->accept);
   efl_event_callback_del(event->object, EFL_UI_DND_EVENT_DRAG_DONE,
                          _dnd_drag_done_cb, done);
   free(done->pos);
   free(done->accept);
   free(done);
}

static void
_dnd_drag_enter_leave_cb(void *data, const Efl_Event *event)
{
   Dnd_Drag_State *state = data;

   if (state->state_cb)
     state->state_cb(state->state_data, event->object);
}

static void
_dnd_drop_cb(void *data, const Efl_Event *event)
{
   Dnd_Drop *drop = data;
   Efl_Selection_Data *org_ddata = event->info;
   Elm_Selection_Data ddata;

   ERR("dropped: obj: %p; data: %s", event->object, (const char *)org_ddata->data.mem);
   ddata.x = org_ddata->pos.x;
   ddata.y = org_ddata->pos.y;
   ddata.format = org_ddata->format;
   ddata.action = org_ddata->action;
   ddata.data = calloc(1, org_ddata->data.len);
   if (!ddata.data) return;
   ddata.data = memcpy(ddata.data, org_ddata->data.mem, org_ddata->data.len);
   ddata.len = org_ddata->data.len;
   if (drop->drop_cb)
     drop->drop_cb(drop->drop_data, event->object, &ddata);
   free(ddata.data);
}

EAPI Eina_Bool
elm_drag_start(Evas_Object *obj, Elm_Sel_Format format, const char *data,
               Elm_Xdnd_Action action,
               Elm_Drag_Icon_Create_Cb icon_create_cb, void *icon_create_data,
               Elm_Drag_Pos drag_pos_cb, void *drag_pos_data,
               Elm_Drag_Accept drag_accept_cb, void *drag_accept_data,
               Elm_Drag_State drag_done_cb, void *drag_done_data)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;
   Eina_Slice sl;
   Dnd_Drag_Pos *pos = calloc(1, sizeof(Dnd_Drag_Pos));
   Dnd_Drag_Accept *accept = calloc(1, sizeof(Dnd_Drag_Accept));
   Dnd_Drag_Done *done = calloc(1, sizeof(Dnd_Drag_Done));
   Dnd_Icon_Create *ic = calloc(1, sizeof(Dnd_Icon_Create));
   if (!pos || !accept || !done || !ic) return EINA_FALSE;

   pos->pos_data = drag_pos_data;
   pos->pos_cb = drag_pos_cb;
   ERR("dp: %p: %p %p", pos, pos->pos_cb, pos->pos_data);

   accept->accept_data = drag_accept_data;
   accept->accept_cb = drag_accept_cb;

   done->done_data = drag_done_data;
   done->done_cb = drag_done_cb;
   done->pos = pos;
   done->accept = accept;

   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_POS, _dnd_drag_pos_cb, pos);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_ACCEPT, _dnd_drag_accept_cb, accept);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_DONE, _dnd_drag_done_cb, done);
   sl.mem = data;
   sl.len = strlen(data);
#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif

   ic->icon_data = icon_create_data;
   ic->icon_cb = icon_create_cb;
   efl_selection_manager_drag_start(sel_man, obj, format, sl, action,
                                    ic, _dnd_icon_create_cb, NULL, seatid);

   return EINA_TRUE;
}

EAPI Eina_Bool
elm_drag_action_set(Evas_Object *obj, Elm_Xdnd_Action action)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;

#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif
   efl_selection_manager_drag_action_set(sel_man, obj, action, seatid);

   return EINA_TRUE;
}

EAPI Eina_Bool
elm_drag_cancel(Evas_Object *obj)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;

#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif

   efl_selection_manager_drag_cancel(sel_man, obj, seatid);

   return EINA_TRUE;
}

static void
_drop_obj_del_cb(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj, void *event_info EINA_UNUSED)
{
   Eina_List *drop_list;
   Dnd_Drop *drop;

   drop_list = efl_key_data_get(obj, "__drop_list");
   EINA_LIST_FREE(drop_list, drop)
     {
        ERR("free one by one cb");
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_ENTER,
                               _dnd_drag_enter_leave_cb, drop->enter);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_LEAVE,
                               _dnd_drag_enter_leave_cb, drop->leave);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_POS,
                               _dnd_drag_pos_cb, drop->pos);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_DROP,
                               _dnd_drop_cb, drop);
        free(drop->enter);
        free(drop->leave);
        free(drop->pos);
        free(drop);
     }
}

EAPI Eina_Bool
elm_drop_target_add(Evas_Object *obj, Elm_Sel_Format format,
                    Elm_Drag_State enter_cb, void *enter_data,
                    Elm_Drag_State leave_cb, void *leave_data,
                    Elm_Drag_Pos pos_cb, void *pos_data,
                    Elm_Drop_Cb drop_cb, void *drop_data)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;
   Dnd_Drag_State *enter, *leave;
   Dnd_Drag_Pos *pos;
   Dnd_Drop *drop;
   Eina_List *drop_list;

   enter = calloc(1, sizeof(Dnd_Drag_State));
   leave = calloc(1, sizeof(Dnd_Drag_State));
   pos = calloc(1, sizeof(Dnd_Drag_Pos));
   drop = calloc(1, sizeof(Dnd_Drop));
   if (!enter || !leave || !pos || !drop) return EINA_FALSE;
#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif
   enter->state_cb = enter_cb;
   enter->state_data = enter_data;
   leave->state_cb = leave_cb;
   leave->state_data = leave_data;
   pos->pos_cb = pos_cb;
   pos->pos_data = pos_data;
   drop->obj = obj;
   drop->format = format;
   drop->drop_cb = drop_cb;
   drop->drop_data = drop_data;
   drop->enter = enter;
   drop->leave = leave;
   drop->pos = pos;

   ERR("register events for obj: %p", obj);
   drop_list = efl_key_data_get(obj, "__drop_list");
   drop_list = eina_list_append(drop_list, drop);
   efl_key_data_set(obj, "__drop_list", drop_list);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL,
                                  _drop_obj_del_cb, NULL);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_ENTER,
                          _dnd_drag_enter_leave_cb, enter);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_LEAVE,
                          _dnd_drag_enter_leave_cb, leave);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_POS,
                          _dnd_drag_pos_cb, pos);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_DROP,
                          _dnd_drop_cb, drop);
   efl_selection_manager_drop_target_add(sel_man, obj, format, seatid);


   return EINA_TRUE;
}

EAPI Eina_Bool
elm_drop_target_del(Evas_Object *obj, Elm_Sel_Format format,
                    Elm_Drag_State enter_cb, void *enter_data,
                    Elm_Drag_State leave_cb, void *leave_data,
                    Elm_Drag_Pos pos_cb, void *pos_data,
                    Elm_Drop_Cb drop_cb, void *drop_data)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;
   //Eina_List *l, *l2;
   Eina_List *drop_list;
   Dnd_Drop *drop;

#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif

   drop_list = efl_key_data_get(obj, "__drop_list");
   drop = eina_list_data_get(drop_list);
   if (drop &&
       (drop->format == drop->format) &&
       (drop->enter->state_cb == enter_cb) &&
       (drop->enter->state_data == enter_data) &&
       (drop->leave->state_cb == leave_cb) &&
       (drop->leave->state_data == leave_data) &&
       (drop->pos->pos_cb == pos_cb) &&
       (drop->pos->pos_data == pos_data) &&
       (drop->drop_cb == drop_cb) &&
       (drop->drop_data == drop_data))
     {
        ERR("free one cb");
        drop_list = eina_list_remove(drop_list, drop);
        efl_key_data_set(obj, "__drop_list", drop_list);
        evas_object_event_callback_del(obj, EVAS_CALLBACK_DEL, _drop_obj_del_cb);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_ENTER,
                               _dnd_drag_enter_leave_cb, drop->enter);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_LEAVE,
                               _dnd_drag_enter_leave_cb, drop->leave);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_POS,
                               _dnd_drag_pos_cb, drop->pos);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_DROP,
                               _dnd_drop_cb, drop);
        free(drop->enter);
        free(drop->leave);
        free(drop->pos);
        free(drop);
     }
   efl_selection_manager_drop_target_del(sel_man, obj, format, seatid);

   /*EINA_LIST_FOREACH_SAFE(drop_list, l, l2, drop)
     {
        if ((drop->obj == obj) &&
            (drop->format == drop->format) &&
            (drop->enter->state_cb == enter_cb) &&
            (drop->enter->state_data == enter_data) &&
            (drop->leave->state_cb == leave_cb) &&
            (drop->leave->state_data == leave_data) &&
            (drop->pos->pos_cb == pos_cb) &&
            (drop->pos->pos_data == pos_data) &&
            (drop->drop_cb == drop_cb) &&
            (drop->drop_data == drop_data))
          {
             drop_list = eina_list_remove(drop_list, drop);
             efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_ENTER,
                                    _dnd_drag_enter_leave_cb, drop->enter);
             efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_LEAVE,
                                    _dnd_drag_enter_leave_cb, drop->leave);
             efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_POS,
                                    _dnd_drag_pos_cb, drop->pos);
             efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_DROP,
                                    _dnd_drop_cb, drop);
             free(drop->enter);
             free(drop->leave);
             free(drop->pos);
             free(drop);
             break;
          }
     }*/

   return EINA_TRUE;
}

static Efl_Object *
_dnd_item_func(void *data, Efl_Canvas_Object *item, Eina_Position2D pos, Eina_Position2D *pos_ret)
{
   ERR("in");
   Elm_Xy_Item_Get_Cb item_get_cb = data;
   Evas_Coord x, y;
   Efl_Object *obj = NULL;

   x = y = 0;
   if (item_get_cb)
     obj = item_get_cb(item, pos.x, pos.y, &x, &y);
   if (pos_ret)
     {
        pos_ret->x = x;
        pos_ret->y = y;
     }

   return obj;
}

static void
_dnd_cont_drag_pos_cb(void *data, const Efl_Event *event)
{
   Dnd_Cont_Drag_Pos *pos = data;
   Efl_Dnd_Drag_Pos *ddata = event->info;
   Evas_Coord xret = 0, yret = 0;
   ERR("dp: %p:: %p %p", pos, pos->pos_cb, pos->pos_data);

   if (pos->item_get_cb)
     {
        Evas_Coord x, y;
        evas_object_geometry_get(event->object, &x, &y, NULL, NULL);
        pos->item_get_cb(event->object, ddata->pos.x + x, ddata->pos.y + y,
                         &xret, &yret);
     }
   if (pos->pos_cb)
     pos->pos_cb(pos->pos_data, event->object, ddata->item, ddata->pos.x, ddata->pos.y,
                 xret, yret, ddata->action);
}

static void
_dnd_cont_drop_cb(void *data, const Efl_Event *event)
{
   Dnd_Cont_Drop *drop = data;
   Efl_Selection_Data *org_ddata = event->info;
   Elm_Selection_Data ddata;
   Evas_Coord xret = 0, yret = 0;

   ERR("dropped: obj: %p; data: %s", event->object, (const char *)org_ddata->data.mem);
   ddata.x = org_ddata->pos.x;
   ddata.y = org_ddata->pos.y;
   ddata.format = org_ddata->format;
   ddata.action = org_ddata->action;
   ddata.data = calloc(1, org_ddata->data.len);
   if (!ddata.data) return;
   ddata.data = memcpy(ddata.data, org_ddata->data.mem, org_ddata->data.len);
   ddata.len = org_ddata->data.len;

   if (drop->item_get_cb)
     {
        Evas_Coord x, y;
        evas_object_geometry_get(event->object, &x, &y, NULL, NULL);
        drop->item_get_cb(event->object, ddata.x + x, ddata.y + y,
                          &xret, &yret);
     }

   if (drop->drop_cb)
     drop->drop_cb(drop->drop_data, event->object, org_ddata->item,
                   &ddata, xret, yret);
   free(ddata.data);
}

static void
_cont_drop_free_data(Evas_Object *obj)
{
   ERR("In");
   Eina_List *cont_drop_list;
   Dnd_Cont_Drop *drop;

   cont_drop_list = efl_key_data_get(obj, "__cont_drop_item");
   drop = eina_list_data_get(cont_drop_list);
   if (drop)
     {
        ERR("Free drop and callbacks");
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_ENTER,
                               _dnd_drag_enter_leave_cb, drop->enter);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_LEAVE,
                               _dnd_drag_enter_leave_cb, drop->leave);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_POS,
                               _dnd_cont_drag_pos_cb, drop->pos);
        efl_event_callback_del(drop->obj, EFL_UI_DND_EVENT_DRAG_DROP,
                               _dnd_cont_drop_cb, drop);
        free(drop->enter);
        free(drop->leave);
        free(drop->pos);
        cont_drop_list = eina_list_remove(cont_drop_list, drop);
        efl_key_data_set(obj, "__cont_drop_item", cont_drop_list);
        free(drop);
     }
}

static void
_cont_drop_obj_del_cb(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj, void *info EINA_UNUSED)
{
   ERR("In");
   _cont_drop_free_data(obj);
}

EAPI Eina_Bool
elm_drop_item_container_add(Evas_Object *obj,
                            Elm_Sel_Format format,
                            Elm_Xy_Item_Get_Cb item_get_cb,
                            Elm_Drag_State enter_cb, void *enter_data,
                            Elm_Drag_State leave_cb, void *leave_data,
                            Elm_Drag_Item_Container_Pos pos_cb, void *pos_data,
                            Elm_Drop_Item_Container_Cb drop_cb, void *drop_data)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;
   Dnd_Drag_State *enter, *leave;
   Dnd_Cont_Drag_Pos *pos;
   Dnd_Cont_Drop *drop;
   Eina_List *cont_drop_list;

   enter = calloc(1, sizeof(Dnd_Drag_State));
   leave = calloc(1, sizeof(Dnd_Drag_State));
   pos = calloc(1, sizeof(Dnd_Cont_Drag_Pos));
   drop = calloc(1, sizeof(Dnd_Cont_Drop));
   if (!enter || !leave || !pos || !drop) return EINA_FALSE;
#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif

   enter->state_cb = enter_cb;
   enter->state_data = enter_data;
   leave->state_cb = leave_cb;
   leave->state_data = leave_data;
   pos->pos_cb = pos_cb;
   pos->pos_data = pos_data;
   pos->item_get_cb = item_get_cb;
   drop->obj = obj;
   drop->format = format;
   drop->drop_cb = drop_cb;
   drop->drop_data = drop_data;
   drop->enter = enter;
   drop->leave = leave;
   drop->pos = pos;

   cont_drop_list = efl_key_data_get(obj, "__cont_drop_item");
   cont_drop_list = eina_list_append(cont_drop_list, drop);
   efl_key_data_set(obj, "__cont_drop_item", cont_drop_list);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL,
                                  _cont_drop_obj_del_cb, NULL);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_ENTER,
                          _dnd_drag_enter_leave_cb, enter);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_LEAVE,
                          _dnd_drag_enter_leave_cb, leave);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_POS,
                          _dnd_cont_drag_pos_cb, pos);
   efl_event_callback_add(obj, EFL_UI_DND_EVENT_DRAG_DROP,
                          _dnd_cont_drop_cb, drop);
   efl_selection_manager_container_drop_item_add(sel_man, obj, format,
                                                 item_get_cb, _dnd_item_func, NULL,
                                                 seatid);

   return EINA_TRUE;
}

EAPI Eina_Bool
elm_drop_item_container_del(Evas_Object *obj)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;

#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif

   ERR("In");
   _cont_drop_free_data(obj);
   evas_object_event_callback_del(obj, EVAS_CALLBACK_DEL, _cont_drop_obj_del_cb);
   efl_selection_manager_container_drop_item_del(sel_man, obj, seatid);

   return EINA_TRUE;
}

static void
_cont_drag_data_func(void *data, Efl_Object *obj, Efl_Selection_Format *format,
                     Eina_Rw_Slice *drag_data, Efl_Selection_Action *action)
{
   ERR("in");
   Item_Container_Drag_Info *di;

   di = data;
   if (!di) return;
   di->data_get_cb(obj, di->it, &di->user_info);
   if (format) *format = di->user_info.format;
   if (drag_data)
     {
        if (di->user_info.data)
          {
             drag_data->mem = (void *)di->user_info.data;
             drag_data->len = strlen(di->user_info.data);
             ERR("drag data: %s (%zd)", (const char *)drag_data->mem, drag_data->len);
          }
     }
   if (action) *action = di->user_info.action;
}

static Eina_List *
_cont_drag_icon_list_create(void *data, Efl_Object *obj EINA_UNUSED)
{
   ERR("in");
   Item_Container_Drag_Info *di;

   di = data;
   return di->icons;
}

static Efl_Object *
_cont_drag_icon_create(void *data, Efl_Object *win, Efl_Object *drag_obj EINA_UNUSED, Eina_Position2D *pos_ret)
{
   ERR("in");
   Item_Container_Drag_Info *di;
   Elm_Object_Item *it = NULL;

   di = data;
   if (!di) return NULL;
   it = di->user_info.createicon(di->user_info.createdata, win, &pos_ret->x, &pos_ret->y);
   di->it = it;
   return it;
}

static Efl_Object *
_cont_drag_item_func(void *data, Efl_Canvas_Object *item, Eina_Position2D pos, Eina_Position2D *pos_ret)
{
   ERR("in");
   Item_Container_Drag_Info *di = data;
   Evas_Coord x, y;
   Efl_Object *obj = NULL;

   x = y = 0;
   if (di->item_get_cb)
     obj = di->item_get_cb(item, pos.x, pos.y, &x, &y);
   if (pos_ret)
     {
        pos_ret->x = x;
        pos_ret->y = y;
     }
   di->it = obj;

   return obj;
}

static void
_cont_drag_free_data(Evas_Object *obj)
{
   Eina_List *di_list;
   Item_Container_Drag_Info *di;

   di_list = efl_key_data_get(obj, "__cont_drag_item");
   di = eina_list_data_get(di_list);
   di_list = eina_list_remove(di_list, di);
   efl_key_data_set(obj, "__cont_drag_item", di_list);
   free(di);
}

static void
_cont_drag_obj_del_cb(void *data EINA_UNUSED, Evas *e EINA_UNUSED, Evas_Object *obj, void *event EINA_UNUSED)
{
   ERR("In");
   _cont_drag_free_data(obj);
}

EAPI Eina_Bool
elm_drag_item_container_add(Evas_Object *obj, double anim_tm, double tm_to_drag,
                            Elm_Xy_Item_Get_Cb item_get_cb, Elm_Item_Container_Data_Get_Cb data_get_cb)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;
   Eina_List *di_list;
   Item_Container_Drag_Info *di;

#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif

   di = calloc(1, sizeof(Item_Container_Drag_Info));
   if (!di) return EINA_FALSE;
   di->data_get_cb = data_get_cb;
   di->item_get_cb = item_get_cb;
   di_list = efl_key_data_get(obj, "__cont_drag_item");
   di_list = eina_list_append(di_list, di);
   efl_key_data_set(obj, "__cont_drag_item", di_list);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _cont_drag_obj_del_cb, NULL);
   efl_selection_manager_container_drag_item_add(sel_man, obj, anim_tm, tm_to_drag,
                                                 di, _cont_drag_data_func, NULL,
                                                 di, _cont_drag_item_func, NULL,
                                                 di, _cont_drag_icon_create, NULL,
                                                 di, _cont_drag_icon_list_create, NULL,
                                                 seatid);
   return EINA_TRUE;
}

EAPI Eina_Bool
elm_drag_item_container_del(Evas_Object *obj)
{
   Eo *sel_man = _selection_manager_get(obj);
   int seatid = 1;

#ifdef HAVE_ELEMENTARY_WL2
   seatid = _wl_default_seat_id_get(obj);
#endif

   _cont_drag_free_data(obj);
   evas_object_event_callback_del(obj, EVAS_CALLBACK_DEL, _cont_drag_obj_del_cb);
   efl_selection_manager_container_drag_item_del(sel_man, obj, seatid);

   return EINA_TRUE;
}

#include "efl_ui_dnd.eo.c"
#include "efl_ui_dnd_container.eo.c"