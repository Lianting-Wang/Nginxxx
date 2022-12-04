nginxxx: ng_main.c ng_readfile.c ng_web_socket.c ng_http.c ng_method.c ng_file_util.c
	gcc -o nginxxx ng_main.c ng_readfile.c ng_web_socket.c ng_http.c ng_method.c ng_file_util.c -I.

clean:
	rm nginxxx
