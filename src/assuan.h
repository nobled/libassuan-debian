/* assuan.c - Definitions for the Assuan protocol
 *	Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
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
 * License along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA 
 */

#ifndef ASSUAN_H
#define ASSUAN_H

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef __cplusplus
extern "C"
{
#if 0
}
#endif
#endif

typedef enum
{
  ASSUAN_No_Error = 0,
  ASSUAN_General_Error = 1,
  ASSUAN_Out_Of_Core = 2,
  ASSUAN_Invalid_Value = 3,
  ASSUAN_Timeout = 4,
  ASSUAN_Read_Error = 5,
  ASSUAN_Write_Error = 6,
  ASSUAN_Problem_Starting_Server = 7,
  ASSUAN_Not_A_Server = 8,
  ASSUAN_Not_A_Client = 9,
  ASSUAN_Nested_Commands = 10,
  ASSUAN_Invalid_Response = 11,
  ASSUAN_No_Data_Callback = 12,
  ASSUAN_No_Inquire_Callback = 13,
  ASSUAN_Connect_Failed = 14,
  ASSUAN_Accept_Failed = 15,

  /* Error codes above 99 are meant as status codes */
  ASSUAN_Not_Implemented = 100,
  ASSUAN_Server_Fault    = 101,
  ASSUAN_Invalid_Command = 102,
  ASSUAN_Unknown_Command = 103,
  ASSUAN_Syntax_Error    = 104,
  ASSUAN_Parameter_Error = 105,
  ASSUAN_Parameter_Conflict = 106,
  ASSUAN_Line_Too_Long = 107,
  ASSUAN_Line_Not_Terminated = 108,
  ASSUAN_No_Input = 109,
  ASSUAN_No_Output = 110,
  ASSUAN_Canceled = 111,
  ASSUAN_Unsupported_Algorithm = 112,
  ASSUAN_Server_Resource_Problem = 113,
  ASSUAN_Server_IO_Error = 114,
  ASSUAN_Server_Bug = 115,
  ASSUAN_No_Data_Available = 116,
  ASSUAN_Invalid_Data = 117,
  ASSUAN_Unexpected_Command = 118,
  ASSUAN_Too_Much_Data = 119,
  ASSUAN_Inquire_Unknown = 120,
  ASSUAN_Inquire_Error = 121,
  ASSUAN_Invalid_Option = 122,
  ASSUAN_Invalid_Index = 123,
  ASSUAN_Unexpected_Status = 124,
  ASSUAN_Unexpected_Data = 125,
  ASSUAN_Invalid_Status = 126,
  ASSUAN_Locale_Problem = 127,
  ASSUAN_Not_Confirmed = 128,

  /* Warning: Don't use the rror codes, below they are deprecated. */
  ASSUAN_Bad_Certificate = 201,
  ASSUAN_Bad_Certificate_Chain = 202,
  ASSUAN_Missing_Certificate = 203,
  ASSUAN_Bad_Signature = 204,
  ASSUAN_No_Agent = 205,
  ASSUAN_Agent_Error = 206,
  ASSUAN_No_Public_Key = 207,
  ASSUAN_No_Secret_Key = 208,
  ASSUAN_Invalid_Name = 209,

  ASSUAN_Cert_Revoked = 301,
  ASSUAN_No_CRL_For_Cert = 302,
  ASSUAN_CRL_Too_Old = 303,
  ASSUAN_Not_Trusted = 304,

  ASSUAN_Card_Error = 401,
  ASSUAN_Invalid_Card = 402,
  ASSUAN_No_PKCS15_App = 403,
  ASSUAN_Card_Not_Present = 404,
  ASSUAN_Invalid_Id = 405,

  /* Error codes in the range 1000 to 9999 may be used by applications
     at their own discretion. */
  ASSUAN_USER_ERROR_FIRST = 1000,
  ASSUAN_USER_ERROR_LAST = 9999

} assuan_error_t;

typedef assuan_error_t AssuanError; /* Deprecated. */

/* This is a list of pre-registered ASSUAN commands */
/* NOTE, these command IDs are now deprectated and solely exists for
   compatibility reasons. */
typedef enum
{
  ASSUAN_CMD_NOP = 0,
  ASSUAN_CMD_CANCEL,    /* cancel the current request */
  ASSUAN_CMD_BYE,
  ASSUAN_CMD_AUTH,
  ASSUAN_CMD_RESET,
  ASSUAN_CMD_OPTION,
  ASSUAN_CMD_DATA,
  ASSUAN_CMD_END,
  ASSUAN_CMD_INPUT,
  ASSUAN_CMD_OUTPUT,

  ASSUAN_CMD_USER = 256  /* Other commands should be used with this offset*/
} AssuanCommand;


#define ASSUAN_LINELENGTH 1002 /* 1000 + [CR,]LF */

struct assuan_context_s;
typedef struct assuan_context_s *assuan_context_t;
typedef struct assuan_context_s *ASSUAN_CONTEXT;

/*-- assuan-handler.c --*/
int assuan_register_command (assuan_context_t ctx,
                             const char *cmd_string,
                             int (*handler)(assuan_context_t, char *));
int assuan_register_bye_notify (assuan_context_t ctx,
                                void (*fnc)(assuan_context_t));
int assuan_register_reset_notify (assuan_context_t ctx,
                                  void (*fnc)(assuan_context_t));
int assuan_register_cancel_notify (assuan_context_t ctx,
                                   void (*fnc)(assuan_context_t));
int assuan_register_input_notify (assuan_context_t ctx,
                                  void (*fnc)(assuan_context_t, const char *));
int assuan_register_output_notify (assuan_context_t ctx,
                                  void (*fnc)(assuan_context_t, const char *));

int assuan_register_option_handler (assuan_context_t ctx,
                                    int (*fnc)(assuan_context_t,
                                               const char*, const char*));

int assuan_process (assuan_context_t ctx);
int assuan_process_next (assuan_context_t ctx);
int assuan_get_active_fds (assuan_context_t ctx, int what,
                           int *fdarray, int fdarraysize);


FILE *assuan_get_data_fp (assuan_context_t ctx);
assuan_error_t assuan_set_okay_line (assuan_context_t ctx, const char *line);
assuan_error_t assuan_write_status (assuan_context_t ctx,
                                    const char *keyword, const char *text);

/* Negotiate a file descriptor.  If LINE contains "FD=N", returns N
   assuming a local file descriptor.  If LINE contains "FD" reads a
   file descriptor via CTX and stores it in *RDF (the CTX must be
   capable of passing file descriptors).  */
assuan_error_t assuan_command_parse_fd (assuan_context_t ctx, char *line,
				     int *rfd);

/*-- assuan-listen.c --*/
assuan_error_t assuan_set_hello_line (assuan_context_t ctx, const char *line);
assuan_error_t assuan_accept (assuan_context_t ctx);
int assuan_get_input_fd (assuan_context_t ctx);
int assuan_get_output_fd (assuan_context_t ctx);
assuan_error_t assuan_close_input_fd (assuan_context_t ctx);
assuan_error_t assuan_close_output_fd (assuan_context_t ctx);


/*-- assuan-pipe-server.c --*/
int assuan_init_pipe_server (assuan_context_t *r_ctx, int filedes[2]);
void assuan_deinit_server (assuan_context_t ctx);

/*-- assuan-socket-server.c --*/
int assuan_init_socket_server (assuan_context_t *r_ctx, int listen_fd);
int assuan_init_connected_socket_server (assuan_context_t *r_ctx, int fd);


/*-- assuan-pipe-connect.c --*/
assuan_error_t assuan_pipe_connect (assuan_context_t *ctx, const char *name,
                                 char *const argv[], int *fd_child_list);
assuan_error_t assuan_pipe_connect2 (assuan_context_t *ctx, const char *name,
                                     char *const argv[], int *fd_child_list,
                                     void (*atfork) (void*, int),
                                     void *atforkvalue);
/*-- assuan-socket-connect.c --*/
assuan_error_t assuan_socket_connect (assuan_context_t *ctx, const char *name,
                                      pid_t server_pid);

/*-- assuan-domain-connect.c --*/

/* Connect to a Unix domain socket server.  RENDEZVOUSFD is
   bidirectional file descriptor (normally returned via socketpair)
   which the client can use to rendezvous with the server.  SERVER s
   the server's pid.  */
assuan_error_t assuan_domain_connect (assuan_context_t *r_ctx,
				   int rendezvousfd,
				   pid_t server);

/*-- assuan-domain-server.c --*/

/* RENDEZVOUSFD is a bidirectional file descriptor (normally returned
   via socketpair) that the domain server can use to rendezvous with
   the client.  CLIENT is the client's pid.  */
assuan_error_t assuan_init_domain_server (assuan_context_t *r_ctx,
				       int rendezvousfd,
				       pid_t client);


/*-- assuan-connect.c --*/
void assuan_disconnect (assuan_context_t ctx);
pid_t assuan_get_pid (assuan_context_t ctx);

/*-- assuan-client.c --*/
assuan_error_t 
assuan_transact (assuan_context_t ctx,
                 const char *command,
                 assuan_error_t (*data_cb)(void *, const void *, size_t),
                 void *data_cb_arg,
                 assuan_error_t (*inquire_cb)(void*, const char *),
                 void *inquire_cb_arg,
                 assuan_error_t (*status_cb)(void*, const char *),
                 void *status_cb_arg);


/*-- assuan-inquire.c --*/
assuan_error_t assuan_inquire (assuan_context_t ctx, const char *keyword,
                            unsigned char **r_buffer, size_t *r_length,
                            size_t maxlen);

/*-- assuan-buffer.c --*/
assuan_error_t assuan_read_line (assuan_context_t ctx,
                              char **line, size_t *linelen);
int assuan_pending_line (assuan_context_t ctx);
assuan_error_t assuan_write_line (assuan_context_t ctx, const char *line );
assuan_error_t assuan_send_data (assuan_context_t ctx,
                              const void *buffer, size_t length);

/* The file descriptor must be pending before assuan_receivefd is
   call.  This means that assuan_sendfd should be called *before* the
   trigger is sent (normally via assuan_send_data ("I sent you a
   descriptor")).  */
assuan_error_t assuan_sendfd (assuan_context_t ctx, int fd);
assuan_error_t assuan_receivefd (assuan_context_t ctx, int *fd);

/*-- assuan-util.c --*/
void assuan_set_malloc_hooks ( void *(*new_alloc_func)(size_t n),
                               void *(*new_realloc_func)(void *p, size_t n),
                               void (*new_free_func)(void*) );
void assuan_set_log_stream (assuan_context_t ctx, FILE *fp);
int assuan_set_error (assuan_context_t ctx, int err, const char *text);
void assuan_set_pointer (assuan_context_t ctx, void *pointer);
void *assuan_get_pointer (assuan_context_t ctx);

void assuan_begin_confidential (assuan_context_t ctx);
void assuan_end_confidential (assuan_context_t ctx);

/*-- assuan-errors.c (built) --*/
const char *assuan_strerror (assuan_error_t err);

/*-- assuan-logging.c --*/

/* Set the stream to which assuan should log message not associated
   with a context.  By default, this is stderr.  The default value
   will be changed when the first log stream is associated with a
   context.  Note, that this function is not thread-safe and should
   in general be used right at startup. */
extern void assuan_set_assuan_log_stream (FILE *fp);

/* Return the stream which is currently being using for global logging.  */
extern FILE *assuan_get_assuan_log_stream (void);

/* Set the prefix to be used at the start of a line emitted by assuan
   on the log stream.  The default is the empty string.  Note, that
   this function is not thread-safe and should in general be used
   right at startup. */
void assuan_set_assuan_log_prefix (const char *text);

/* Return a prefix to be used at the start of a line emitted by assuan
   on the log stream.  The default implementation returns the empty
   string, i.e. ""  */
const char *assuan_get_assuan_log_prefix (void);

#ifdef __cplusplus
}
#endif
#endif /* ASSUAN_H */
