#include "stdafx.h"
#include "Server.h"
#include "MainServer.h"

int main(int argc, char *argv[])
{
	MainServer* mainserver = new MainServer();
	mainserver->Initialize();
	mainserver->Run();
	mainserver->Release();
}