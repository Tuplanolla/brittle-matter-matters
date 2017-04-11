#ifndef BMM_MSG_H
/// Messaging protocol.
#define BMM_MSG_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "cpp.h"
#include "ext.h"

/// This preprocessor directive refers to
/// the maximal number of octets in the flag part of a message header.
#define BMM_MSG_FLAGSIZE 1

/// This preprocessor directive refers to
/// the maximal number of octets in the prefix part of a message header.
#define BMM_MSG_PRESIZE 8

/// This preprocessor directive refers to
/// the maximal number of octets in a message header.
#define BMM_MSG_HEADSIZE (BMM_MSG_FLAGSIZE + BMM_MSG_PRESIZE)

/// This enumeration specifies message priority.
enum bmm_msg_prio {
  BMM_MSG_PRIO_LOW,
  BMM_MSG_PRIO_HIGH
};

/// This enumeration specifies integer endianness.
enum bmm_msg_endian {
  BMM_MSG_ENDIAN_LITTLE,
  BMM_MSG_ENDIAN_BIG
};

/// This enumeration distinguishes
/// size-prefixed messages from literal-terminated messages.
enum bmm_msg_tag {
  BMM_MSG_TAG_SP,
  BMM_MSG_TAG_LT
};

/// This structure allows the user to signal
/// what kind of message they want to send.
/// Middle-endianness or free patterns are not supported.
struct bmm_msg_spec {
  enum bmm_msg_prio prio;
  enum bmm_msg_endian endian;
  union {
    size_t size;
    struct {
      size_t e;
      uint8_t buf[BMM_MSG_PRESIZE];
    } term;
  } msg;
  enum bmm_msg_tag tag;
};

/// These preprocessor directives help work with bits in message headers.
#define BMM_MSG_MASK_PRIO (BMM_MASKBITS_1(7))
#define BMM_MSG_MASK_ENDIAN (BMM_MASKBITS_3(6, 5, 4))
#define BMM_MSG_MASK_VAR (BMM_MASKBITS_1(3))
#define BMM_MSG_MASK_TAG (BMM_MASKBITS_1(2))
#define BMM_MSG_MASK_FIXSIZE (BMM_MASKBITS_3(2, 1, 0))
#define BMM_MSG_MASK_VARSIZE (BMM_MASKBITS_2(1, 0))

/// The call `bmm_msg_spec_def(spec)`
/// writes the default message specification into `spec`.
__attribute__ ((__nonnull__))
void bmm_msg_spec_def(struct bmm_msg_spec*);

/// The call `bmm_msg_spec_read(spec, f, ptr)`
/// extracts the message specification `spec`
/// from the message header `buf` of length `n`
/// that is obtained by sequentially calling `f(&buf[i], ptr)` for all `i`.
/// It is guaranteed that `n <= 10`.
__attribute__ ((__nonnull__ (1, 2)))
bool bmm_msg_spec_read(struct bmm_msg_spec*,
    bool (*)(uint8_t*, void*), void*);

/// The call `bmm_msg_spec_write(spec, f, ptr)`
/// builds the message header `buf` of length `n`
/// for the message specification `spec` and
/// sequentially calls `f(&buf[i], ptr)` for all `i`.
/// It is guaranteed that `n <= 10`.
__attribute__ ((__nonnull__ (1, 2)))
bool bmm_msg_spec_write(struct bmm_msg_spec const*,
    bool (*)(uint8_t const*, void*), void*);

// TODO Tune this.
enum bmm_msg_id {
  BMM_MSG_NOP = 0,
  BMM_MSG_NSTEP = 60,
  BMM_MSG_NPART = 142,
  BMM_MSG_PARTS = 144,
  BMM_MSG_NEIGH = 168,
  BMM_MSG_EKINE = 185,
  BMM_MSG_ESMOM = 186,
  BMM_MSG_EVMOM = 187
};

// TODO Deprecate the rest.

#include "dem.h"

struct bmm_msg_head {
  unsigned char flags;
  unsigned char type;
};

#define BMM_FBIT_INTLE 7
#define BMM_FBIT_FPLE 5
#define BMM_FBIT_FLUSH 4
#define BMM_FBIT_BODY 3
#define BMM_FBIT_PREFIX 2

__attribute__ ((__nonnull__ (2)))
bool bmm_msg_preread(size_t* const ptr, struct bmm_msg_head const* const head);

__attribute__ ((__nonnull__))
bool bmm_msg_prewrite(struct bmm_msg_head const* const bad_head,
    size_t const size);

__attribute__ ((__nonnull__))
void bmm_head_def(struct bmm_msg_head*);

__attribute__ ((__deprecated__, __nonnull__))
bool bmm_msg_get(struct bmm_msg_head*, struct bmm_dem*);

__attribute__ ((__deprecated__, __nonnull__))
void bmm_msg_put(struct bmm_msg_head const*, struct bmm_dem const*);

#endif
