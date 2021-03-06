#include "k_syscall.h"
#include "logging.h"
#include <kernel/panic.h>
#include <sys/syscall.h>

// `+ 1` because it is a 1-based index.
static syscall_handler_t syscall_handlers[NB_SYSCALLS + 1] = { 0 };

void syscall_register_handler(uint8_t id, syscall_handler_t handler);
void syscall_open(registers_t* registers);
void syscall_close(registers_t* registers);
void syscall_read(registers_t* registers);
void syscall_lseek(registers_t* registers);
void syscall_fstat(registers_t* registers);
void syscall_gettimeofday(registers_t* registers);
void syscall_reboot(registers_t* registers);
void syscall_sendto(registers_t* registers);
void syscall_socket(registers_t* registers);
void syscall_write(registers_t* registers);
void syscall_test(registers_t* registers);
void syscall_recvfrom(registers_t* registers);
void syscall_gethostbyname2(registers_t* registers);

void syscall_init()
{
  syscall_register_handler(SYSCALL_TEST, syscall_test);
  syscall_register_handler(SYSCALL_WRITE, syscall_write);
  syscall_register_handler(SYSCALL_READ, syscall_read);
  syscall_register_handler(SYSCALL_GETTIMEOFDAY, syscall_gettimeofday);
  syscall_register_handler(SYSCALL_OPEN, syscall_open);
  syscall_register_handler(SYSCALL_CLOSE, syscall_close);
  syscall_register_handler(SYSCALL_REBOOT, syscall_reboot);
  syscall_register_handler(SYSCALL_FSTAT, syscall_fstat);
  syscall_register_handler(SYSCALL_LSEEK, syscall_lseek);
  syscall_register_handler(SYSCALL_SOCKET, syscall_socket);
  syscall_register_handler(SYSCALL_SENDTO, syscall_sendto);
  syscall_register_handler(SYSCALL_RECVFROM, syscall_recvfrom);
  syscall_register_handler(SYSCALL_GETHOSTBYNAME2, syscall_gethostbyname2);
}

void syscall_register_handler(uint8_t id, syscall_handler_t handler)
{
  syscall_handlers[id] = handler;
}

void syscall_handler(registers_t* registers)
{
  syscall_handler_t handler = syscall_handlers[registers->rax];

  if (handler != 0) {
    handler(registers);
    return;
  }

  PANIC("Received unimplemented syscall: %d\n", registers->rax);
}

void syscall_open(registers_t* registers)
{
  const char* pathname = (const char*)registers->rbx;
  uint32_t flags = registers->rcx;

  registers->rdx = k_open(pathname, flags);
}

void syscall_close(registers_t* registers)
{
  int fd = (int)registers->rbx;

  registers->rdx = k_close(fd);
}

void syscall_read(registers_t* registers)
{
  int fd = (int)registers->rbx;
  void* buf = (char*)registers->rcx;
  size_t count = (size_t)registers->rsi;

  registers->rdx = k_read(fd, buf, count);
}

void syscall_lseek(registers_t* registers)
{
  int fd = (int)registers->rbx;
  off_t offset = (off_t)registers->rcx;
  int whence = (int)registers->rsi;

  registers->rdx = k_lseek(fd, offset, whence);
}

void syscall_fstat(registers_t* registers)
{
  int fd = (int)registers->rbx;
  struct stat* statbuf = (struct stat*)registers->rcx;

  registers->rdx = k_fstat(fd, statbuf);
}

void syscall_gettimeofday(registers_t* registers)
{
  struct timeval* t = (struct timeval*)registers->rbx;
  void* z = (void*)registers->rcx;

  registers->rdx = k_gettimeofday(t, z);
}

void syscall_reboot(registers_t* registers)
{
  int command = (int)registers->rbx;

  registers->rdx = k_reboot(command);
}

void syscall_sendto(registers_t* registers)
{
  int sockfd = (int)registers->rsi;
  const void* buf = (const void*)registers->rdx;
  size_t len = (size_t)registers->rcx;
  int flags = (int)registers->r10;
  const struct sockaddr* dst_addr = (const struct sockaddr*)registers->r8;
  socklen_t addrlen = (socklen_t)registers->r9;

  registers->rdx = k_sendto(sockfd, buf, len, flags, dst_addr, addrlen);
}

void syscall_socket(registers_t* registers)
{
  int domain = (int)registers->rbx;
  int type = (int)registers->rcx;
  int protocol = (int)registers->rsi;

  registers->rdx = k_socket(domain, type, protocol);
}

void syscall_write(registers_t* registers)
{
  int fd = (int)registers->rbx;
  const char* buf = (char*)registers->rcx;
  size_t count = (size_t)registers->rsi;

  registers->rdx = k_write(fd, buf, count);
}

void syscall_test(registers_t* registers)
{
  const char* s = (const char*)registers->rbx;

  k_test(s);
}

void syscall_recvfrom(registers_t* registers)
{
  int sockfd = (int)registers->rsi;
  void* buf = (void*)registers->rdx;
  size_t len = (size_t)registers->rcx;
  int flags = (int)registers->r10;
  struct sockaddr* src_addr = (struct sockaddr*)registers->r8;
  socklen_t* addrlen = (socklen_t*)registers->r9;

  registers->rdx = k_recvfrom(sockfd, buf, len, flags, src_addr, addrlen);
}

void syscall_gethostbyname2(registers_t* registers)
{
  const char* name = (const char*)registers->rsi;
  struct in_addr* in = (struct in_addr*)registers->rdx;

  registers->rdx = k_gethostbyname2(name, in);
}
