#ifndef C8AE9E64_B750_47C2_8590_583FEDEFEBC5
#define C8AE9E64_B750_47C2_8590_583FEDEFEBC5

// —————————————————————————————————————————————————————————————————————————————————————
//  types
//
//  [Source](https://github.com/rsms/compis/blob/4823a04f9466e4a1ab903a3a718ec95af7038fe9/src/colib.h#L9)

// clang-format off
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <sys/types.h>

typedef int8_t             i8;
typedef uint8_t            u8;
typedef int16_t            i16;
typedef uint16_t           u16;
typedef int32_t            i32;
typedef uint32_t           u32;
typedef signed long long   i64;
typedef unsigned long long u64;
typedef size_t             usize;
typedef ssize_t            isize;
typedef intptr_t           intptr;
typedef uintptr_t          uintptr;
typedef float              f32;
typedef double             f64;
// clang-format on

// —————————————————————————————————————————————————————————————————————————————————————
//  error codes
//
// [Source](https://github.com/rsms/compis/blob/4823a04f9466e4a1ab903a3a718ec95af7038fe9/src/colib.h#L695)

typedef int err_t;
enum err_ { // clang-format off
  ErrOk           =   0, // no error
  ErrInvalid      =  -1, // invalid data or argument
  ErrSysOp        =  -2, // invalid syscall op or syscall op data
  ErrBadfd        =  -3, // invalid file descriptor
  ErrBadName      =  -4, // invalid or misformed name
  ErrNotFound     =  -5, // resource not found
  ErrNameTooLong  =  -6, // name too long
  ErrCanceled     =  -7, // operation canceled
  ErrNotSupported =  -8, // not supported
  ErrExists       =  -9, // already exists
  ErrEnd          = -10, // end of resource
  ErrAccess       = -11, // permission denied
  ErrNoMem        = -12, // cannot allocate memory
  ErrMFault       = -13, // bad memory address
  ErrOverflow     = -14, // value too large
  ErrReadOnly     = -15, // read-only
  ErrIO           = -16, // I/O error
  ErrNotDir       = -17, // not a directory
  ErrIsDir        = -18, // is a directory
}; // clang-format on

err_t err_errno(); // current errno value
err_t err_errnox(int errnoval);
const char *err_str(err_t);

#endif /* C8AE9E64_B750_47C2_8590_583FEDEFEBC5 */
