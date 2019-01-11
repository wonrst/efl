#ifndef EOLIAN_MONO_VARIABLE_DEFINITION_HH
#define EOLIAN_MONO_VARIABLE_DEFINITION_HH

#include <Eina.hh>

#include "grammar/generator.hpp"
#include "grammar/klass_def.hpp"

#include "grammar/indentation.hpp"
#include "grammar/list.hpp"
#include "grammar/alternative.hpp"
#include "grammar/attribute_reorder.hpp"
#include "logging.hh"
#include "type.hh"
#include "name_helpers.hh"
#include "helpers.hh"
#include "function_helpers.hh"
#include "marshall_type.hh"
#include "parameter.hh"
#include "documentation.hh"
#include "using_decl.hh"
#include "generation_contexts.hh"
#include "blacklist.hh"

namespace eolian_mono {


struct constant_definition_generator
{
  template<typename OutputIterator, typename Context>
  bool generate(OutputIterator sink, attributes::variable_def constant, Context context) const
  {
    EINA_LOG_ERR("Would be generating variable [%s]", constant.full_name.c_str());
    return true;


  }
} const constant_definition;

}

namespace efl { namespace eolian { namespace grammar {

template <>
struct is_eager_generator< ::eolian_mono::constant_definition_generator> : std::true_type {};
template <>
struct is_generator< ::eolian_mono::constant_definition_generator> : std::true_type {};

namespace type_traits {
template <>
struct attributes_needed< ::eolian_mono::constant_definition_generator> : std::integral_constant<int, 1> {};
}

} } }

#endif
