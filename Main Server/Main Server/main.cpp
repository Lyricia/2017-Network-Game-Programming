#include "stdafx.h"
#include "MainServer.h"



int main(int argc, char *argv[])
{
	ActionInfo t1[2] = {};
	ObjInfo s1[2] = {};

	t1[0].Direction = { 1.1f,2.2f };
	t1[0].ObjectID = 1;

	t1[1].Direction = { 1.1f,2.2f };
	t1[1].ObjectID = 123;

	s1[0].Ammo.GunAmmo = 111;
	s1[0].Direction = { 2.2f, 3.3f };
	s1[0].HP = 123;
	s1[0].ObjectID = 3;
	s1[0].Position = { 3,3 };

	s1[1].Ammo.GunAmmo = 222;
	s1[1].Direction = { 2.2f, 3.3f };
	s1[1].HP = 113;
	s1[1].ObjectID = 6;
	s1[1].Position = { 3,3 };

	NGPMSG* msg = CreateMSG(
		MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE,
		1,
		1,
		2,
		2, 2,
		&s1, &t1);

	ActionInfo* t2 = new ActionInfo();
	ObjInfo* s2 = new ObjInfo();


	DispatchMSG(msg, *t2, *s2);





	MainServer* mainserver = new MainServer();
	
	mainserver->Initialize();
	mainserver->Run();
	mainserver->Release();


	cout << endl;
}