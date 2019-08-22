#include <iostream>
#include <string>
#include "ChatServer.hpp"
using namespace std;

static void Usage(string proc)
{
	cout << "Usage：" << proc << " tcp_port udp_port" << endl;
}

void* RunProduct(void* arg)
{
	pthread_detach(pthread_self());
	ChatServer *sp = (ChatServer*)arg;	
	for(;;)
	{
		sp->Product();
	}
}

void* RunConsume(void* arg)
{
	pthread_detach(pthread_self());
	ChatServer *sp = (ChatServer*)arg;
	for(;;)
	{
		sp->Consume();
	}
}

int main(int argc, char *argv[])
{
	if(argc != 3)
	{
		Usage(argv[0]);
		exit(1);
	}

	int tcp_port = atoi(argv[1]);
	int udp_port = atoi(argv[2]);

	ChatServer *sp = new ChatServer(tcp_port, udp_port);
	sp->InitServer();

	pthread_t c,p;
	pthread_create(&p, NULL, RunProduct, (void *)sp);
	pthread_create(&c, NULL, RunConsume, (void *)sp);

	sp->Start();

	return 0;
}

