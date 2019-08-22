#pragma once

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <ncurses.h>
#include <pthread.h>
using namespace std;

#define MESSAGE_SIZE 1024

class Window
{
private:
	WINDOW* header;
	WINDOW* output;
	WINDOW* online;
	WINDOW* input;
	pthread_mutex_t lock;
public:
	Window()
	{
		initscr();
		curs_set(0); //光标隐藏
		pthread_mutex_init(&lock, NULL);
	}
	void SafeWrefresh(WINDOW* w)
	{
		pthread_mutex_lock(&lock);
		wrefresh(w);
		pthread_mutex_unlock(&lock);
	}
	void DrawHeader()
	{
		int h = LINES*0.2;
		int w = COLS;
		int y = 0;
		int x = 0;
		header = newwin(h, w, y, x);
		box(header, '*', '*');
		SafeWrefresh(header);
	}
	void DrawOutput()
	{
		int h = LINES*0.6;
		int w = COLS*0.75;
		int y = LINES*0.2;
		int x = 0;
		output = newwin(h, w, y, x);
		box(output, '#', '#');
		SafeWrefresh(output);
	}
	void DrawOnline()
	{
		int h = LINES*0.6;
		int w = COLS*0.25;
		int y = LINES*0.2;
		int x = COLS*0.75;
		online = newwin(h, w, y, x);
		box(online, '#', '#');
		SafeWrefresh(online);
	}
	void DrawInput()
	{
		int h = LINES*0.2;
		int w = COLS;
		int y = LINES*0.8;
		int x = 0;
		input = newwin(h, w, y, x);
		box(input, 0, 0);
		const string tips = "Please Enter: ";
		PutStrToWin(input, 2, 2, tips);
		SafeWrefresh(input);
	}
	void PutStrToWin(WINDOW* w, int y, int x, const string& message)
	{
		mvwaddstr(w, y, x, message.c_str());
		SafeWrefresh(w);
	}
	void GetMessageFromInput(string& message)
	{
		char buffer[MESSAGE_SIZE];
		memset(buffer, '\0', sizeof(buffer));
		wgetnstr(input, buffer, sizeof(buffer));
		message = buffer;
		delwin(input);
		DrawInput();
	}
	void PutMessageToOutput(const string& message)
	{
		static int line = 1;
		int y, x;
		getmaxyx(output, y, x);
		if(line > y - 2)
		{
			delwin(output);
			DrawOutput();
			line = 1;
		}
		PutStrToWin(output, line++, 2, message);
	}
	void PutUserToOnline(const vector<string>& users)
	{
		int size = users.size();
		for(int i = 0; i < size; i++)
		{
			PutStrToWin(online, i + 2, 2, users[i]);
		}
	}
	void WelcomeRun()
	{
		int i = 1;
		int dir = 0;
		int y, x;
		string welcome = " Welcome to my chat system!";
		int len  = welcome.size();
		for(; ;)
		{
			DrawHeader();
			getmaxyx(header, y, x);
			PutStrToWin(header, y/2-1, i, welcome);
			
			if(i >= x - len - 2)
			{
				dir = 1;
			}
			else if(i <= 1)
			{
				dir = 0;
			}
			
			if(dir == 0)
			{
				i++;
			}
			else
			{
				i--;
			}
			usleep(30000);
			delwin(header);
		}
	}

	~Window()
	{
		endwin();
		pthread_mutex_destroy(&lock);
	}
};











