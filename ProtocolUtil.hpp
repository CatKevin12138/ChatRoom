#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "json/json.h"
#include "Log.hpp"

#define BACKLOG 5
#define MESSAGE_SIZE 1024 

class Request
{
public:
	string method; //REGISTER, LOGIN, LOGOUT
	string content_length; //"Content_Length：89"
	string blank;
	string text;
public:
	Request()
		:blank("\n")
	{}
	~Request()
	{}
};

class  Util
{
public:
	static bool RegisterEnter(string& _n, string& _s, string& _pwd)
	{
		
			cout << "Please Enter Nickname: ";
			cin >> _n;
			cout << "Please Enter School: ";
			cin >> _s;
			cout << "Please Enter Passwd: ";
			cin >> _pwd;
			string again;
			cout << "Please Enter Passwd Again: ";
			cin >> again;
			if(again == _pwd)
			{
				return true;
			}
			return false;
	}
	static bool LoginEnter(unsigned int& _id, string& _pwd)
	{
		cout << "Please Enter ID: ";
		cin >> _id;
		cout << "Please Enter Passwd: ";
		cin >> _pwd;
		return true;
	}
	static void Serializer(Json::Value& root, string& out)
	{
		Json::FastWriter w;
		out = w.write(root);
	}
	static void Deserializer(string& in, Json::Value& root)
	{
		Json::Reader r;
		r.parse(in, root, false);
	}
	static string IntToString(int x)
	{
		stringstream ss;
		ss << x;
		return ss.str();
	}
	static int StringToInt(string& str)
	{
		int x;
		stringstream ss(str);
		ss >> x;
		return x;
	}
	static void RecvOneLine(int sock, string& out)
	{
		char c = 'x';
		while(c != '\n')
		{
			ssize_t s = recv(sock, &c, 1, 0);
			if(s > 0)
			{
				if(c == '\n')
					break;
				out.push_back(c);
			}
			else
			{
				break;
			}
		}
	}
	// TCP
	static void RecvRequest(int sock, Request& rq)
	{
		RecvOneLine(sock, rq.method);
		RecvOneLine(sock, rq.content_length);
		RecvOneLine(sock, rq.blank);

		string& cl = rq.content_length;
		size_t pos = cl.find(": ");
		if(string::npos == pos)
		{
			return;
		}
		string sub = cl.substr(pos+2);
		int size = StringToInt(sub);
		char c;
		for(auto i = 0; i < size; ++i)
		{
			recv(sock, &c, 1, 0);
			(rq.text).push_back(c);
		}
	}
	static void SendRequest(int sock, Request& rq)
	{
		string& _m = rq.method;
		string& _cl = rq.content_length;
		string& _b = rq.blank;
		string& _t = rq.text;
		send(sock, _m.c_str(), _m.size(), 0);
		send(sock, _cl.c_str(), _cl.size(), 0);
		send(sock, _b.c_str(), _b.size(), 0);
		send(sock, _t.c_str(), _t.size(), 0);
	}
	//UDP
	static void RecvMessage(int sock, string& message, struct sockaddr_in& peer)
	{
		char msg[MESSAGE_SIZE];
		socklen_t len = sizeof(peer);
		ssize_t s = recvfrom(sock, msg, sizeof(msg)-1, 0, (struct sockaddr*)&peer, &len);
		if(s <= 0)
		{
			LOG("message recvfrom error", WARNING);
		}
		else
		{
			message = msg;
		}
	}
	static void SendMessage(int sock, const string& message, struct sockaddr_in &peer)
	{
		sendto(sock, message.c_str(), message.size(), 0, (struct sockaddr*)&peer, sizeof(peer));
	}

	static void AddUser(vector<string>& online, string& f)
	{
		for(auto it = online.begin(); it != online.end(); it++)
		{
			if(*it == f)
			{
				return;
			}
		}
		online.push_back(f);
	}
};

class SocketApi{
public:
	static int Socket(int type) //成员函数设为static，使用时不必创建对象来调用
	{
		int sock = socket(AF_INET, type, 0);
		if(sock < 0) //创建套接字失败
		{
			LOG("Socket error!", ERROR);
			exit(2);
		}
	}
	static void Bind(int sock, int port)
	{
		struct sockaddr_in local;
		local.sin_family = AF_INET;
		local.sin_addr.s_addr = htonl(INADDR_ANY); //表示地址为0.0.0.0，实际上表示任意地址，所有地址
		local.sin_port = htons(port); //主机字节序转成网络字节序(大端)

		if(bind(sock, (struct sockaddr*)&local, sizeof(local)) < 0)
		{
			LOG("Bind  error!",ERROR);
			exit(3);
		}
	}
	static void Listen(int sock)
	{
		if(listen(sock, BACKLOG) < 0)
		{
			LOG("Listen error", ERROR);
			exit(4);
		}
	}
	static int Accept(int listen_sock, string &out_ip, int &out_port)
	{
		struct sockaddr_in peer;
		socklen_t len = sizeof(peer);
		int sock = accept(listen_sock, (struct sockaddr*)&peer, &len);
		if(sock < 0)
		{
			LOG("Accept error", WARNING);
			return -1;
		}
		out_ip = inet_ntoa(peer.sin_addr); //转成点分十进制
		out_port = htons(peer.sin_port);
		return sock;
	}
	static bool Connect(const int &sock, string peer_ip, const int &port)
	{
		struct sockaddr_in peer;
		peer.sin_family = AF_INET;
		peer.sin_addr.s_addr = inet_addr(peer_ip.c_str());
		peer.sin_port = htons(port);

		if(connect(sock, (struct sockaddr*)&peer, sizeof(peer)) < 0)
		{
			LOG("connect error", WARNING);
			return false;
		}

		return true;
	}
};

















