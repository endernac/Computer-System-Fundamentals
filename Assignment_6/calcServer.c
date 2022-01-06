/*
 * Interactive calculator program
 *
 * This should work correctly once you have implemented
 * and tested your calc_ functions
 */

#include <stdio.h>      /* for snprintf */
#include "csapp.h"      /* for rio_ functions */
#include "calc.h"

/* buffer size for reading lines of input from user */
#define LINEBUF_SIZE 1024

// thread info struct
struct ConnInfo {
    struct Calc *calc;
	int client_fd;
};

int chat_with_client(struct Calc *calc, int infd, int outfd);

void fatal(const char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(1);
}


void *worker(void *arg) {
	struct ConnInfo *info = arg;

	/*
	 * set thread as detached, so its resources are automatically
	 * reclaimed when it finishes
	 */
	pthread_detach(pthread_self());

	/* handle client request */
	chat_with_client(info->calc, info->client_fd, info->client_fd);

	close(info->client_fd);
	free(info);

	return NULL;
}

int main(int argc, char *argv[]) {
	struct Calc *calc = calc_create();

	if (argc != 2) {
        fatal("Usage: ./arithserver <port>");
    }

    int server_fd = open_listenfd(argv[1]);
    if (server_fd < 0) {
        fatal("Couldn't open server socket\n");
    }

    while (1) {
		int client_fd = Accept(server_fd, NULL, NULL);
		if (client_fd < 0) {
			fatal("Error accepting client connection");
		}

		/* create ConnInfo object */
		struct ConnInfo *info = malloc(sizeof(struct ConnInfo));
        info->calc = calc;
		info->client_fd = client_fd;

		/* start new thread to handle client connection */
		pthread_t thr_id;
		if (pthread_create(&thr_id, NULL, worker, info) != 0) {
			fatal("pthread_create failed");
		}
	}

    // close server socket
    close(server_fd);
    
    /* chat with client using standard input and standard output */
	//chat_with_client(calc, 0, 1);

	calc_destroy(calc);

	return 0;
}

int chat_with_client(struct Calc *calc, int infd, int outfd) {
	rio_t in;
	char linebuf[LINEBUF_SIZE];

	/* wrap standard input (which is file descriptor 0) */
	rio_readinitb(&in, infd);

	/*
	 * Read lines of input, evaluate them as calculator expressions,
	 * and (if evaluation was successful) print the result of each
	 * expression.  Quit when "quit" command is received.
	 */
	int done = 0;
	while (!done) {
		ssize_t n = rio_readlineb(&in, linebuf, LINEBUF_SIZE);
		if (n <= 0) {
			/* error or end of input */
			done = 1;
		} else if (strcmp(linebuf, "quit\n") == 0 || strcmp(linebuf, "quit\r\n") == 0) {
			/* quit command */
			done = 1;
        } else if (strcmp(linebuf, "shutdown\n") == 0 || strcmp(linebuf, "shutdown\r\n") == 0) {
            /* quit command */
            done = 1;
            return 0;
		} else {
			/* process input line */
			int result;
			if (calc_eval(calc, linebuf, &result) == 0) {
				/* expression couldn't be evaluated */
				rio_writen(outfd, "Error\n", 6);
			} else {
				/* output result */
				int len = snprintf(linebuf, LINEBUF_SIZE, "%d\n", result);
				if (len < LINEBUF_SIZE) {
					rio_writen(outfd, linebuf, len);
				}
			}
		}
	}

    return 1;
}
