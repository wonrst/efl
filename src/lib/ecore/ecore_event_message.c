#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Ecore.h>

#include "ecore_private.h"

#define MY_CLASS ECORE_EVENT_MESSAGE_CLASS

//////////////////////////////////////////////////////////////////////////

typedef struct _Ecore_Event_Message_Data Ecore_Event_Message_Data;

struct _Ecore_Event_Message_Data
{
   int type;
   void *ev;
   Ecore_End_Cb free_func;
   void *data;
};

//////////////////////////////////////////////////////////////////////////

EOLIAN static void
_ecore_event_message_data_set(Eo *obj EINA_UNUSED, Ecore_Event_Message_Data *pd, int type, void *data, void *free_func, void *free_data)
{
   pd->type = type;
   pd->ev = data;
   pd->free_func = free_func;
   pd->data = free_data;
}

EOLIAN static void
_ecore_event_message_data_get(const Eo *obj EINA_UNUSED, Ecore_Event_Message_Data *pd, int *type, void **data, void **free_func, void **free_data)
{
   if (type) *type = pd->type;
   if (data) *data = pd->ev;
   if (free_func) *free_func = pd->free_func;
   if (free_data) *free_data = pd->data;
}

EOLIAN static void
_ecore_event_message_data_steal(Eo *obj EINA_UNUSED, Ecore_Event_Message_Data *pd, int *type, void **data, void **free_func, void **free_data)
{
   if (type) *type = pd->type;
   if (data) *data = pd->ev;
   if (free_func) *free_func = pd->free_func;
   if (free_data) *free_data = pd->data;
   pd->type = -1;
   pd->ev = NULL;
   pd->free_func = NULL;
   pd->data = NULL;
}

EOLIAN static Efl_Object *
_ecore_event_message_efl_object_constructor(Eo *obj, Ecore_Event_Message_Data *pd EINA_UNUSED)
{
   obj = efl_constructor(efl_super(obj, MY_CLASS));
   pd->type = -1;
   return obj;
}

EOLIAN static void
_ecore_event_message_efl_object_destructor(Eo *obj EINA_UNUSED, Ecore_Event_Message_Data *pd EINA_UNUSED)
{
   if (pd->ev)
     {
        Ecore_End_Cb fn_free = pd->free_func;
        void *ev = pd->ev;

        pd->ev = NULL;
        if (fn_free) fn_free(pd->data, ev);
        else free(ev);
     }
   efl_destructor(efl_super(obj, MY_CLASS));
}

//////////////////////////////////////////////////////////////////////////

void _ecore_event_message_data_set(Eo *obj, Ecore_Event_Message_Data *pd, int type, void *data, void *free_func, void *free_data);

EOAPI EFL_VOID_FUNC_BODYV(ecore_event_message_data_set, EFL_FUNC_CALL(type, data, free_func, free_data), int type, void *data, void *free_func, void *free_data);

void _ecore_event_message_data_get(const Eo *obj, Ecore_Event_Message_Data *pd, int *type, void **data, void **free_func, void **free_data);

EOAPI EFL_VOID_FUNC_BODYV_CONST(ecore_event_message_data_get, EFL_FUNC_CALL(type, data, free_func, free_data), int *type, void **data, void **free_func, void **free_data);

void _ecore_event_message_data_steal(Eo *obj, Ecore_Event_Message_Data *pd, int *type, void **data, void **free_func, void **free_data);

EOAPI EFL_VOID_FUNC_BODYV(ecore_event_message_data_steal, EFL_FUNC_CALL(type, data, free_func, free_data), int *type, void **data, void **free_func, void **free_data);

Efl_Object *_ecore_event_message_efl_object_constructor(Eo *obj, Ecore_Event_Message_Data *pd);


void _ecore_event_message_efl_object_destructor(Eo *obj, Ecore_Event_Message_Data *pd);


static Eina_Bool
_ecore_event_message_class_initializer(Efl_Class *klass)
{
   const Efl_Object_Ops *opsp = NULL, *copsp = NULL;

#ifndef ECORE_EVENT_MESSAGE_EXTRA_OPS
#define ECORE_EVENT_MESSAGE_EXTRA_OPS
#endif

   EFL_OPS_DEFINE(ops,
      EFL_OBJECT_OP_FUNC(ecore_event_message_data_set, _ecore_event_message_data_set),
      EFL_OBJECT_OP_FUNC(ecore_event_message_data_get, _ecore_event_message_data_get),
      EFL_OBJECT_OP_FUNC(ecore_event_message_data_steal, _ecore_event_message_data_steal),
      EFL_OBJECT_OP_FUNC(efl_constructor, _ecore_event_message_efl_object_constructor),
      EFL_OBJECT_OP_FUNC(efl_destructor, _ecore_event_message_efl_object_destructor),
      ECORE_EVENT_MESSAGE_EXTRA_OPS
   );
   opsp = &ops;

#ifdef ECORE_EVENT_MESSAGE_EXTRA_CLASS_OPS
   EFL_OPS_DEFINE(cops, ECORE_EVENT_MESSAGE_EXTRA_CLASS_OPS);
   copsp = &cops;
#endif

   return efl_class_functions_set(klass, opsp, copsp);
}

static const Efl_Class_Description _ecore_event_message_class_desc = {
   EO_VERSION,
   "Ecore.Event.Message",
   EFL_CLASS_TYPE_REGULAR,
   sizeof(Ecore_Event_Message_Data),
   _ecore_event_message_class_initializer,
   NULL,
   NULL
};

EFL_DEFINE_CLASS(ecore_event_message_class_get, &_ecore_event_message_class_desc, EFL_LOOP_MESSAGE_CLASS, NULL);
