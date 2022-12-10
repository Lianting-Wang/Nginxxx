#include "ng_web_socket.h"

int listening_port(int port) {
	int sock_fd;
	struct sockaddr_in server_addr;

	sock_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sock_fd == -1) return -1;

	server_addr.sin_family = AF_INET;		//IPV4
	server_addr.sin_port = htons(port);		//Port
	server_addr.sin_addr.s_addr = INADDR_ANY;
	memset(&(server_addr.sin_zero), 0, 8);

	if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1) return -2;
	if (listen(sock_fd, BACKLOG) == -1) return -3;
	printf("Socket success! sock_fd=%d listening... \n", sock_fd);
	return sock_fd;
}

int accept_client(int sock_fd) {
	int client_fd;
	struct sockaddr_in client_addr;
	socklen_t sin_size = sizeof(client_addr);

	client_fd = accept(sock_fd, (struct sockaddr *) &client_addr, &sin_size);
	if (client_fd == -1) return -1;   //Waiting for client link
	printf("\n--------------------------------\n");
	printf("client port: %d\n", ntohs(client_addr.sin_port));
	return client_fd;
}

int close_socket(int sock_fd) {
	return close(sock_fd);
}
