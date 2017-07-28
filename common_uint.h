/// Common operations for unsigned integer types.

#include <stddef.h>

#include "ext.h"

/// The call `bmm_quot(x, y)`
/// returns the quotient and remainder of `x` divided by `y`
/// in `qr` such that `qr.quot * y + qr.rem == x` and `qr.rem >= 0`.
/// If `y == 0`, the behavior is undefined.
/// Overflows are impossible both internally and externally.
#ifndef DEBUG
__attribute__ ((__const__, __pure__))
#endif
inline type(bmm_quot_t, A) type(bmm_quot, A)(A const x, A const y) {
#ifndef DEBUG
  dynamic_assert(y != 0, "Invalid argument");
#endif

  type(bmm_quot_t, A) const qr = {.quot = x / y, .rem = x % y};

  return qr;
}

/// The call `bmm_wrap(x, a, b)`
/// finds such `y` that `a <= y < b`
/// by shifting `x` by the appropriate number of multiples of `b - a`.
/// If `b <= a`, the behavior is undefined.
/// Overflows are impossible both internally and externally.
#ifndef DEBUG
__attribute__ ((__const__, __pure__))
#endif
inline A type(bmm_wrap, A)(A const x, A const a, A const b) {
#ifndef DEBUG
  dynamic_assert(b > a, "Invalid argument");
#endif

  A const c = b - a;
  A const r = x % c;
  A const s = a % c;

  return (r >= s ? r - s : c - (s - r)) + a;

  // The following implementation is easier to understand,
  // but susceptible to overflowing.
  // A const c = b - a;
  //
  // return (x - a) % c + a;

  // The following implementation is easier to understand,
  // but slower (linear instead of constant).
  // A const c = b - a;
  //
  // A y = x;
  //
  // if (y < a)
  //   do
  //     y += c;
  //   while (y < a);
  // else if (y >= b)
  //   do
  //     y -= c;
  //   while (y >= b);
  //
  // return y;
}

/// The call `bmm_uwrap(x, b)`
/// is equivalent to `bmm_wrap(x, 0, b)`.
/// If `b <= 0`, the behavior is undefined.
/// Overflows are impossible both internally and externally.
#ifndef DEBUG
__attribute__ ((__const__, __pure__))
#endif
inline A type(bmm_uwrap, A)(A const x, A const b) {
#ifndef DEBUG
  dynamic_assert(b > 0, "Invalid argument");
#endif

  return x % b;
}

/// The call `bmm_hc(pij, i, ndim, nper)`
/// sets the index vector `pij` to the index `i`
/// in a hypercube with dimension `ndim` and side length `nper`.
/// Overflows are impossible both internally and externally.
__attribute__ ((__nonnull__))
inline void type(bmm_hc, A)(A *const pij, A const i,
    size_t const ndim, A const nper) {
  type(bmm_quot_t, A) dm = {.quot = i};
  for (size_t idim = 0; idim < ndim; ++idim) {
    dm = type(bmm_quot, A)(dm.quot, nper);

    pij[ndim - 1 - idim] = dm.rem;
  }
}

/// The call `bmm_unhc(ij, ndim, nper)`
/// returns the index of the index vector `ij`
/// in a hypercube with dimension `ndim` and side length `nper`.
/// Overflows are impossible internally but possible externally.
__attribute__ ((__nonnull__, __pure__))
inline A type(bmm_unhc, A)(A const *const ij,
    size_t const ndim, A const nper) {
  A i = 0;

  for (size_t idim = 0; idim < ndim; ++idim) {
    i *= nper;
    i += ij[idim];
  }

  return i;
}

/// The call `bmm_hc(pij, i, ndim, nper)`
/// sets the index vector `pij` to the index `i`
/// in a hypercuboid with dimension `ndim` and side lengths `nper`.
/// Overflows are impossible both internally and externally.
__attribute__ ((__nonnull__))
inline void type(bmm_hcd, A)(A *restrict const pij, A const i,
    size_t const ndim, A const *restrict const nper) {
  type(bmm_quot_t, A) dm = {.quot = i};
  for (size_t idim = 0; idim < ndim; ++idim) {
    dm = type(bmm_quot, A)(dm.quot, nper[ndim - 1 - idim]);

    pij[ndim - 1 - idim] = dm.rem;
  }

  // The following implementation is suitable for loop fusion,
  // but less reliable.
  // size_t *const buf = alloca(ndim * sizeof *buf);
  //
  // type(bmm_quot_t, A) dm = {.quot = i};
  // for (size_t idim = 0; idim < ndim; ++idim) {
  //   dm = type(bmm_quot, A)(dm.quot, nper[ndim - 1 - idim]);
  //
  //   buf[ndim - 1 - idim] = dm.rem;
  // }
  //
  // for (size_t idim = 0; idim < ndim; ++idim)
  //   pij[idim] = buf[idim];

  // The following implementation is suitable for loop fusion,
  // but slower (quadratic instead of linear).
  // for (size_t idim = 0; idim < ndim; ++idim) {
  //   type(bmm_quot_t, A) dm = {.quot = i};
  //   for (size_t jdim = 0; jdim < ndim - idim; ++jdim)
  //     dm = type(bmm_quot, A)(dm.quot, nper[ndim - 1 - jdim]);
  //
  //   pij[idim] = dm.rem;
  // }
}

/// The call `bmm_unhc(ij, ndim, nper)`
/// returns the index of the index vector `ij`
/// in a hypercuboid with dimension `ndim` and side lengths `nper`.
/// Overflows are impossible internally but possible externally.
__attribute__ ((__nonnull__, __pure__))
inline A type(bmm_unhcd, A)(A const *restrict const ij,
    size_t const ndim, A const *restrict const nper) {
  A i = 0;

  for (size_t idim = 0; idim < ndim; ++idim) {
    i *= nper[idim];
    i += ij[idim];
  }

  return i;
}