# Assuan Makefile
# Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
#
# This file is part of Assuan.
#
# Assuan is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation; either version 2.1 of
# the License, or (at your option) any later version.
#
# Assuan is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA 

## Process this file with automake to produce Makefile.in

EXTRA_DIST = libassuan-config.in mkerrors libassuan.m4
INCLUDES = -I.. -I$(top_srcdir)/include
BUILT_SOURCES = assuan-errors.c
MOSTLYCLEANFILES = assuan-errors.c

bin_SCRIPTS = libassuan-config
m4datadir = $(datadir)/aclocal
m4data_DATA = libassuan.m4
lib_LIBRARIES = libassuan.a
include_HEADERS = assuan.h


#libassuan_a_LDFLAGS =
libassuan_a_SOURCES = \
	assuan-defs.h \
	assuan-util.c \
	assuan-errors.c \
	assuan-buffer.c \
	assuan-handler.c \
	assuan-inquire.c \
	assuan-listen.c \
	assuan-connect.c \
	assuan-client.c \
	assuan-pipe-server.c \
	assuan-socket-server.c \
	assuan-pipe-connect.c \
	assuan-socket-connect.c \
	assuan-io.c \
	assuan-domain-connect.c \
	assuan-domain-server.c \
	assuan-logging.c

libassuan_a_LIBADD = @LIBOBJS@

assuan-errors.c : assuan.h
	$(srcdir)/mkerrors < $(srcdir)/assuan.h > assuan-errors.c