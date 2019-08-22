#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <pthread.h>
using namespace std;

class User
{
private:
	string nickname;
	string school;
	string passwd;
public:
	User()
	{}
	User(const string& _n, const string& _s, const string& _pwd)
		:nickname(_n)
		,school(_s)
		,passwd(_pwd)
	{}
	bool CheckPasswd(const string& _pwd)
	{
		return passwd == _pwd ? true : false;
	}
	const string& GetNickName()
	{
		return nickname;
	}
	const string& GetSchool()
	{
		return school;
	}
	~User()
	{}

};

class UserManager
{
private:
	unsigned int assign_id;
	unordered_map<unsigned int, User> users;
	unordered_map<unsigned int, struct sockaddr_in> online_users;
	pthread_mutex_t lock;

	void Lock()
	{
		pthread_mutex_lock(&lock);
	}
	void Unlock()
	{
		pthread_mutex_unlock(&lock);
	}
public:
	UserManager()
		:assign_id(10000)
	{
		pthread_mutex_init(&lock, NULL);
	}
	unsigned int Insert(const string& _n, const string& _s, const string& _pwd)
	{
		Lock();
		unsigned int id = assign_id++;
		User u(_n, _s, _pwd);
		if(users.find(id) == users.end())
		{
			users.insert({id, u});
			Unlock();
			return id;
		}
		Unlock();
		return 1;
	}
	unsigned int Check(const unsigned int& _id, const string& _pwd)
	{
		Lock();
		auto it = users.find(_id);
		if(it != users.end())
		{
			User& u = it->second;
			if(u.CheckPasswd(_pwd))
			{
				Unlock();
				return _id;
			}
		}
		Unlock();
		return 2;
	}
	void AddOnlineUser(const unsigned int& id, struct sockaddr_in& peer)
	{
		Lock();
		auto it = online_users.find(id);
		if(it == online_users.end())
		{
			online_users.insert({id, peer});
		}
		Unlock();
	}
	void GetUserInfo(const unsigned int& id, string& name, string& school)
	{
		Lock();
		name = users[id].GetNickName();
		school = users[id].GetSchool();
		Unlock();
		
	}
	unordered_map<unsigned int, struct sockaddr_in> OnlineUsers()
	{
		Lock();
		auto online = online_users;
		Unlock();
		return online;
	}
	~UserManager()
	{
		pthread_mutex_destroy(&lock);
	}
};
















