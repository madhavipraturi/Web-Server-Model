# _*_ MakeFiel _*_
#target: dependencies
#	action

TCP_server: WebServer.c 
	gcc WebServer.c -o WebServer 
	./WebServer 8080 TCP

TCP_client: Client.c
	gcc Client.c -o Client
	./Client www.cs585.com TCP

UDP_server: WebServer.c
	gcc WebServer.c -o WebServer 
	./WebServer 8080 UDP

UDP_client: Client.c
	gcc Client.c -o Client
	./Client www.cs585.com UDP

clean:
	rm -rf Client WebServer