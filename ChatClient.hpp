#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include "ProtocolUtil.hpp"
#include "Message.hpp"
#include "Window.hpp"
using namespace std;

#define TCP_PORT 8080
#define UDP_PORT 8888

class ChatClient;

struct ParamPair
{
	Window* wp;
	ChatClient* cp;
};

class ChatClient
{
private:
	string peer_ip;
	int tcp_sock;
	int udp_sock;

	string passwd;

	struct sockaddr_in server;
public:
	unsigned int id;
	string nickname;
	string school;
public:
	ChatClient(string ip_)
		:peer_ip(ip_)
		,tcp_sock(-1)
		,udp_sock(-1)
		,id(0)
	{	
		server.sin_family = AF_INET;
		server.sin_port = htons(UDP_PORT);
		server.sin_addr.s_addr = inet_addr(peer_ip.c_str());
	}
	void InitClient()
	{
		udp_sock = SocketApi::Socket(SOCK_DGRAM);
	}
	bool ConnectServer()
	{
		tcp_sock = SocketApi::Socket(SOCK_STREAM);
		return SocketApi::Connect(tcp_sock, peer_ip, TCP_PORT);
	}
	bool Register()
	{
		if(Util::RegisterEnter(nickname, school, passwd) && ConnectServer())
		{
			Request rq;
			rq.method = "REGISTER\n";
			
			Json::Value root;
			root["name"] = nickname;
			root["school"] = school;
			root["pwd"] = passwd;
			
			Util::Serializer(root, rq.text);

			rq.content_length = "Content Length: ";
			rq.content_length += Util::IntToString(rq.text.size());
			rq.content_length += "\n";

			Util::SendRequest(tcp_sock, rq);
			recv(tcp_sock, &id, sizeof(id), 0);

			bool res = false;
			if(id >= 10000)
			{
				res = true;
				cout << "Register Success! Your Login ID is: " << id << endl;
			}
			else
			{
				cout << "Register Failed! Code is: " << id << endl;
			}

			close(tcp_sock);
			return res;
		}
	}
	bool Login()
	{	
		if(Util::LoginEnter(id, passwd) && ConnectServer())
		{
			Request rq;
			rq.method = "LOGIN\n";
			
			Json::Value root;
			root["id"] = id;
			root["pwd"] = passwd;
			
			Util::Serializer(root, rq.text);

			rq.content_length = "Content Length: ";
			rq.content_length += Util::IntToString(rq.text.size());
			rq.content_length += "\n";

			Util::SendRequest(tcp_sock, rq);
			unsigned int result = 0;
			recv(tcp_sock, &result, sizeof(result), 0);

			bool res = false;
			if(result >= 10000)
			{
				res = true;
				string name = "None";
				string school = "None";
				string text = "I am  online, talk with me...";
				unsigned int id  = result;
				unsigned int type = LOGIN_TYPE;
				Message m(name, school, text, id, type);
				string send;
				m.ToSendString(send);
				UdpSend(send);
				cout << "Login Success!" << endl;
			}
			else
			{
				cout << "Login Failed! Code is: " << result << endl;
			}

			close(tcp_sock);
			return res;
		}
	}
	static void* Welcome(void* arg)
	{
		pthread_detach(pthread_self());
		Window* wp = (Window*)arg;
		wp->WelcomeRun();
	}
	void UdpSend(const string& message)
	{
		Util::SendMessage(udp_sock, message, server);
	}
	void UdpRecv(string& message)
	{
		struct sockaddr_in peer;
		Util::RecvMessage(udp_sock, message, peer);
	}
	static void* Input(void* arg)
	{
		pthread_detach(pthread_self());
		struct ParamPair* ppp = (struct ParamPair*)arg;
		Window* wp = ppp->wp;
		ChatClient* cp = ppp->cp;

		wp->DrawInput();
		string text;
		for(; ;)
		{
			wp->GetMessageFromInput(text);
			Message msg(cp->nickname, cp->school, text, cp->id);
			string send;
			msg.ToSendString(send);
			cp->UdpSend(send);
		}
	}
	void Chat()
	{
		Window w;
		pthread_t h, l;

		struct ParamPair pp = {&w, this};
		
		pthread_create(&h, NULL, Welcome, &w);
		pthread_create(&l, NULL, Input, &pp);
		
		w.DrawOutput();
		w.DrawOnline();
		string recv;
		string show;
		vector<string> online;
		for(; ;)
		{
			Message msg;
			UdpRecv(recv);
			msg.ToRecvValue(recv);

			if(msg.ID() == id && msg.Type() == LOGIN_TYPE)
			{
				nickname = msg.NickName();
				school = msg.School();
			}

			show = msg.NickName();
			show += '-';
			show += msg.School();

			string f = show;
			Util::AddUser(online, f);
			
			show += "# ";
			show += msg.Text(); 
			w.PutMessageToOutput(show);
			
			w.PutUserToOnline(online);
		}

	}
//	void Logout()
//	{
//		if(Util::LoginEnter(id, passwd) && ConnectServer())
//		{
//			Request rq;
//			rq.method = "LOGIN\n";
//			
//			Json::Value root;
//			root["id"] = id;
//			root["pwd"] = passwd;
//			
//			Util::Serializer(root, rq.text);
//
//			rq.content_length = "Content Length: ";
//			rq.content_length += Util::IntToString(rq.text.size());
//			rq.content_length += "\n";
//
//			Util::SendRequest(tcp_sock, rq);
//			unsigned int result = 0;
//			recv(tcp_sock, &result, sizeof(result), 0);
//
//			bool res = false;
//			if(result >= 10000)
//			{
//				res = true;
//				string name = "None";
//				string school = "None";
//				string text = "I am  online, talk with me...";
//				unsigned int id  = result;
//				unsigned int type = LOGIN_TYPE;
//				Message m(name, school, text, id, type);
//				string send;
//				m.ToSendString(send);
//				UdpSend(send);
//				cout << "Login Success!" << endl;
//			}
//			else
//			{
//				cout << "Login Failed! Code is: " << result << endl;
//			}
//
//			close(tcp_sock);
//			return res;
//		}
//	}
	~ChatClient()
	{}
};

