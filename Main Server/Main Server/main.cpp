#include "stdafx.h"
#include "Server.h"
#include "MainServer.h"



int main(int argc, char *argv[])
{
	size_t size = sizeof(ActionInfo);
	size = sizeof(AMMUNITION);
	size = sizeof(ObjInfo);

	MainServer* mainserver = new MainServer();
	
	mainserver->Initialize();
	mainserver->Run();
	mainserver->Release();


	cout << endl;
}