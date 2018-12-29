#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Elementary.h>
#include "elm_priv.h"

const char *model_itemw = "Item.Width";
const char *model_itemh = "Item.Height";
const char *model_selfw = "Self.Width";
const char *model_selfh = "Self.Height";
const char *model_totalw = "Total.Width";
const char *model_totalh = "Total.Height";

static Eina_Iterator *
_efl_ui_model_size_properties_child(void)
{
   const char *properties[] = {
     model_itemw, model_itemh, model_selfh, model_selfw
   };
   return EINA_C_ARRAY_ITERATOR_NEW(properties);
}

static Eina_Iterator *
_efl_ui_model_size_properties_root(void)
{
   const char *properties[] = {
     model_itemw, model_itemh
   };
   return EINA_C_ARRAY_ITERATOR_NEW(properties);
}

static Eina_Iterator *
_efl_ui_model_size_efl_model_properties_get(const Eo *obj, void *pd EINA_UNUSED)
{
   Eina_Iterator *super;
   Eina_Iterator *prop;

   super = efl_model_properties_get(efl_super(obj, EFL_UI_MODEL_SIZE_CLASS));
   if (efl_isa(efl_parent_get(obj), EFL_UI_MODEL_SIZE_CLASS))
     prop = _efl_ui_model_size_properties_child();
   else
     prop = _efl_ui_model_size_properties_root();

   return eina_multi_iterator_new(super, prop);
}

#include "efl_ui_model_size.eo.c"
