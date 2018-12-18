#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "Efl.h"

#include "efl_mono_model_internal.eo.h"

typedef struct _Efl_Mono_Model_Internal_Data Efl_Mono_Model_Internal_Data;

typedef struct _Properties_Info Properties_Info;
struct _Properties_Info
{
  const char* name;
  Eina_Value_Type* type;
};

struct _Efl_Mono_Model_Internal_Data
{
  Eina_Array *properties_info;
};

#define MY_CLASS EFL_MONO_MODEL_INTERNAL_CLASS

static Eo *
_efl_mono_model_internal_efl_object_constructor(Eo *obj, Efl_Mono_Model_Internal_Data *pd)
{
   obj = efl_constructor(efl_super(obj, MY_CLASS));

   pd->properties_info = eina_array_new(5);

   if (!obj) return NULL;

   return obj;
}

static void
_efl_mono_model_internal_efl_object_destructor(Eo *obj, Efl_Mono_Model_Internal_Data *pd)
{
   efl_destructor(efl_super(obj, MY_CLASS));
}

static void
_efl_mono_model_internal_add_property(Eo *obj, Efl_Mono_Model_Internal_Data *pd, const char *name, void *type)
{
  fprintf(stderr, "add property name : %s\n", name); fflush(stderr);
  
  Properties_Info* info = malloc(sizeof(Properties_Info));
  info->name = eina_stringshare_add(name);
  info->type = type;
  eina_array_push (pd->properties_info, info);
}


#include "efl_mono_model_internal.eo.c"
