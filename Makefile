nginxxx: ng_main.c ng_readfile.c ng_web_socket.c ng_http.c
	gcc -o nginxxx ng_main.c ng_readfile.c ng_web_socket.c ng_http.c -I.

clean:
	rm nginxxx.o