#ifndef _ELM_SUITE_H
#define _ELM_SUITE_H

#include <check.h>
#define EFL_NOLEGACY_API_SUPPORT
#include <Efl_Ui.h>
#include "../efl_check.h"

#define ck_assert_strn_eq(s1, s2, len)          \
  {                                             \
    char expected[len+1], actual[len+1];        \
                                                \
    strncpy(expected, s1, len);                 \
    expected[len] = '\0';                       \
    strncpy(actual, s2, len);                   \
    actual[len] = '\0';                         \
                                                \
    ck_assert_str_eq(expected, actual);         \
  }

void efl_ui_model(TCase *tc);

#endif /* _ELM_SUITE_H */
