/*
 * app_socket
 *
 * Copyright (C) 2015
 * Miho <miho@miho.org.ua>
 * http://miho.org.ua
 *
 *
 * This program is free software, distributed under the terms of
 * the GNU General Public License Version 2. See the LICENSE file
 * at the top of the source tree.
 */

#include "asterisk.h"

ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "asterisk/app.h"
#include "asterisk/channel.h"
#include "asterisk/cli.h"
#include "asterisk/pbx.h"

// Register a name of application in dialplan
static char *app_socket = "Socket";

// Send data to server and receive answer
static char *send_socket(const char *message) {
	struct timeval tv;
	int buf_len = 1024*1024;
    char buf[buf_len];
    int sock;
    struct sockaddr_in addr;

    tv.tv_sec = 5; // Connect timeout in sec

    // Creating socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
    	ast_log(LOG_ERROR, "Cannot create socket!\n");
    	return 0;
    }

    // Set options of connect
    // Set timeout
	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
    // Set interface and port    
    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425);
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    // Connecting to server
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)     
    {
    	ast_log(LOG_ERROR, "Cannot connect to server!\n");
        return 0;
    }

    // Send message to server. Len "+1" is need!!!
    send(sock, message, strlen(message) + 1, 0);

    // Reciive data
    recv(sock, buf, buf_len, 0);

    close(sock);

    return ast_strdupa(buf);
}

// Func used when calling Socket app in dialplan
static int socket_exec(struct ast_channel *chan, const char *data) {
    // Arguments is needed
	if (ast_strlen_zero(data)) {
		ast_log(LOG_ERROR, "Socket app need arguments!\n");
		return 0;
	}

	char *message;
	message = ast_strdupa(data);

    char *socket_data = send_socket(message); // Send data

    // Set asterisk variables id dialplan
    pbx_builtin_setvar_helper(chan, "SOCKET_DATA", S_OR(socket_data, NULL));

	return 0;
}

// Func calling from CLI "socket test text"
static char *handle_cli_socket_test(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a) {
	char *message;
	char *ret = CLI_FAILURE;

    // Register CLI command
	switch (cmd) {
        case CLI_INIT:
                e->command = "socket test";
                e->usage =
                        "Usage: socket test <text>\n";
                return NULL;
        case CLI_GENERATE:
                return NULL;
        }

	ast_module_ref(ast_module_info->self);

	message = ast_strdupa(a->argv[2]);

    char *res = send_socket(message);
    if (res) {
        ast_verb(0, "Received: %s\n", res);
    }

    ret = CLI_SUCCESS;
    ast_module_unref(ast_module_info->self);
    return ret;
}

// Register CLI applications and it func
static struct ast_cli_entry cli_socket[] = {
        AST_CLI_DEFINE(handle_cli_socket_test, "Test socket")
};

// Load module. CLI: module load app_socket.so
static int load_module(void) {
    int res;

    res = ast_register_application_xml(app_socket, socket_exec);

    ast_cli_register_multiple(cli_socket, ARRAY_LEN(cli_socket));


    return res;
}

// Unload module. CLI: module unload app_socket.so
static int unload_module(void) {
	int res;

	res = ast_unregister_application(app_socket);

	ast_cli_unregister_multiple(cli_socket, ARRAY_LEN(cli_socket));

	return res;
}

// Info about this module for asterisk
AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "Miho Socket app");