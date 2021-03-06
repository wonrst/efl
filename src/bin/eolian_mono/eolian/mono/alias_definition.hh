#ifndef EOLIAN_MONO_ALIAS_DEFINITION_HH
#define EOLIAN_MONO_ALIAS_DEFINITION_HH

#include "grammar/generator.hpp"
#include "grammar/klass_def.hpp"
#include "grammar/indentation.hpp"

#include "using_decl.hh"
#include "name_helpers.hh"
#include "blacklist.hh"
#include "documentation.hh"
#include "generation_contexts.hh"

namespace eolian_mono {

struct alias_definition_generator
{
  template<typename OutputIterator, typename Context>
  bool generate(OutputIterator sink, attributes::alias_def const& alias, Context const& context) const
  {
     if (blacklist::is_alias_blacklisted(alias))
       {
          EINA_CXX_DOM_LOG_DBG(eolian_mono::domain) << "Alias " <<  name_helpers::alias_full_eolian_name(alias) << "is blacklisted. Skipping.";
          return true;
       }

     if (alias.is_undefined)
       {
          EINA_CXX_DOM_LOG_DBG(eolian_mono::domain) << "Alias " <<  name_helpers::alias_full_eolian_name(alias) << "is undefined. Skipping.";
          return true;
       }

     if (!name_helpers::open_namespaces(sink, alias.namespaces, context))
       return false;

     std::string const alias_name = utils::remove_all(alias.eolian_name, '_');
     if (!as_generator(
                 "public struct " << alias_name << " {\n"
                 << scope_tab << "private " << type << " payload;\n"
                 << scope_tab << "public static implicit operator " << alias_name << "(" << type << " x)\n"
                 << scope_tab << "{\n"
                 << scope_tab << scope_tab << "return new " << alias_name << "{payload=x};\n"
                 << scope_tab << "}\n"
                 << scope_tab << "public static implicit operator " << type << "(" << alias_name << " x)\n"
                 << scope_tab << "{\n"
                 << scope_tab << scope_tab << "return x.payload;\n"
                 << scope_tab << "}\n"
                 << "}\n"
                 ).generate(sink, std::make_tuple(alias.base_type, alias.base_type, alias.base_type), context))
       return false;

     if (!name_helpers::close_namespaces(sink, alias.namespaces, context))
       return false;

     return true;
  }
} const alias_definition {};

}

namespace efl { namespace eolian { namespace grammar {

template<>
struct is_eager_generator< ::eolian_mono::alias_definition_generator> : std::true_type {};
template<>
struct is_generator< ::eolian_mono::alias_definition_generator> : std::true_type {};

namespace type_traits {

template<>
struct attributes_needed< ::eolian_mono::alias_definition_generator> : std::integral_constant<int, 1> {};

}

} } }

#endif
