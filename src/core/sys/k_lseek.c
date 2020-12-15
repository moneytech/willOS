#include "logging.h"
#include "syscall.h"
#include <errno.h>
#include <fcntl.h>
#include <fs/vfs.h>
#include <proc/descriptor.h>
#include <stddef.h>
#include <sys/types.h>

off_t k_lseek(int fd, off_t offset, int whence)
{
  if (fd < 3) {
    CORE_SYS_DEBUG("invalid file descriptor fd=%d", fd);
    return -EPERM;
  }

  descriptor_t* desc = get_descriptor(fd);

  if (desc == NULL) {
    CORE_SYS_DEBUG("file descriptor fd=%d not found", fd);
    return -EBADF;
  }

  stat_t stat;
  vfs_stat(desc->inode, &stat);

  switch (whence) {
    case SEEK_SET:
      if (offset > stat.size) {
        return -EINVAL;
      }

      desc->offset = offset;
      break;
    case SEEK_CUR:
      if (desc->offset + offset > stat.size) {
        return -EINVAL;
      }

      desc->offset += offset;
      break;
    case SEEK_END:
      // TODO: implement me
    default:
      return -EINVAL;
  }

  return desc->offset;
}