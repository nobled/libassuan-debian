/* assuan-io.c - Wraps the read and write functions.
 * Copyright (C) 2002, 2004, 2006, 2007, 2008 Free Software Foundation, Inc.
 *
 * This file is part of Assuan.
 *
 * Assuan is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * Assuan is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/time.h>
#include <sys/types.h>
#ifdef HAVE_SYS_SOCKET_H
# include <sys/socket.h>
#endif
#include <unistd.h>
#include <errno.h>
#ifdef HAVE_W32_SYSTEM
# include <windows.h>
#else
# include <sys/wait.h>
#endif

#include "assuan-defs.h"


#ifndef HAVE_W32_SYSTEM
pid_t 
_assuan_waitpid (pid_t pid, int *status, int options)
{
  return waitpid (pid, status, options);
}
#endif


static ssize_t
do_io_read (assuan_fd_t fd, void *buffer, size_t size)
{
#ifdef HAVE_W32_SYSTEM
  /* Due to the peculiarities of the W32 API we can't use read for a
     network socket and thus we try to use recv first and fallback to
     read if recv detects that it is not a network socket.  */
  int n;

  n = recv (HANDLE2SOCKET(fd), buffer, size, 0);
  if (n == -1)
    {
      switch (WSAGetLastError ())
        {
        case WSAENOTSOCK:
          {
            DWORD nread = 0;
            
            n = ReadFile (fd, buffer, size, &nread, NULL);
            if (!n)
              {
                switch (GetLastError())
                  {
                  case ERROR_BROKEN_PIPE: errno = EPIPE; break;
                  default: errno = EIO; 
                  }
                n = -1;
              }
            else
              n = (int)nread;
          }
          break;
          
        case WSAEWOULDBLOCK: errno = EAGAIN; break;
        case ERROR_BROKEN_PIPE: errno = EPIPE; break;
        default: errno = EIO; break;
        }
    }
  return n;
#else /*!HAVE_W32_SYSTEM*/
  return read (fd, buffer, size);
#endif /*!HAVE_W32_SYSTEM*/
}


ssize_t
_assuan_io_read (assuan_fd_t fd, void *buffer, size_t size)
{
  ssize_t retval;
  
  if (_assuan_io_hooks.read_hook
      && _assuan_io_hooks.read_hook (NULL, fd, buffer, size, &retval) == 1)
    return retval;

  return do_io_read (fd, buffer, size);
}

ssize_t
_assuan_simple_read (assuan_context_t ctx, void *buffer, size_t size)
{
  ssize_t retval;
  
  if (_assuan_io_hooks.read_hook
      && _assuan_io_hooks.read_hook (ctx, ctx->inbound.fd, 
                                     buffer, size, &retval) == 1)
    return retval;

  return do_io_read (ctx->inbound.fd, buffer, size);
}



static ssize_t
do_io_write (assuan_fd_t fd, const void *buffer, size_t size)
{
#ifdef HAVE_W32_SYSTEM
  /* Due to the peculiarities of the W32 API we can't use write for a
     network socket and thus we try to use send first and fallback to
     write if send detects that it is not a network socket.  */
  int n;

  n = send (HANDLE2SOCKET(fd), buffer, size, 0);
  if (n == -1 && WSAGetLastError () == WSAENOTSOCK)
    {
      DWORD nwrite;

      n = WriteFile (fd, buffer, size, &nwrite, NULL);
      if (!n)
        {
          switch (GetLastError ())
            {
            case ERROR_BROKEN_PIPE: 
            case ERROR_NO_DATA: errno = EPIPE; break;
            default:            errno = EIO;   break;
            }
          n = -1;
        }
      else
        n = (int)nwrite;
    }
  return n;
#else /*!HAVE_W32_SYSTEM*/
  return write (fd, buffer, size);
#endif /*!HAVE_W32_SYSTEM*/
}

ssize_t
_assuan_io_write (assuan_fd_t fd, const void *buffer, size_t size)
{
  ssize_t retval;
  
  if (_assuan_io_hooks.write_hook
      && _assuan_io_hooks.write_hook (NULL, fd, buffer, size, &retval) == 1)
    return retval;
  return do_io_write (fd, buffer, size);
}

ssize_t
_assuan_simple_write (assuan_context_t ctx, const void *buffer, size_t size)
{
  ssize_t retval;
  
  if (_assuan_io_hooks.write_hook
      && _assuan_io_hooks.write_hook (ctx, ctx->outbound.fd, 
                                      buffer, size, &retval) == 1)
    return retval;

  return do_io_write (ctx->outbound.fd, buffer, size);
}


#ifdef HAVE_W32_SYSTEM
int
_assuan_simple_sendmsg (assuan_context_t ctx, void *msg)
#else
ssize_t
_assuan_simple_sendmsg (assuan_context_t ctx, struct msghdr *msg)
#endif
{
#ifdef HAVE_W32_SYSTEM
  return _assuan_error (ASSUAN_Not_Implemented);
#else
  int ret;
  while ( (ret = sendmsg (ctx->outbound.fd, msg, 0)) == -1 && errno == EINTR)
    ;
  return ret;
#endif
}


#ifdef HAVE_W32_SYSTEM
int
_assuan_simple_recvmsg (assuan_context_t ctx, void *msg)
#else
ssize_t
_assuan_simple_recvmsg (assuan_context_t ctx, struct msghdr *msg)
#endif
{
#ifdef HAVE_W32_SYSTEM
  return _assuan_error (ASSUAN_Not_Implemented);
#else
  int ret;
  while ( (ret = recvmsg (ctx->inbound.fd, msg, 0)) == -1 && errno == EINTR)
    ;
  return ret;
#endif
}


void
_assuan_usleep (unsigned int usec)
{
#ifdef HAVE_W32_SYSTEM
  /* FIXME.  */
  Sleep (usec / 1000);
#else
  struct timespec req;
  struct timespec rem;

  if (usec == 0)
    return;

  req.tv_sec = 0;
  req.tv_nsec = usec * 1000;
  
  while (nanosleep (&req, &rem) < 0 && errno == EINTR)
    req = rem;
#endif
}
