#include <cheat.h>
#include <cheats.h>
#include <math.h>
#include <stdint.h>
#include <string.h>

#include "alias.h"
#include "common.h"
#include "cpp.h"
#include "endy.h"
#include "ext.h"
#include "fp.h"
#include "geom2d.h"
#include "neigh.h"
#include "msg.h"

CHEAT_DECLARE(

#define A signed_char

/// The call `bmm_tamean2(x, y)`
/// returns the truncated arithmetic mean of `x` and `y`.
/// Overflows are impossible both internally and externally.
__attribute__ ((__const__, __pure__))
static /* inline */ A type(tamean2, A)(A const x, A const y) {
  // TODO Not like this.

  A const z = (x / 2 + y / 2);

  if ((x % 2 == 0) && (y % 2 == 0))
    return z;

  if ((x % 2 == 1) && (y % 2 == 1))
    return z + (z >= 0);

  if ((x % 2 == -1) && (y % 2 == -1))
    return z - (z <= 0);

  if ((x % 2 == 1 && y % 2 == -1) ||
      (x % 2 == -1 && y % 2 == 1))
    return z;

  if ((x % 2 == 0 && y % 2 == 1) ||
      (x % 2 == 1 && y % 2 == 0))
    return z + (z < 0);

  if ((x % 2 == -1 && y % 2 == 0) ||
      (x % 2 == 0 && y % 2 == -1))
    return z - (z > 0);

  return -128;

  // The following implementation is less complicated,
  // but susceptible to overflowing.
  // return (x + y) / 2;
}

__attribute__ ((__const__, __pure__))
static int ref(int const x, int const y) {
  return (x + y) / 2;
}
)

CHEAT_TEST(ref,
  for (int i = -128; i < 128; ++i)
    for (int j = -128; j < 128; ++j) {
      signed char const x = (signed char) i;
      signed char const y = (signed char) j;

      cheat_assert_signed_char(type(tamean2, signed_char)(x, y),
          (signed char) ref(i, j));
    }
)

CHEAT_TEST(quot_sint,
  for (int i = -128; i < 128; ++i)
    for (int j = -128; j < 128; ++j)
      if (j != 0) {
        signed char const x = (signed char) i;
        signed char const y = (signed char) j;

        type(bmm_quot_t, signed_char) const qr =
          type(bmm_quot, signed_char)(x, y);
        cheat_assert_signed_char(qr.quot * y + qr.rem, x);
        cheat_assert_not_int(type(bmm_sgn, signed_char)(qr.rem), -1);
      }
)

CHEAT_TEST(quot_uint,
  for (int i = 0; i < 256; ++i)
    for (int j = 0; j < 256; ++j)
      if (j != 0) {
        unsigned char const x = (unsigned char) i;
        unsigned char const y = (unsigned char) j;

        type(bmm_quot_t, unsigned_char) const qr =
          type(bmm_quot, unsigned_char)(x, y);
        cheat_assert_unsigned_char(qr.quot * y + qr.rem, x);
        cheat_assert(qr.rem >= 0);
      }
)

CHEAT_TEST(quot_fp,
  for (int i = -128; i < 128; ++i)
    for (int j = -128; j < 128; ++j)
      if (j != 0) {
        double const x = (double) i / 64.0;
        double const y = (double) j / 64.0;

        type(bmm_quot_t, double) const qr =
          type(bmm_quot, double)(x, y);
        cheat_assert_double(qr.quot * y + qr.rem, x, 1.0e-6);
        cheat_assert(qr.rem >= 0);
      }
)

CHEAT_DECLARE(
  __attribute__ ((__const__, __pure__))
  static int wrap_ref(int const x, int const a, int const b) {
    int const c = b - a;

    int y = x;

    if (y < a)
      do
        y += c;
      while (y < a);
    else if (y >= b)
      do
        y -= c;
      while (y >= b);

    return y;
  }
)

CHEAT_TEST(wrap_sint,
  for (int i = -128; i < 128; ++i)
    for (int j = -128; j < 128; ++j)
      for (int k = j + 1; k < 128; ++k) {
        signed char const x = (signed char) i;
        signed char const a = (signed char) j;
        signed char const b = (signed char) k;

        cheat_assert_signed_char(type(bmm_wrap, signed_char)(x, a, b),
            (signed char) wrap_ref(i, j, k));
      }
)

CHEAT_TEST(wrap_uint,
  for (int i = 0; i < 256; ++i)
    for (int j = 0; j < 256; ++j)
      for (int k = j + 1; k < 128; ++k) {
        unsigned char const x = (unsigned char) i;
        unsigned char const a = (unsigned char) j;
        unsigned char const b = (unsigned char) k;

        cheat_assert_unsigned_char(type(bmm_wrap, unsigned_char)(x, a, b),
            (unsigned char) wrap_ref(i, j, k));
      }
)

CHEAT_TEST(wrap_fp,
  for (int i = -128; i < 128; ++i)
    for (int j = -128; j < 128; ++j)
      for (int k = j + 1; k < 128; ++k) {
        double const x = (double) i / 64.0;
        double const a = (double) j / 64.0;
        double const b = (double) k / 64.0;

        cheat_assert(type(bmm_wrap, double)(x, a, b) >= a);
        cheat_assert(type(bmm_wrap, double)(x, a, b) < b);
      }
)

CHEAT_TEST(uwrap_sint,
  for (int i = -128; i < 128; ++i)
    for (int j = 1; j < 128; ++j) {
      signed char const x = (signed char) i;
      signed char const b = (signed char) j;

      cheat_assert_signed_char(type(bmm_uwrap, signed_char)(x, b),
          type(bmm_wrap, signed_char)(x, 0, b));
    }
)

CHEAT_TEST(uwrap_uint,
  for (int i = 0; i < 256; ++i)
    for (int j = 1; j < 128; ++j) {
      unsigned char const x = (unsigned char) i;
      unsigned char const b = (unsigned char) j;

      cheat_assert_unsigned_char(type(bmm_uwrap, unsigned_char)(x, b),
          type(bmm_wrap, unsigned_char)(x, 0, b));
    }
)

CHEAT_TEST(uwrap_fp,
  for (int i = -128; i < 128; ++i)
    for (int j = 1; j < 128; ++j) {
      double const x = (double) i / 64.0;
      double const b = (double) j / 64.0;

      cheat_assert(type(bmm_uwrap, double)(x, b) >= 0);
      cheat_assert(type(bmm_uwrap, double)(x, b) < b);
    }
)

CHEAT_DECLARE(
  __attribute__ ((__const__, __pure__))
  static int pow_ref(int const x, size_t const e) {
    int y = 1;

    for (size_t i = 0; i < e; ++i)
      y *= x;

    return y;
  }
)

CHEAT_TEST(pow,
  for (int i = -5; i < 6; ++i)
    for (size_t j = 0; j < 6; ++j)
      cheat_assert_size(type(bmm_pow, int)(i, j), pow_ref(i, j));
)

CHEAT_TEST(mean2,
  for (int i = 1; i < 128; ++i)
    for (int j = 1; j < 128; ++j) {
      double const x = (double) i / 64.0;
      double const y = (double) j / 64.0;

      double const a = type(bmm_amean2, double)(x, y);
      double const g = type(bmm_gmean2, double)(x, y);
      double const h = type(bmm_hmean2, double)(x, y);

      cheat_assert(a >= g);
      cheat_assert(g >= h);
    }
)

CHEAT_DECLARE(
  __attribute__ ((__const__, __pure__))
  static int tamean2_ref(int const x, int const y) {
    return (x + y) / 2;
  }
)

// TODO Unskip.
CHEAT_SKIP(tmean2,
  for (int i = -128; i < 128; ++i)
    for (int j = -128; j < 128; ++j) {
      signed char const x = (signed char) i;
      signed char const y = (signed char) j;

      cheat_assert_signed_char(type(bmm_tamean2, signed_char)(x, y),
          (signed char) tamean2_ref(i, j));
    }
)

CHEAT_DECLARE(
  __attribute__ ((__const__, __pure__))
  static int famean2_ref(int const x, int const y) {
    return type(bmm_quot, int)(x + y, 2).quot;
  }
)

CHEAT_TEST(fmean2,
  for (int i = -128; i < 128; ++i)
    for (int j = -128; j < 128; ++j) {
      signed char const x = (signed char) i;
      signed char const y = (signed char) j;

      cheat_assert_signed_char(type(bmm_famean2, signed_char)(x, y),
          (signed char) famean2_ref(i, j));
    }
)

CHEAT_TEST(size_fact,
  cheat_assert_size(bmm_size_fact(0, 1), 1);
  cheat_assert_size(bmm_size_fact(1, 1), 1);
  cheat_assert_size(bmm_size_fact(2, 1), 2);
  cheat_assert_size(bmm_size_fact(3, 1), 6);
  cheat_assert_size(bmm_size_fact(4, 1), 24);
  cheat_assert_size(bmm_size_fact(5, 1), 120);
  cheat_assert_size(bmm_size_fact(6, 1), 720);
)

CHEAT_TEST(size_fact2,
  cheat_assert_size(bmm_size_fact(0, 2), 1);
  cheat_assert_size(bmm_size_fact(1, 2), 1);
  cheat_assert_size(bmm_size_fact(2, 2), 2);
  cheat_assert_size(bmm_size_fact(3, 2), 3);
  cheat_assert_size(bmm_size_fact(4, 2), 8);
  cheat_assert_size(bmm_size_fact(5, 2), 15);
  cheat_assert_size(bmm_size_fact(6, 2), 48);
)

CHEAT_TEST(size_flog,
  cheat_assert_size(bmm_size_flog(1, 2), 0);
  cheat_assert_size(bmm_size_flog(2, 2), 1);
  cheat_assert_size(bmm_size_flog(3, 2), 1);
  cheat_assert_size(bmm_size_flog(4, 2), 2);
  cheat_assert_size(bmm_size_flog(5, 2), 2);
  cheat_assert_size(bmm_size_flog(6, 2), 2);
)

CHEAT_TEST(size_clog,
  cheat_assert_size(bmm_size_clog(1, 2), 0);
  cheat_assert_size(bmm_size_clog(2, 2), 1);
  cheat_assert_size(bmm_size_clog(3, 2), 2);
  cheat_assert_size(bmm_size_clog(4, 2), 2);
  cheat_assert_size(bmm_size_clog(5, 2), 3);
  cheat_assert_size(bmm_size_clog(6, 2), 3);
)

CHEAT_DECLARE(
  __attribute__ ((__nonnull__, __pure__))
  static int compar(size_t const i, size_t const j, void *const cls) {
    int const *const x = cls;

    // return type(bmm_cmp, int)(x[i], x[j]);
    return x[i] < x[j] ? -1 : x[i] > x[j] ? 1 : 0;
  }

  __attribute__ ((__nonnull__))
  static void swap(size_t const i, size_t const j, void *const cls) {
    int *const x = cls;

    type(bmm_swap, int)(&x[i], &x[j]);
  }
)

CHEAT_TEST(hsort,
  int x[255];

  static size_t const perm[][3] = {
    {0, 1, 2},
    {5, 4, 3}
  };

  for (size_t i = 0; i < nmembof(x); ++i) {
    size_t n = i % (1 << nmembof(perm) * 2);
    size_t k = 0;

    for (size_t j = 0; j < nmembof(*perm); ++j) {
      k |= (n >> perm[0][j] & 1) << perm[1][j];
      k |= (n >> perm[1][j] & 1) << perm[0][j];
    }

    x[i] = (int) k;
  }

  bmm_hsort_cls(nmembof(x), compar, swap, x);

  for (size_t i = 1; i < nmembof(x); ++i)
    cheat_assert(x[i - 1] <= x[i]);
)

CHEAT_DECLARE(
  static size_t const ndim = 2;
  static size_t const nper[] = {6, 5};
  static bool const per[] = {true, false};
)

CHEAT_TEST(geom2d_shell_inside,
  double const x[] = {0.5, 0.5};
  double const r = 1.0 / sqrt(3.0);
  double const xper[] = {1.0, 1.0};
  bool const per[] = {false, false};

  cheat_assert_double(bmm_geom2d_shellvol(x, r, xper, per),
      (M_2PI / 3.0) * r, 1e-6);
)

CHEAT_TEST(geom2d_shell_outside,
  double const x[] = {-0.5, -0.5};
  double const r = 1.0;
  double const xper[] = {1.0, 1.0};
  bool const per[] = {false, false};

  cheat_assert_double(bmm_geom2d_shellvol(x, r, xper, per),
      (M_PI_2 / 3.0) * r, 1e-6);
)

CHEAT_TEST(size_hc_ord,
  size_t ij[2];

  type(bmm_hc, size_t)(ij, 0, ndim, nper[1]);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 0);

  type(bmm_hc, size_t)(ij, 1, ndim, nper[1]);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 1);

  type(bmm_hc, size_t)(ij, 2, ndim, nper[1]);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 2);
)

CHEAT_TEST(size_hc_iso,
  size_t ij[2];

  for (size_t i = 0; i < type(bmm_pow, size_t)(nper[1], ndim); ++i) {
    type(bmm_hc, size_t)(ij, i, ndim, nper[1]);
    size_t const j = type(bmm_unhc, size_t)(ij, ndim, nper[1]);

    cheat_assert_size(j, i);
  }
)

CHEAT_TEST(size_hcd_ord,
  size_t ij[2];

  type(bmm_hcd, size_t)(ij, 0, ndim, nper);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 0);

  type(bmm_hcd, size_t)(ij, 1, ndim, nper);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 1);

  type(bmm_hcd, size_t)(ij, 2, ndim, nper);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 2);
)

CHEAT_TEST(size_hcd_iso,
  size_t ij[2];

  for (size_t i = 0; i < bmm_size_prod(nper, ndim); ++i) {
    type(bmm_hcd, size_t)(ij, i, ndim, nper);
    size_t const j = type(bmm_unhcd, size_t)(ij, ndim, nper);

    cheat_assert_size(j, i);
  }
)

/*
CHEAT_TEST(neigh_np,
  cheat_assert_size(bmm_neigh_np(ndim), 9);
)

CHEAT_TEST(neigh_n,
  size_t buf[2];

  cheat_assert_size(bmm_neigh_n(buf, (size_t const[]) {0, 0}, ndim, nper), 4);
  cheat_assert_size(bmm_neigh_n(buf, (size_t const[]) {0, 1}, ndim, nper), 6);
  cheat_assert_size(bmm_neigh_n(buf, (size_t const[]) {1, 0}, ndim, nper), 6);
  cheat_assert_size(bmm_neigh_n(buf, (size_t const[]) {1, 1}, ndim, nper), 9);
)
*/

CHEAT_TEST(neigh_ncp,
  int const mask = BMM_NEIGH_MASK_FULL;

  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {1, 0},
        ndim, nper, per, mask), 6);
  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {1, 1},
        ndim, nper, per, mask), 9);
  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {1, 2},
        ndim, nper, per, mask), 9);
  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {2, 0},
        ndim, nper, per, mask), 6);
  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {2, 1},
        ndim, nper, per, mask), 9);
  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {2, 2},
        ndim, nper, per, mask), 9);
  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {3, 0},
        ndim, nper, per, mask), 6);
  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {3, 1},
        ndim, nper, per, mask), 9);
  cheat_assert_size(bmm_neigh_ncpij((size_t const[]) {3, 2},
        ndim, nper, per, mask), 9);
)

/*
CHEAT_TEST(neigh_ijp,
  size_t ij[2];

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 0, ndim, nper);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 3);

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 1, ndim, nper);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 2, ndim, nper);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 2);

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 3, ndim, nper);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 3);

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 4, ndim, nper);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 5, ndim, nper);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 2);

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 6, ndim, nper);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 3);

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 7, ndim, nper);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ijp(ij, (size_t const[]) {1, 1}, 8, ndim, nper);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 2);
)

CHEAT_TEST(neigh_ij,
  size_t ij[2];

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 0, ndim, nper);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 0);

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 1, ndim, nper);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 2, ndim, nper);
  cheat_assert_size(ij[0], 0);
  cheat_assert_size(ij[1], 2);

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 3, ndim, nper);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 0);

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 4, ndim, nper);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 5, ndim, nper);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 2);

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 6, ndim, nper);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 0);

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 7, ndim, nper);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ij(ij, (size_t const[]) {1, 1}, 8, ndim, nper);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 2);
)
*/

CHEAT_TEST(neigh_ijcp,
  int const mask = BMM_NEIGH_MASK_FULL;

  size_t ij[2];

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 0, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 0);

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 1, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 2, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 2);

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 3, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 0);

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 4, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 5, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 2);

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 6, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 0);

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 7, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ijcpij(ij, (size_t const[]) {1, 1}, 8, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 2);
  cheat_assert_size(ij[1], 2);
)

/*
CHEAT_TEST(neigh_ijpuh,
  size_t ij[2];

  bmm_neigh_ijpuh(ij, (size_t const[]) {4, 4}, 0, ndim, nper);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 1);

  bmm_neigh_ijpuh(ij, (size_t const[]) {4, 4}, 1, ndim, nper);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 2);

  bmm_neigh_ijpuh(ij, (size_t const[]) {4, 4}, 2, ndim, nper);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 3);
)

CHEAT_TEST(neigh_ijuh,
  size_t ij[2];

  bmm_neigh_ijuh(ij, (size_t const[]) {4, 4}, 0, ndim, nper);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 4);

  bmm_neigh_ijuh(ij, (size_t const[]) {4, 4}, 1, ndim, nper);
  cheat_assert_size(ij[0], 5);
  cheat_assert_size(ij[1], 3);

  bmm_neigh_ijuh(ij, (size_t const[]) {4, 4}, 2, ndim, nper);
  cheat_assert_size(ij[0], 5);
  cheat_assert_size(ij[1], 4);
)
*/

CHEAT_TEST(neigh_ijcpuh,
  int const mask = BMM_NEIGH_MASK_UPPERH;

  size_t ij[2];

  bmm_neigh_ijcpij(ij, (size_t const[]) {4, 4}, 0, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 4);
  cheat_assert_size(ij[1], 4);

  bmm_neigh_ijcpij(ij, (size_t const[]) {4, 4}, 1, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 3);

  bmm_neigh_ijcpij(ij, (size_t const[]) {4, 4}, 2, ndim, nper, per, mask);
  cheat_assert_size(ij[0], 1);
  cheat_assert_size(ij[1], 4);
)

CHEAT_DECLARE(
  static enum bmm_msg_prio const msg_prio[] = {
    BMM_MSG_PRIO_LOW,
    BMM_MSG_PRIO_HIGH
  };

  static enum bmm_endy const msg_endy[] = {
    BMM_ENDY_LITTLE,
    BMM_ENDY_BIG
  };

  struct msg {
    size_t i;
    unsigned char buf[BMM_MSG_HEADSIZE];
  };

  static enum bmm_io_read msg_read(void *const pbuf, size_t const n,
      void *const ptr) {
    unsigned char *const buf = pbuf;
    struct msg *const msg = ptr;

    for (size_t i = 0; i < n; ++i) {
      buf[i] = msg->buf[msg->i];
      ++msg->i;
    }

    return BMM_IO_READ_SUCCESS;
  }

  static bool msg_write(void const *pbuf, size_t const n, void *const ptr) {
    unsigned char const *const buf = pbuf;
    struct msg *const msg = ptr;

    for (size_t i = 0; i < n; ++i) {
      msg->buf[msg->i] = buf[i];
      ++msg->i;
    }

    return true;
  }
)

CHEAT_TEST(msg_spec_sp_iso,
  for (size_t iprio = 0; iprio < nmembof(msg_prio); ++iprio)
    for (size_t iendy = 0; iendy < nmembof(msg_endy); ++iendy)
      for (size_t size = 0; size < 1000; ++size) {
        struct bmm_msg_spec out;

        out.prio = msg_prio[iprio];
        out.endy = msg_endy[iendy];
        out.tag = BMM_MSG_TAG_SP;
        out.msg.size = size;

        struct msg msg;

        msg.i = 0;
        cheat_assert(bmm_msg_spec_write(&out, msg_write, &msg));

        struct bmm_msg_spec in;

        msg.i = 0;
        cheat_assert(bmm_msg_spec_read(&in, msg_read, &msg) ==
            BMM_IO_READ_SUCCESS);

        cheat_assert_int(out.prio, in.prio);
        cheat_assert_int(out.endy, in.endy);
        cheat_assert_int(out.tag, in.tag);
        cheat_assert_size(out.msg.size, in.msg.size);
      }
)

CHEAT_TEST(msg_spec_lt_iso,
  for (size_t iprio = 0; iprio < nmembof(msg_prio); ++iprio)
    for (size_t iendy = 0; iendy < nmembof(msg_endy); ++iendy)
      for (size_t e = 0; e < 4; ++e) {
        struct bmm_msg_spec out;

        out.prio = msg_prio[iprio];
        out.endy = msg_endy[iendy];
        out.tag = BMM_MSG_TAG_LT;
        out.msg.term.e = e;

        for (size_t i = 0; i < type(bmm_pow, size_t)(2, e); ++i)
          out.msg.term.buf[i] = i << i * 8 & 0xff;

        struct msg msg;

        msg.i = 0;
        cheat_assert(bmm_msg_spec_write(&out, msg_write, &msg));

        struct bmm_msg_spec in;

        msg.i = 0;
        cheat_assert(bmm_msg_spec_read(&in, msg_read, &msg) ==
            BMM_IO_READ_SUCCESS);

        cheat_assert_int(out.prio, in.prio);
        cheat_assert_int(out.endy, in.endy);
        cheat_assert_int(out.tag, in.tag);
        cheat_assert_size(out.msg.term.e, in.msg.term.e);

        for (size_t i = 0; i < out.msg.term.e; ++i)
          cheat_assert_unsigned_char(out.msg.term.buf[i], in.msg.term.buf[i]);
      }
)
