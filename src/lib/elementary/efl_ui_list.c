#ifdef HAVE_CONFIG_H
# include "elementary_config.h"
#endif
#define ELM_INTERFACE_ATSPI_ACCESSIBLE_PROTECTED
#define ELM_INTERFACE_ATSPI_WIDGET_ACTION_PROTECTED
#define EFL_ACCESS_SELECTION_PROTECTED

#include <Elementary.h>
#include "efl_ui_list_private.h"
#include "efl_ui_list_precise_layouter.eo.h"

#include <assert.h>

#define MY_CLASS EFL_UI_LIST_CLASS
#define MY_CLASS_NAME "Efl.Ui.List"

#define MY_PAN_CLASS EFL_UI_LIST_PAN_CLASS

#define SIG_CHILD_ADDED "child,added"
#define SIG_CHILD_REMOVED "child,removed"
#define SELECTED_PROP "selected"

#undef DBG
#define DBG(...) do { \
    fprintf(stderr, __FILE__ ":" "%d %s ", __LINE__, __PRETTY_FUNCTION__); \
    fprintf(stderr,  __VA_ARGS__);                                     \
    fprintf(stderr, "\n"); fflush(stderr);                              \
  } while(0)

static const Evas_Smart_Cb_Description _smart_callbacks[] = {
   {SIG_CHILD_ADDED, ""},
   {SIG_CHILD_REMOVED, ""},
   {NULL, NULL}
};

void _efl_ui_list_custom_layout(Efl_Ui_List *);
void _efl_ui_list_item_select_set(Efl_Ui_List_LayoutItem*, Eina_Bool);
static void _layout(Efl_Ui_List_Data* pd);

static Eina_Bool _key_action_move(Evas_Object *obj, const char *params);
static Eina_Bool _key_action_select(Evas_Object *obj, const char *params);
static Eina_Bool _key_action_escape(Evas_Object *obj, const char *params);

static const Elm_Action key_actions[] = {
   {"move", _key_action_move},
   {"select", _key_action_select},
   {"escape", _key_action_escape},
   {NULL, NULL}
};

EOLIAN static void
_efl_ui_list_pan_efl_canvas_group_group_calculate(Eo *obj EINA_UNUSED, Efl_Ui_List_Pan_Data *psd)
{
   evas_object_smart_changed(psd->wobj);
}


EOLIAN static void
_efl_ui_list_pan_elm_pan_pos_set(Eo *obj EINA_UNUSED, Efl_Ui_List_Pan_Data *psd, Evas_Coord x, Evas_Coord y)
{
   if ((x == psd->x) && (y == psd->y)) return;

   psd->x = x;
   psd->y = y;

   evas_object_smart_changed(psd->wobj);
}

EOLIAN static void
_efl_ui_list_pan_elm_pan_pos_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Pan_Data *psd, Evas_Coord *x, Evas_Coord *y)
{
   if (x) *x = psd->x;
   if (y) *y = psd->y;
}

EOLIAN static void
_efl_ui_list_pan_elm_pan_pos_max_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Pan_Data *psd, Evas_Coord *x, Evas_Coord *y)
{
   Evas_Coord ow, oh;
   Eina_Size2D min;

   elm_interface_scrollable_content_viewport_geometry_get
              (psd->wobj, NULL, NULL, &ow, &oh);

   min = efl_ui_list_model_min_size_get(psd->wobj);
   ow = min.w - ow;
   if (ow < 0) ow = 0;
   oh = min.h - oh;
   if (oh < 0) oh = 0;

   if (x) *x = ow;
   if (y) *y = oh;
}

EOLIAN static void
_efl_ui_list_pan_elm_pan_pos_min_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Pan_Data *psd EINA_UNUSED, Evas_Coord *x, Evas_Coord *y)
{
   if (x) *x = 0;
   if (y) *y = 0;
}

EOLIAN static void
_efl_ui_list_pan_elm_pan_content_size_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Pan_Data *psd, Evas_Coord *w, Evas_Coord *h)
{
   Eina_Size2D min;
   min = efl_ui_list_model_min_size_get(psd->wobj);

   if (w) *w = min.w;
   if (h) *h = min.h;
}

EOLIAN static void
_efl_ui_list_pan_efl_object_destructor(Eo *obj, Efl_Ui_List_Pan_Data *psd EINA_UNUSED)
{
   efl_destructor(efl_super(obj, MY_PAN_CLASS));
}

#include "efl_ui_list_pan.eo.c"

static Eina_Bool
_efl_model_properties_has(Efl_Model *model, Eina_Stringshare *propfind)
{
   const Eina_Array *properties;
   Eina_Array_Iterator iter_prop;
   Eina_Stringshare *property;
   Eina_Bool ret = EINA_FALSE;
   unsigned i = 0;

   EINA_SAFETY_ON_NULL_RETURN_VAL(model, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(propfind, EINA_FALSE);

   properties = efl_model_properties_get(model);

   EINA_ARRAY_ITER_NEXT(properties, i, property, iter_prop)
     {
        if (property == propfind)
          {
             ret = EINA_TRUE;
             break;
          }
     }
   return ret;
}

static void
_child_added_cb(void *data, const Efl_Event *event)
{
   Efl_Model_Children_Event* evt = event->info;
   Efl_Ui_List *obj = data;
   EFL_UI_LIST_DATA_GET_OR_RETURN(obj, pd);

   efl_ui_list_segarray_insert(&pd->segarray, evt->index, evt->child);
   evas_object_smart_changed(obj);
}

static void
_child_removed_cb(void *data, const Efl_Event *event)
{
   Efl_Model_Children_Event* evt = event->info;
   Efl_Ui_List *obj = data;
   EFL_UI_LIST_DATA_GET_OR_RETURN(obj, pd);

   efl_ui_list_segarray_remove(&pd->segarray, evt->index);
   evas_object_smart_changed(obj);
}

static void
_on_item_mouse_up(void *data, Evas *evas EINA_UNUSED, Evas_Object *o EINA_UNUSED, void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   Efl_Ui_List_LayoutItem *item = data;

   if (ev->button != 1) return;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return;

//   printf("clicked item %d\n", efl_ui_list_item_index_get(item));
   _efl_ui_list_item_select_set(item, EINA_TRUE);
}

static void
_count_then(void * data, Efl_Event const* event EINA_UNUSED)
{
   Efl_Ui_List_Data *pd = data;
   EINA_SAFETY_ON_NULL_RETURN(pd);

   pd->count_future = NULL;
   _layout(pd);
}

static void
_count_error(void * data, Efl_Event const* event EINA_UNUSED)
{
   Efl_Ui_List_Data *pd = data;
   EINA_SAFETY_ON_NULL_RETURN(pd);
   pd->count_future = NULL;
}

static void
_children_slice_error(void * data EINA_UNUSED, Efl_Event const* event EINA_UNUSED)
{
   Efl_Ui_List_Data *pd = data;
   EINA_SAFETY_ON_NULL_RETURN(pd);
   pd->slice_future = NULL;
}

static void
_show_region_hook(void *data EINA_UNUSED, Evas_Object *obj, Eina_Rect r)
{
   elm_interface_scrollable_content_region_show(obj, r.x, r.y, r.w, r.h);
}

EOLIAN static void
_efl_ui_list_select_mode_set(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd, Elm_Object_Select_Mode mode)
{
   /* Eina_Array_Iterator iterator; */
   /* Efl_Ui_List_Item *item; */
   /* unsigned int i; */

   if (pd->select_mode == mode)
     return;

   /* if (pd->select_mode == ELM_OBJECT_SELECT_MODE_NONE) */
   /*   { */
   /*      EINA_ARRAY_ITER_NEXT(pd->items.array, i, item, iterator) */
   /*        { */
   /*           if (item->selected) */
   /*             elm_layout_signal_emit(item->layout, "elm,state,selected", "elm"); */

   /*           efl_ui_model_connect(item->layout, "signal/elm,state,%v", "selected"); */
   /*        } */
   /*   } */
   /* else if (mode == ELM_OBJECT_SELECT_MODE_NONE) */
   /*   { */
   /*      EINA_ARRAY_ITER_NEXT(pd->items.array, i, item, iterator) */
   /*        { */
   /*           if (item->selected) */
   /*             elm_layout_signal_emit(item->layout, "elm,state,unselected", "elm"); */

   /*           efl_ui_model_connect(item->layout, "signal/elm,state,%v", NULL); */
   /*        } */
   /*   } */
   pd->select_mode = mode;
}

EOLIAN static Elm_Object_Select_Mode
_efl_ui_list_select_mode_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   return pd->select_mode;
}

EOLIAN static void
_efl_ui_list_default_style_set(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd, Eina_Stringshare *style)
{
   eina_stringshare_replace(&pd->style, style);
}

EOLIAN static Eina_Stringshare *
_efl_ui_list_default_style_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   return pd->style;
}

//FIXME update layout
EOLIAN static void
_efl_ui_list_homogeneous_set(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd, Eina_Bool homogeneous)
{
   pd->homogeneous = homogeneous;
}

EOLIAN static Eina_Bool
_efl_ui_list_homogeneous_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   return pd->homogeneous;
}

EOLIAN static Efl_Ui_Theme_Apply
_efl_ui_list_elm_widget_theme_apply(Eo *obj, Efl_Ui_List_Data *pd EINA_UNUSED)
{
   return elm_obj_widget_theme_apply(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_list_efl_gfx_position_set(Eo *obj, Efl_Ui_List_Data *pd, Eina_Position2D p)
{
   int pan_x, pan_y;
   if (_evas_object_intercept_call(obj, EVAS_OBJECT_INTERCEPT_CB_MOVE, 0, p.x, p.y))
     return;

   efl_gfx_position_set(efl_super(obj, MY_CLASS), p);

   elm_pan_pos_get(pd->pan_obj, &pan_x, &pan_y);
   evas_object_move(pd->hit_rect, p.x, p.y);
   evas_object_move(pd->pan_obj, p.x - pan_x, p.y - pan_y);
   evas_object_smart_changed(pd->obj);
}

EOLIAN static void
_efl_ui_list_elm_interface_scrollable_region_bring_in(Eo *obj, Efl_Ui_List_Data *pd, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
{
   int pan_x, pan_y;
   elm_pan_pos_get(pd->pan_obj, &pan_x, &pan_y);
   elm_interface_scrollable_region_bring_in(efl_super(obj, MY_CLASS), x + pan_x, y + pan_y, w, h);
}

EOLIAN static void
_efl_ui_list_efl_gfx_size_set(Eo *obj, Efl_Ui_List_Data *pd, Eina_Size2D size)
{
   DBG("w: %d h: %d", size.w, size.h);
   if (_evas_object_intercept_call(obj, EVAS_OBJECT_INTERCEPT_CB_RESIZE, 0, size.w, size.h))
     return;

   evas_object_resize(pd->hit_rect, size.w, size.h);
   efl_gfx_size_set(efl_super(obj, MY_CLASS), size);

   evas_object_smart_changed(pd->obj);
}

EOLIAN static void
_efl_ui_list_efl_canvas_group_group_calculate(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   _layout(pd);
}

EOLIAN static void
_efl_ui_list_efl_canvas_group_group_member_add(Eo *obj, Efl_Ui_List_Data *pd, Evas_Object *member)
{
   efl_canvas_group_member_add(efl_super(obj, MY_CLASS), member);

   if (pd->hit_rect)
     evas_object_raise(pd->hit_rect);
}

EOLIAN static void
_efl_ui_list_efl_canvas_group_group_add(Eo *obj, Efl_Ui_List_Data *pd)
{
   Efl_Ui_List_Pan_Data *pan_data;
   Evas_Coord minw, minh;

   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   efl_canvas_group_add(efl_super(obj, MY_CLASS));
   elm_widget_sub_object_parent_add(obj);

   elm_widget_can_focus_set(obj, EINA_TRUE);

   pd->hit_rect = evas_object_rectangle_add(evas_object_evas_get(obj));
   evas_object_data_set(pd->hit_rect, "_elm_leaveme", obj);
   evas_object_smart_member_add(pd->hit_rect, obj);
   elm_widget_sub_object_add(obj, pd->hit_rect);

   /* common scroller hit rectangle setup */
   evas_object_color_set(pd->hit_rect, 0, 0, 0, 0);
   evas_object_show(pd->hit_rect);
   evas_object_repeat_events_set(pd->hit_rect, EINA_TRUE);

   elm_widget_on_show_region_hook_set(obj, NULL, _show_region_hook, NULL);

   if (!elm_layout_theme_set(obj, "list", "base", elm_widget_style_get(obj)))
     CRI("Failed to set layout!");

   elm_interface_scrollable_objects_set(obj, wd->resize_obj, pd->hit_rect);
   elm_interface_scrollable_bounce_allow_set
           (obj, EINA_FALSE, _elm_config->thumbscroll_bounce_enable);

   pd->mode = ELM_LIST_COMPRESS;

   elm_interface_atspi_accessible_type_set(obj, ELM_ATSPI_TYPE_DISABLED);
   pd->pan_obj = efl_add(MY_PAN_CLASS, evas_object_evas_get(obj));
   pan_data = efl_data_scope_get(pd->pan_obj, MY_PAN_CLASS);
   pan_data->wobj = obj;

   elm_interface_scrollable_extern_pan_set(obj, pd->pan_obj);
   evas_object_show(pd->pan_obj);

   edje_object_size_min_calc(wd->resize_obj, &minw, &minh);
   evas_object_size_hint_min_set(obj, minw, minh);

   elm_layout_sizing_eval(obj);
}

EOLIAN static void
_efl_ui_list_efl_canvas_group_group_del(Eo *obj, Efl_Ui_List_Data *pd)
{
   //_efl_ui_list_children_free(obj, pd);

   ELM_SAFE_FREE(pd->pan_obj, evas_object_del);
   efl_canvas_group_del(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_list_efl_ui_focus_manager_focus_set(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd, Efl_Ui_Focus_Object *focus)
{
    efl_ui_focus_manager_focus_set(pd->manager, focus);
}

EOLIAN static Efl_Ui_Focus_Manager*
_efl_ui_list_elm_widget_focus_manager_create(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd EINA_UNUSED, Efl_Ui_Focus_Object *root)
{
   if (!pd->manager)
     pd->manager = efl_add(EFL_UI_FOCUS_MANAGER_CALC_CLASS, obj,
                          efl_ui_focus_manager_root_set(efl_added, root));

   return pd->manager;
}

EOLIAN static Eo *
_efl_ui_list_efl_object_finalize(Eo *obj, Efl_Ui_List_Data *pd)
{
   if(!pd->relayout)
     {
        pd->relayout = efl_add(EFL_UI_LIST_PRECISE_LAYOUTER_CLASS, obj);
        efl_ui_list_relayout_model_set(pd->relayout, pd->model);
     }
   return obj;
}

EOLIAN static Eo *
_efl_ui_list_efl_object_constructor(Eo *obj, Efl_Ui_List_Data *pd)
{
   Efl_Ui_Focus_Manager *manager;

   obj = efl_constructor(efl_super(obj, MY_CLASS));
   pd->obj = obj;
   efl_canvas_object_type_set(obj, MY_CLASS_NAME);
   evas_object_smart_callbacks_descriptions_set(obj, _smart_callbacks);
   elm_interface_atspi_accessible_role_set(obj, ELM_ATSPI_ROLE_LIST);

   efl_ui_list_segarray_setup(&pd->segarray, 32);

   manager = elm_obj_widget_focus_manager_create(obj, obj);
   efl_composite_attach(obj, manager);
   _efl_ui_focus_manager_redirect_events_add(manager, obj);

   pd->style = eina_stringshare_add(elm_widget_style_get(obj));

   pd->factory = NULL;
   pd->orient = EFL_ORIENT_DOWN;
   pd->align.h = 0;
   pd->align.v = 0;
   pd->min.w = 0;
   pd->min.h = 0;

   return obj;
}

EOLIAN static void
_efl_ui_list_efl_object_destructor(Eo *obj, Efl_Ui_List_Data *pd)
{
   efl_unref(pd->model);
   eina_stringshare_del(pd->style);

   efl_destructor(efl_super(obj, MY_CLASS));
}

EOLIAN static void
_efl_ui_list_layout_factory_set(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd, Efl_Ui_Factory *factory)
{
   //TODO: clean all current layouts??
   if (pd->factory)
     efl_unref(pd->factory);

   pd->factory = efl_ref(factory);
}

EOLIAN static void
_efl_ui_list_efl_ui_view_model_set(Eo *obj, Efl_Ui_List_Data *pd, Efl_Model *model)
{
   if (pd->model == model)
     return;

   if (pd->relayout)
      efl_ui_list_relayout_model_set(pd->relayout, model);

   if (pd->count_future)
     {
        efl_future_cancel(pd->count_future);
        pd->count_future = NULL;
     }

   if (pd->model)
     {
        efl_event_callback_del(pd->model, EFL_MODEL_EVENT_CHILD_ADDED, _child_added_cb, obj);
        efl_event_callback_del(pd->model, EFL_MODEL_EVENT_CHILD_REMOVED, _child_removed_cb, obj);
        //TODO: FIXME: XXX: SegArray Clear
        efl_ui_list_segarray_setup(&pd->segarray, 32);
        efl_unref(pd->model);
        pd->model = NULL;
     }

   if (!pd->factory)
     pd->factory = efl_add(EFL_UI_LAYOUT_FACTORY_CLASS, obj);

   if (model)
     {
        pd->model = model;
        efl_ref(pd->model);
        efl_event_callback_add(pd->model, EFL_MODEL_EVENT_CHILD_ADDED, _child_added_cb, obj);
        efl_event_callback_add(pd->model, EFL_MODEL_EVENT_CHILD_REMOVED, _child_removed_cb, obj);
        pd->count_future = efl_model_children_count_get(pd->model);
        efl_future_then(pd->count_future, &_count_then, &_count_error, NULL, pd);
     }

   evas_object_smart_changed(pd->obj);
}

EOLIAN static Efl_Model *
_efl_ui_list_efl_ui_view_model_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   return pd->model;
}

EOLIAN const Elm_Atspi_Action *
_efl_ui_list_elm_interface_atspi_widget_action_elm_actions_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd EINA_UNUSED)
{
   static Elm_Atspi_Action atspi_actions[] = {
          { "move,prior", "move", "prior", _key_action_move},
          { "move,next", "move", "next", _key_action_move},
          { "move,up", "move", "up", _key_action_move},
          { "move,up,multi", "move", "up_multi", _key_action_move},
          { "move,down", "move", "down", _key_action_move},
          { "move,down,multi", "move", "down_multi", _key_action_move},
          { "move,first", "move", "first", _key_action_move},
          { "move,last", "move", "last", _key_action_move},
          { "select", "select", NULL, _key_action_select},
          { "select,multi", "select", "multi", _key_action_select},
          { "escape", "escape", NULL, _key_action_escape},
          { NULL, NULL, NULL, NULL }
   };

   return &atspi_actions[0];
}

EOLIAN Eina_List*
_efl_ui_list_elm_interface_atspi_accessible_children_get(Eo *obj, Efl_Ui_List_Data *pd)
{
   //Efl_Ui_List_Item **litem;
   Eina_List *ret = NULL, *ret2 = NULL;

   /* EINA_INARRAY_FOREACH(&pd->items.array, litem) */
   /*    ret = eina_list_append(ret, (*litem)->layout); */

   ret2 = elm_interface_atspi_accessible_children_get(efl_super(obj, MY_CLASS));
   return eina_list_merge(ret, ret2);
}

EOLIAN int
_efl_ui_list_efl_access_selection_selected_children_count_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   return eina_list_count(pd->selected_items);
}

EOLIAN Eo*
_efl_ui_list_efl_access_selection_selected_child_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd, int child_index)
{
   if(child_index <  (int) eina_list_count(pd->selected_items))
     {
        Efl_Ui_List_Item* items = eina_list_nth(pd->selected_items, child_index);
        return items[child_index].item.layout;
     }
   else
     return NULL;
}

EOLIAN Eina_Bool
_efl_ui_list_efl_access_selection_child_select(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd EINA_UNUSED, int child_index EINA_UNUSED)
{
   /* if (pd->select_mode != ELM_OBJECT_SELECT_MODE_NONE) */
   /*   { */
   /*     if(child_index < eina_inlist_count(&pd->items.array)) */
   /*       { */
   /*          Efl_Ui_List_Item* items = pd->items.array.members; */
   /*          _efl_ui_list_item_select_set(&items[child_index], EINA_TRUE); */
   /*          return EINA_TRUE; */
   /*       } */
   /*   } */
   return EINA_FALSE;
}

EOLIAN Eina_Bool
_efl_ui_list_efl_access_selection_selected_child_deselect(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd EINA_UNUSED, int child_index EINA_UNUSED)
{
/*    Efl_Ui_List_Item *item = eina_list_nth(pd->selected, child_index); */
/*    if (item) */
/*      { */
/*         _efl_ui_list_item_select_set(pd, item->layout, EINA_FALSE); */
/*         return EINA_TRUE; */
/*      } */
   return EINA_FALSE;
}

EOLIAN Eina_Bool
_efl_ui_list_efl_access_selection_is_child_selected(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd, int child_index)
{
   /* Efl_Ui_List_Item **item = eina_inarray_nth(&pd->items.array, child_index); */
   /* EINA_SAFETY_ON_NULL_RETURN_VAL(*item, EINA_FALSE); */
   return EINA_FALSE;// (*item)->selected;
}

EOLIAN Eina_Bool
_efl_ui_list_efl_access_selection_all_children_select(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   /* Efl_Ui_List_Item **item; */

   /* if (pd->select_mode == ELM_OBJECT_SELECT_MODE_NONE) */
   /*   return EINA_FALSE; */

   /* EINA_INARRAY_FOREACH(&pd->items.array, item) */
   /*    _efl_ui_list_item_select_set(*item, EINA_TRUE); */

   return EINA_TRUE;
}

EOLIAN Eina_Bool
_efl_ui_list_efl_access_selection_clear(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   //Efl_Ui_List_Item *item;
   //Eina_List *l;

   /* if (pd->select_mode == ELM_OBJECT_SELECT_MODE_NONE) */
   /*   return EINA_FALSE; */

   /* EINA_LIST_FOREACH(pd->selected_items, l, item) */
   /*    _efl_ui_list_item_select_set(item, EINA_FALSE); */

   return EINA_TRUE;
}

EOLIAN Eina_Bool
_efl_ui_list_efl_access_selection_child_deselect(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd EINA_UNUSED, int child_index EINA_UNUSED)
{
   /* Efl_Ui_List_Item *item = eina_array_data_get(pd->items.array, child_index); */
   /* if (item) */
   /*   { */
   /*    _efl_ui_list_item_select_set(item, EINA_FALSE); */
   /*      return EINA_TRUE; */
   /*   } */

   return EINA_FALSE;
}

static Eina_Bool
_key_action_move(Evas_Object *obj, const char *params)
{
/*
   EFL_UI_LIST_DATA_GET_OR_RETURN_VAL(obj, pd, EINA_FALSE);
   const char *dir = params;

   Evas_Coord page_x, page_y;
   Evas_Coord v_w, v_h;
   Evas_Coord x, y;
   elm_interface_scrollable_content_pos_get(obj, &x, &y);
   elm_interface_scrollable_page_size_get(obj, &page_x, &page_y);
   elm_interface_scrollable_content_viewport_geometry_get(obj, NULL, NULL, &v_w, &v_h);
   if (!strcmp(dir, "up") || !strcmp(dir, "up_multi"))
     {
        efl_ui_focus_manager_move(pd->manager, EFL_UI_FOCUS_DIRECTION_UP);
     }
   else if (!strcmp(dir, "down") || !strcmp(dir, "down_multi"))
     {
        efl_ui_focus_manager_move(pd->manager, EFL_UI_FOCUS_DIRECTION_DOWN);
     }
   if (!strcmp(dir, "left"))
     {
        efl_ui_focus_manager_move(pd->manager, EFL_UI_FOCUS_DIRECTION_LEFT);
     }
   else if (!strcmp(dir, "right"))
     {
        efl_ui_focus_manager_move(pd->manager, EFL_UI_FOCUS_DIRECTION_RIGHT);
     }

*/
/*
   else if (!strcmp(dir, "first"))
     {
        item = eina_list_data_get(pd->items);
        x = 0;
        y = 0;
        elm_widget_focus_next_object_set(obj, item->layout, ELM_FOCUS_UP);
     }
   else if (!strcmp(dir, "last"))
     {
        item = eina_list_data_get(eina_list_last(pd->items));
        elm_obj_pan_pos_max_get(pd->pan.obj, &x, &y);
     }
   else if (!strcmp(dir, "prior"))
     {
        if (_horiz(pd->orient))
          {
             if (page_x < 0)
               x -= -(page_x * v_w) / 100;
             else
               x -= page_x;
          }
        else
          {
             if (page_y < 0)
               y -= -(page_y * v_h) / 100;
             else
               y -= page_y;
          }
     }
   else if (!strcmp(dir, "next"))
     {
        if (_horiz(pd->orient))
          {
             if (page_x < 0)
               x += -(page_x * v_w) / 100;
             else
               x += page_x;
          }
        else
          {
             if (page_y < 0)
               y += -(page_y * v_h) / 100;
             else
               y += page_y;
          }
     }
*/
//   else
     return EINA_FALSE;

//   elm_interface_scrollable_content_pos_set(obj, x, y, EINA_TRUE);
//   return EINA_TRUE;
}

static Eina_Bool
_key_action_select(Evas_Object *obj, const char *params EINA_UNUSED)
{
/*   EFL_UI_LIST_DATA_GET_OR_RETURN_VAL(obj, pd, EINA_FALSE);

   Eo *focused = efl_ui_focus_manager_focused(pd->manager);
   if (focused)
     _efl_ui_list_item_select_set(item, EINA_TRUE);
*/
   return EINA_FALSE;
}

static Eina_Bool
_key_action_escape(Evas_Object *obj, const char *params EINA_UNUSED)
{
   efl_ui_focus_manager_reset_history(obj);
   return EINA_TRUE;
}

ELM_WIDGET_KEY_DOWN_DEFAULT_IMPLEMENT(efl_ui_list, Efl_Ui_List_Data)

void
_efl_ui_list_item_select_set(Efl_Ui_List_LayoutItem *item, Eina_Bool selected)
{
   Eina_Stringshare *sprop;
   assert(item != NULL);
   assert(item->children != NULL);

   selected = !!selected;
   /* if (!item->model || item->selected == selected) return; */

   sprop = eina_stringshare_add(SELECTED_PROP);

   if (_efl_model_properties_has(item->children, sprop))
     {
        Eina_Value v;
        eina_value_setup(&v, EINA_VALUE_TYPE_UCHAR);
        eina_value_set(&v, selected);
        efl_model_property_set(item->children, sprop, &v);
        eina_value_flush(&v);
     }
   eina_stringshare_del(sprop);

   //TODO I need call this event or catch only by model connect event?
/*
   if (selected)
     efl_event_callback_legacy_call(item->layout, EFL_UI_EVENT_SELECTED, NULL);
   else
     efl_event_callback_legacy_call(item->layout, EFL_UI_EVENT_UNSELECTED, NULL);
*/
}

static void
_efl_ui_list_relayout_set(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd EINA_UNUSED, Efl_Ui_List_Relayout *object)
{
   if(pd->relayout)
     efl_unref(pd->relayout);
   pd->relayout = efl_ref(object);
}

static Efl_Ui_List_Relayout *
_efl_ui_list_relayout_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd EINA_UNUSED)
{
   return pd->relayout;
}

static void
_layout(Efl_Ui_List_Data *pd)
{
   if (!pd->model)
     return;

   efl_ui_list_relayout_layout_do(pd->relayout, pd->obj, pd->segarray_first, &pd->segarray);
}

static void
_children_slice_then(void * data, Efl_Event const* event)
{
   Efl_Ui_List_Data *pd = data;
   Eina_Accessor *acc = (Eina_Accessor*)((Efl_Future_Event_Success*)event->info)->value;

   efl_ui_list_segarray_insert_accessor(&pd->segarray, pd->outstanding_slice.slice_start, acc);

   pd->segarray_first = pd->outstanding_slice.slice_start;
   pd->outstanding_slice.slice_start = pd->outstanding_slice.slice_count = 0;
   pd->slice_future = NULL;
}

/* EFL UI LIST MODEL INTERFACE */
EOLIAN static Eina_Size2D
_efl_ui_list_efl_ui_list_model_min_size_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
   return pd->min;
}

EOLIAN static void
_efl_ui_list_efl_ui_list_model_min_size_set(Eo *obj, Efl_Ui_List_Data *pd, Eina_Size2D min)
{
   ELM_WIDGET_DATA_GET_OR_RETURN(obj, wd);

   pd->min.w = min.w;
   pd->min.h = min.h;

   evas_object_size_hint_min_set(wd->resize_obj, pd->min.w, pd->min.h);
   efl_event_callback_legacy_call(pd->pan_obj, ELM_PAN_EVENT_CHANGED, NULL);
}

EOLIAN static Efl_Ui_List_LayoutItem *
_efl_ui_list_efl_ui_list_model_realize(Eo *obj, Efl_Ui_List_Data *pd, Efl_Ui_List_LayoutItem *item)
{
   Efl_Ui_List_Item_Event evt;
   EINA_SAFETY_ON_NULL_RETURN_VAL(item->children, item);

   item->layout = efl_ui_factory_create(pd->factory, item->children, obj);
   evas_object_smart_member_add(item->layout, pd->pan_obj);
   evas_object_event_callback_add(item->layout, EVAS_CALLBACK_MOUSE_UP, _on_item_mouse_up, item);

   evt.child = item->children;
   evt.layout = item->layout;
   evt.index = efl_ui_list_item_index_get((Efl_Ui_List_Item *)item);
   efl_event_callback_call(obj, EFL_UI_LIST_EVENT_ITEM_REALIZED, &evt);

   evas_object_show(item->layout);
   return item;
}

EOLIAN static void
_efl_ui_list_efl_ui_list_model_unrealize(Eo *obj, Efl_Ui_List_Data *pd, Efl_Ui_List_LayoutItem *item)
{
   Efl_Ui_List_Item_Event evt;
   EINA_SAFETY_ON_NULL_RETURN(item->layout);

   evas_object_event_callback_del_full(item->layout, EVAS_CALLBACK_MOUSE_UP, _on_item_mouse_up, item);
   evas_object_smart_member_del(item->layout);
   evas_object_hide(item->layout);
   evas_object_move(item->layout, -9999, -9999);

   if (elm_object_focus_allow_get(item->layout))
     {
        if (elm_widget_focus_get(item->layout))
          {
             efl_ui_focus_manager_reset_history(obj);
          }
     }

   evt.child = item->children;
   evt.layout = item->layout;
   evt.index = efl_ui_list_item_index_get((Efl_Ui_List_Item *)item);
   efl_event_callback_call(obj, EFL_UI_LIST_EVENT_ITEM_UNREALIZED, &evt);

   efl_ui_factory_release(pd->factory, item->layout);
   item->layout = NULL;
}

EOLIAN static void
_efl_ui_list_efl_ui_list_model_load_range_set(Eo* obj EINA_UNUSED, Efl_Ui_List_Data* pd, int first, int count)
{
   //FIXME?? slice cancel??
   if(!pd->slice_future)
     {
        pd->slice_future = efl_model_children_slice_get(pd->model, first, count);
        pd->outstanding_slice.slice_start = first;
        pd->outstanding_slice.slice_count = count;
        efl_future_then(pd->slice_future, &_children_slice_then, &_children_slice_error, NULL, pd);
     }
}

EOLIAN static int
_efl_ui_list_efl_ui_list_model_size_get(Eo *obj EINA_UNUSED, Efl_Ui_List_Data *pd)
{
    /* TODO */
    return pd->item_count;
}

/* Internal EO APIs and hidden overrides */

#define EFL_UI_LIST_EXTRA_OPS \
   EFL_CANVAS_GROUP_ADD_DEL_OPS(efl_ui_list)

#include "efl_ui_list.eo.c"
#include "efl_ui_list_relayout.eo.c"
#include "efl_ui_list_model.eo.c"
