//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <unistd.h> 

#include "asterisk.h"


ASTERISK_FILE_VERSION(__FILE__, "$Revision$")

#include "asterisk/module.h"
#include "asterisk/app.h"
#include "asterisk/channel.h"
#include "asterisk/cli.h"
#include "asterisk/pbx.h"

static char *app_socket = "Socket";

static char *send_socket(int type, const char *message1)
{

	struct timeval tv;
	tv.tv_sec = 5;

    char message[strlen(message1)];
    char buf[sizeof(char*)];
    int sock, len;
    struct sockaddr_in addr;
    char *msg_ptr = (char*)malloc(sizeof(char*));

    strcpy(message, message1);
    strcpy(msg_ptr, "\0");

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0)
    {
    	ast_log(LOG_ERROR, "Cannot create socket!\n");
    	return 0;
        //perror("socket");
        //exit(1);
    }

	setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO,(struct timeval *)&tv,sizeof(struct timeval));
	setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO,(struct timeval *)&tv,sizeof(struct timeval));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(3425); // или любой другой порт...
    addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
    	ast_log(LOG_ERROR, "Cannot connect to server!\n");
            //perror("connect");
        return 0;
    }


    send(sock, message, sizeof(message), 0);

    while (1)
    {
        len = recv(sock,buf,1,0);
        if(len == 0)
        {

            //ast_verb(0, "Client  disconnected\n");
            //close(sock);
            break;
           
        }
        if(len == -1)
        {
        	ast_log(LOG_WARNING, "Receive data error (maybe timeout)!\n");
            //ast_verb(0, "recv said");
            //close(sock);
        	break;
        }
        //ast_verb(0, msg_ptr);
        msg_ptr = (char*)realloc(msg_ptr, (strlen(msg_ptr) + strlen(buf)) * sizeof(char*));
        strcat(msg_ptr, buf);

        if(*buf == '\n' || *buf == '\0')
        {   break;
        }
    }
    
    //cout<<"RD: "<<msg_ptr<<endl;
    msg_ptr = (char*)realloc(msg_ptr, (strlen(msg_ptr) + strlen("\0")) * sizeof(char*));
    strcat(msg_ptr, "\0");
    close(sock);
    //free(msg_ptr);

    return msg_ptr;
}

static int socket_exec(struct ast_channel *chan, const char *data)
{
	/*unsigned int vsize;
	char *parse;
	AST_DECLARE_APP_ARGS(args,
		AST_APP_ARG(level);
		AST_APP_ARG(msg);
	);

	if (ast_strlen_zero(data)) {
		return 0;
	}

	parse = ast_strdupa(data);
	AST_STANDARD_APP_ARGS(args, parse);
	if (args.argc == 1) {
		args.msg = args.level;
		args.level = "0";
	}

	if (sscanf(args.level, "%30u", &vsize) != 1) {
		vsize = 0;
		ast_log(LOG_WARNING, "'%s' is not a verboser number\n", args.level);
	} else if (4 < vsize) {
		vsize = 4;
	}

	ast_verb(vsize, "%s\n", args.msg);*/

	//char *message;

	//message = (char*)realloc(message,(sizeof(message) + sizeof(data)));
	 //strcpy(message, data);
	//message = ast_strdupa(a->argv[2]);


	//ast_verb(0, "Pre send func\n");
	if (ast_strlen_zero(data)) {
		ast_log(LOG_ERROR, "Socket app need arguments!\n");
		return 0;
	}
	char *parse;
	parse = ast_strdupa(data);
    char *socket_data = send_socket(0, parse);
    //ast_log(LOG_NOTICE, "Received data: %s\n", socket_data);
    if (socket_data) {
    	pbx_builtin_setvar_helper(chan, "SOCKET_DATA", socket_data);
    }
	//pbx_builtin_setvar_helper(chan, "SOCKET_DATA", S_OR(send_socket(0, data), NULL));
	//ast_log(LOG_WARNING, "%s\n", socket_data);

	return 0;
}

static char *handle_cli_socket_show(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	char *ret = CLI_FAILURE;
	switch (cmd) {
        case CLI_INIT:
                e->command = "socket show";
                e->usage =
                        "Usage: socket show to show status\n";
                return NULL;
        case CLI_GENERATE:
                return NULL;
        }

	ast_module_ref(ast_module_info->self);
    ret = CLI_SUCCESS;
    return ret;
}

static char *handle_cli_socket_set(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	char *ret = CLI_FAILURE;
	switch (cmd) {
        case CLI_INIT:
                e->command = "socket set";
                e->usage =
                        "Usage: socket set variable\n";
                return NULL;
        case CLI_GENERATE:
                return NULL;
        }

	ast_module_ref(ast_module_info->self);
    ret = CLI_SUCCESS;
    return ret;
}

static char *handle_cli_socket_test(struct ast_cli_entry *e, int cmd, struct ast_cli_args *a)
{
	//ast_verb(0, "Pre test init\n");
	//char *message;
	char *parse;
	char *ret = CLI_FAILURE;
	switch (cmd) {
        case CLI_INIT:
                e->command = "socket test";
                e->usage =
                        "Usage: socket set variable\n";
                return NULL;
        case CLI_GENERATE:
                return NULL;
        }

	ast_module_ref(ast_module_info->self);
	parse = ast_strdupa(a->argv[2]);
	//ast_verb(0, "Pre send func\n");
    char *res = send_socket(1, parse);
    if (res) {
    	ast_cli(a->fd, "Received: %s.\n", res);
    }

    ret = CLI_SUCCESS;
    ast_module_unref(ast_module_info->self);
    return ret;
}


static struct ast_cli_entry cli_socket[] = {
        AST_CLI_DEFINE(handle_cli_socket_show, "Show socket status"),
        AST_CLI_DEFINE(handle_cli_socket_set, "Set socet variable"),
        AST_CLI_DEFINE(handle_cli_socket_test, "Test socket")
};

static int unload_module(void)
{
	int res;

	res = ast_unregister_application(app_socket);
	//res |= ast_unregister_application(app_log);

	ast_cli_unregister_multiple(cli_socket, ARRAY_LEN(cli_socket));

	return res;
}

static int load_module(void)
{
	int res;

	res = ast_register_application_xml(app_socket, socket_exec);
	//res |= ast_register_application_xml(app_verbose, verbose_exec);

	ast_cli_register_multiple(cli_socket, ARRAY_LEN(cli_socket));


	return res;
}

AST_MODULE_INFO_STANDARD(ASTERISK_GPL_KEY, "Miho Socket app");