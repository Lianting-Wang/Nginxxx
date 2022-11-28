#include "ng_main.h"

int main() {
	int i;
	pid_t pid;
	struct niginxxx_instance* niginxxx = ng_init_instance();
	// printf("Niginxxx instance initialized success!\n");
	struct port_list* port_lists = niginxxx->port_lists;
	int sock_fd[niginxxx->lists_num];
	for (i=0; i<niginxxx->lists_num; ++i) {
		pid = fork(); //Create a child process for each virtual host
		if (!pid) {
			printf("I am pid %d for port %d\n", getpid(), port_lists->port);
			sock_fd[i] = listening_port(port_lists->port);
			break;
		}
		port_lists = port_lists->next;
	}
	if (!pid) {
		int client_fd;
    pid_t host_pid;
		while(niginxxx) {
			client_fd = accept_client(sock_fd[i]);
			signal(SIGCHLD, SIG_IGN);
			host_pid = fork(); //Create a new child process for each connection
			if (!host_pid) {
				handle_request(client_fd, niginxxx->hosts, port_lists->host_lists);
				// close_socket(client_fd);
				exit(0);
			}
		}
	} else {
		int status = 0;
		wait(&status); //wait the end of child process
		if(WIFEXITED(status)){
			printf("child process return %d\n",WEXITSTATUS(status));
		}
		printf("I am pid %d\n", getpid());
	}
	for (i=0; i<niginxxx->lists_num; ++i) {
		close_socket(sock_fd[i]);
	}
	ng_destroy_instance(niginxxx);
	printf("Niginxxx instance destroied success!\n");
	return 0;
}
