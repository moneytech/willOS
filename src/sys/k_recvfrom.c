#include "k_syscall.h"
#include "logging.h"
#include <arpa/inet.h>
#include <core/utils.h>
#include <errno.h>
#include <net/socket.h>
#include <proc/descriptor.h>
#include <stddef.h>

ssize_t k_recvfrom(int sockfd,
                   void* buf,
                   size_t len,
                   int flags,
                   struct sockaddr* src_addr,
                   socklen_t* addrlen)
{
  UNUSED(addrlen);
  UNUSED(src_addr);
  UNUSED(flags);

  if (sockfd < 3) {
    SYS_DEBUG("invalid socket descriptor sd=%d", sockfd);
    return -ENOTSOCK;
  }

  descriptor_t* desc = get_descriptor(sockfd);

  if (desc == NULL) {
    SYS_DEBUG("socket descriptor sockfd=%d not found", sockfd);
    return -EBADF;
  }

  if (desc->inode != NULL) {
    SYS_DEBUG("sockfd=%d is not a socket descriptor", sockfd);
    return -ENOTSOCK;
  }

  if (desc->domain != AF_INET || desc->type != SOCK_DGRAM ||
      !is_protocol_supported(desc->type, desc->protocol)) {
    SYS_DEBUG("invalid sockfd=%d", sockfd);
    return -EINVAL;
  }

  return socket_unbufferize(sockfd, (uint8_t*)buf, len);
}
