#include "errors.h"
#include "strs.h"
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>

bool bmm_strtoz(char const* const str, size_t* const p) {
  char* endptr;
  errno = 0;
  long long int const x = strtoll(str, &endptr, 10);
  if (errno != 0 || endptr[0] != '\0' || x < 0) {
    bmm_error("Cannot parse '%s' as a size.", str);

    return false;
  } else {
    if (p != NULL)
      *p = (size_t) x;

    return true;
  }
}
