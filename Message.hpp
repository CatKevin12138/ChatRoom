#pragma once

#include <iostream>
#include <string>
#include "json/json.h"
#include "ProtocolUtil.hpp"
using namespace std;

#define NORMAL_TYPE 0
#define LOGIN_TYPE 1

class Message
{
private:
	string nickname;
	string school;
	string text;
	unsigned int id;
	unsigned int type;
public:
	Message()
	{}
	Message(const string& _n, const string& _s, const string& _t, const unsigned int& _id, unsigned int _type = NORMAL_TYPE)
		:nickname(_n)
		,school(_s)
		,text(_t)
		,id(_id)
		,type(_type)
	{}
	void ToSendString(string& send)
	{
		Json::Value root;
		root["name"] = nickname;
		root["school"] = school;
		root["text"] = text;
		root["id"] = id;
		root["type"] = type;
		Util::Serializer(root, send);
	}
	void ToRecvValue(string& recv)
	{
		Json::Value root;
		Util::Deserializer(recv, root);
		nickname = root["name"].asString();
		school = root["school"].asString();
		text = root["text"].asString();
		id = root["id"].asInt();
		type = root["type"].asInt();
	}
	const string& NickName()
	{
		return nickname;
	}
	const string& School()
	{
		return school;
	}
	const string& Text()
	{
		return text;
	}
	const unsigned int& ID()
	{
		return id;
	}
	const unsigned int& Type()
	{
		return type;
	}
	~Message()
	{}
};











