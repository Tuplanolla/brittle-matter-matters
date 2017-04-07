#include <errno.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "ext.h"
#include "hack.h"
#include "sec.h"
#include "tle.h"

struct tle {
  char const* prog;
  double sec;
  enum {STD, EXT} tag;
  union {
    int std;
    enum bmm_tle ext;
  } num;
  char buf[BUFSIZ];
};

static _Thread_local struct tle tle = {
  .prog = NULL,
  .sec = NAN,
  .tag = EXT,
  .num.std = BMM_TLE_SUCCESS,
  .buf = "Success"
};

void bmm_tle_reset(char const* const prog) {
  char const* const str = strrchr(prog, '/');

  tle.prog = str == NULL ? prog : &str[1];

  tle.sec = bmm_sec_now();
}

int bmm_tle_num_std(void) {
  switch (tle.tag) {
    case STD:
      return tle.num.std;
    case EXT:
      return 0;
  }
}

enum bmm_tle bmm_tle_num_ext(void) {
  switch (tle.tag) {
    case STD:
      return BMM_TLE_SUCCESS;
    case EXT:
      return tle.num.ext;
  }
}

char const* bmm_tle_msg(void) {
  return tle.buf;
}

static void init(void) {
  if (tle.prog == NULL) {
    tle.prog = "a.out";

    tle.sec = bmm_sec_now();
  }
}

__attribute__ ((__nonnull__))
static bool prefix(size_t* const ptr,
    char const* const file, size_t const line, char const* const proc) {
  init();

  int const i = snprintf(tle.buf, sizeof tle.buf,
      "[%f] %s (%zu): %s (%s:%zu): ",
      bmm_sec_now() - tle.sec, tle.prog, (size_t) getpid(), proc, file, line);

  if (i < 0)
    return false;

  size_t const n = (size_t) i;

  if (n >= sizeof tle.buf)
    return false;

  if (ptr != NULL)
    *ptr = n;

  return true;
}

static void suffix(void) {
  static char const buf[] = "Cannot report error";

  static_assert(sizeof tle.buf >= sizeof buf, "Buffer too short");

  (void) strcpy(tle.buf, buf);
}

static bool suffix_std(size_t const n) {
  tle.tag = STD;

  tle.num.std = errno;

  if (!bmm_hack_strerror_r(errno, &tle.buf[n], sizeof tle.buf - n)) {
    tle.num.std = errno;

    if (!bmm_hack_strerror_r(errno, &tle.buf[n], sizeof tle.buf - n)) {
      tle.num.std = errno;

      return false;
    }
  }

  return true;
}

__attribute__ ((__format__ (__printf__, 3, 0), __nonnull__))
static bool suffix_ext(size_t const n,
    enum bmm_tle const num, char const* const fmt, va_list ap) {
  tle.tag = EXT;

  tle.num.ext = num;

  size_t const m = sizeof tle.buf - n;

  int const i = vsnprintf(&tle.buf[n], m, fmt, ap);

  if (i < 0)
    return false;

  size_t const k = (size_t) i;

  if (k >= m)
    return false;

  return true;
}

void bmm_tle_std(void) {
  if (!suffix_std(0))
    suffix();
}

void bmm_tle_stds(char const* const file, size_t const line,
    char const* const proc) {
  size_t n;

  if (!prefix(&n, file, line, proc)) {
    bmm_tle_std();

    return;
  }

  if (!suffix_std(n))
    suffix();
}

void bmm_tle_vext(enum bmm_tle const num, char const* const fmt, va_list ap) {
  tle.tag = EXT;

  tle.num.ext = num;

  int const i = vsnprintf(tle.buf, sizeof tle.buf, fmt, ap);
  if (i < 0 || (size_t) i >= sizeof tle.buf)
    bmm_tle_std();
}

void bmm_tle_ext(enum bmm_tle const num, char const* const fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  bmm_tle_vext(num, fmt, ap);
  va_end(ap);
}

void bmm_tle_vexts(char const* const file, size_t const line,
    char const* const proc,
    enum bmm_tle const num, char const* const fmt, va_list ap) {
  size_t n;

  if (!prefix(&n, file, line, proc)) {
    if (!suffix_std(0))
      suffix();

    return;
  }

  if (!suffix_ext(n, num, fmt, ap))
    if (!suffix_std(n))
      suffix();
}

void bmm_tle_exts(char const* const file, size_t const line,
    char const* const proc,
    enum bmm_tle const num, char const* const fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  bmm_tle_vexts(file, line, proc, num, fmt, ap);
  va_end(ap);
}