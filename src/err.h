#ifndef BF02C38A_B180_4521_82AB_EC9A64C2F691
#define BF02C38A_B180_4521_82AB_EC9A64C2F691

#include "lumlib.h"
#include <errno.h>

// [Source](https://github.com/rsms/compis/blob/4823a04f9466e4a1ab903a3a718ec95af7038fe9/src/err.c#L6)

const char *err_str(err_t e)
{
    switch ((enum err_)e) { // clang-format off
        case ErrOk:           return "(no error)";
        case ErrInvalid:      return "invalid data or argument";
        case ErrSysOp:        return "invalid syscall op or syscall op data";
        case ErrBadfd:        return "invalid file descriptor";
        case ErrBadName:      return "invalid or misformed name";
        case ErrNotFound:     return "not found";
        case ErrNameTooLong:  return "name too long";
        case ErrCanceled:     return "operation canceled";
        case ErrNotSupported: return "not supported";
        case ErrExists:       return "already exists";
        case ErrEnd:          return "end of resource";
        case ErrAccess:       return "permission denied";
        case ErrNoMem:        return "cannot allocate memory";
        case ErrMFault:       return "bad memory address";
        case ErrOverflow:     return "value too large";
        case ErrReadOnly:     return "read-only";
        case ErrIO:           return "I/O error";
        case ErrNotDir:       return "not a directory";
        case ErrIsDir:        return "is a directory";
    } // clang-format on
    return "(unknown error)";
}

err_t err_errnox(int e)
{
    switch (e) { // clang-format off
    case 0: return 0;
    case EACCES:  return ErrAccess;
    case EEXIST:  return ErrExists;
    case ENOENT:  return ErrNotFound;
    case EBADF:   return ErrBadfd;
    case EROFS:   return ErrReadOnly;
    case EIO:     return ErrIO;
    case ENOTDIR: return ErrNotDir;
    case EISDIR:  return ErrIsDir;
    case ENOTSUP:
    case ENOSYS:
      return ErrNotSupported;
    case EINVAL:
    default:
      return ErrInvalid;
  } // clang-format on
}

err_t err_errno()
{
    return err_errnox(errno);
}

#endif /* BF02C38A_B180_4521_82AB_EC9A64C2F691 */
