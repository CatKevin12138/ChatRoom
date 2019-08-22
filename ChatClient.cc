#include <iostream>
#include "ChatClient.hpp"
using namespace std;

static void Usage(string proc)
{
	cout << "Usage：" << proc << " peer_ip" << endl;
}

static void Menu(int& n)
{
	cout << "##################################################" << endl;
	cout << "###   1. Register                    2.Login   ###" << endl;
	cout << "###   3. Exit                                  ###" << endl;
	cout << "##################################################" << endl;
	cout << "Please select:> ";
	cin >> n;
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		Usage(argv[0]);
		exit(1);
	}
	ChatClient *cp = new ChatClient(argv[1]); 
	cp->InitClient();
	int number = 0;
	while(1)
	{
		Menu(number);
		switch(number)
		{
			case 1: //Register
				cp->Register();
				break;
			case 2: //Login
				if(cp->Login())
				{
					cp->Chat();
					cp->Logout();
				}
				break;
			case 3:
				exit(0);
				break;
			default:
				exit(1);
				break;
		}
	}
	

//	if(cp->ConnectServer())
//	{
//		cout << "connect success!" << endl;
//	}


	return 0;
}
