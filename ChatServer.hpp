#pragma once

#include <iostream>
#include <pthread.h>
#include "Log.hpp"
#include "ProtocolUtil.hpp"
#include "UserManager.hpp"
#include "DataPool.hpp"
#include "Message.hpp"
using namespace std;

class ChatServer;

class Param
{
public:
	ChatServer *sp;
	int sock;
	string ip;
	int port;
public:
	Param(ChatServer *_sp, const int &_sock, const string& _ip, const int& _port)
		:sp(_sp)
		,sock(_sock)
		,ip(_ip)
		,port(_port)
	{}

	~Param()
	{}
};

class ChatServer
{
private:
	int tcp_listen_sock;
	int tcp_port;

	int udp_chat_sock;
	int udp_port;

	UserManager um;
	DataPool pool;
public:
	ChatServer(int tcp_port_ = 8080, int udp_port_ = 8888)
		:tcp_listen_sock(-1)
		,tcp_port(tcp_port_)
		,udp_chat_sock(-1)
		,udp_port(udp_port_)
	{}

	void InitServer()
	{
		tcp_listen_sock = SocketApi::Socket(SOCK_STREAM);
		udp_chat_sock = SocketApi::Socket(SOCK_DGRAM);
		SocketApi::Bind(tcp_listen_sock, tcp_port);
		SocketApi::Bind(udp_chat_sock, udp_port);
		
		SocketApi::Listen(tcp_listen_sock);

	}
	unsigned int RegisterUser(const string& name, const string& school, const string& passwd)
	{
		return um.Insert(name, school, passwd);
	}
	unsigned int LoginUser(const unsigned int& id, const string& passwd, const string& ip, const int& port)
	{
		return um.Check(id, passwd);
	}
	void Product()
	{
		string message;
		struct sockaddr_in peer;
		Util::RecvMessage(udp_chat_sock, message, peer);
		cout << "debug: recv message: " << message << endl; 
		if(!message.empty())
		{
			Message m;
			m.ToRecvValue(message);
			if(m.Type() == LOGIN_TYPE)
			{
				um.AddOnlineUser(m.ID(), peer);
				string name;
				string school;
				um.GetUserInfo(m.ID(), name, school);
				Message new_msg(name, school, m.Text(), m.ID(), m.Type());
				new_msg.ToSendString(message);
			}
		
			pool.PutMessage(message);
		}
	}
	void Consume()
	{
		string message;
		pool.GetMessage(message);
		cout << "debug: send message: " << message << endl; 
		auto online = um.OnlineUsers();	
		for(auto it = online.begin(); it != online.end(); it++)
		{
			Util::SendMessage(udp_chat_sock, message, it->second);
		}
	}
	static void *HandleRequest(void *arg)
	{
		Param *p = (Param*)arg;
		int sock = p->sock;
		string ip = p->ip;
		int port = p->port;
		ChatServer *sp = p->sp;
		delete p;
		pthread_detach(pthread_self());		

		Request rq;
		Util::RecvRequest(sock, rq);
		Json::Value root;

		LOG(rq.text, NORMAL);
		
		Util::Deserializer(rq.text, root);
		if(rq.method == "REGISTER")
		{
			string name = root["name"].asString();
			string school = root["school"].asString();	
			string passwd = root["pwd"].asString();
			unsigned int id = sp->RegisterUser(name, school, passwd);
			send(sock, &id, sizeof(id), 0);
		}
		else if(rq.method == "LOGIN")
		{
			unsigned int id  = root["id"].asInt();
			string passwd = root["pwd"].asString();

			unsigned int result = sp->LoginUser(id, passwd, ip, port);		
			cout << send(sock, &result, sizeof(result), 0);
		}
		else
		{

		}

		close(sock);
	}
	void Start()
	{

		string ip;
		int port;
		for(;;)
		{
			int sock = SocketApi::Accept(tcp_listen_sock, ip, port);
			if(sock > 0)
			{
				cout<< "get a new client " << ip << "："<< port << endl;
				Param *p = new Param(this, sock, ip, port);
				pthread_t tid;
				pthread_create(&tid, NULL,  HandleRequest, (void*)p);
			}
		}
	}


	~ChatServer()
	{}

};
