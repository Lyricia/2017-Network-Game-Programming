#pragma once

#define DETAILBUFSIZE 512

//struct D2D_POINT_2F {};

enum MSGSIZE {
	SIZE_HEADER = 8,
	SIZE_ACTIONINFO = 12,
	SIZE_OBJINFO = 24,
	SIZE_AMMUNITION = 4
};

namespace MSGTYPE {
	enum MSGSTATE {
		ROOMCREATION = 10,
		CLIENTREADY,
		CLIENTGAMEOVER,
		AICREATTIONREQUEST,
		AIAGENTINFO
	};

	enum MSGACTION {
		MOVE = 20,
		SHOOT,
		BUILDTURRET,
		RELOAD
	};

	enum MSGUPDATE {
		ADJUSTPOS = 30,
		CREATEOBJECT,
		DELETEOBJECT,
		UPDATEOBJECTSTATE
	};
}

struct ActionInfo {
	D2D_POINT_2F		Direction;
	unsigned int		ObjectID;
};

struct AMMUNITION {
	unsigned char		Granade;
	unsigned char		GunAmmo;
	unsigned char		TurretKit;
	unsigned char		packer;
};

struct ObjInfo {
	AMMUNITION			Ammo;
	D2D_POINT_2F		Position;
	D2D_POINT_2F		Direction;
	unsigned char		ObjectID;
	unsigned char		HP;
	short				packer;
};

struct MSGHEADER {
	unsigned char		MSGTYPE;
	unsigned short		DETAILINFOSIZE;
	unsigned char		ROOMNO;
	unsigned char		OBJECTNO;
	
	unsigned char		NUM_OBJINFO;
	unsigned char		NUM_ACTIONINFO;

	char				packer;
};

struct NGPMSG {
	MSGHEADER			header;
	char				detailinfo[DETAILBUFSIZE];
};

inline NGPMSG* CreateMSG(char type, short size, char roomno, char objectno, char nObjinfo, char nActioninfo, void* detail) 
{
	NGPMSG* msg = new NGPMSG();
	msg->detailinfo = 1;

	MSGHEADER msgHeader = { type, size, roomno, objectno, nObjinfo, nActioninfo };
	
	msg->header = msgHeader;
	msg->detailinfo = detail;

	return msg;
}

inline void DispatchMSG(NGPMSG* msg)
{
	switch (msg->header.MSGTYPE) 
	{
	case MSGTYPE::MSGACTION::MOVE:
	{
		int num_actioninfo = msg->header.NUM_ACTIONINFO;
		ActionInfo* actionlist = new ActionInfo();
		memcpy(actionlist, msg->detailinfo, sizeof())


		for (int i = 0; i < num_actioninfo; ++i)
		{

		}

		delete[] actionlist;
		break;
	}

	case MSGTYPE::MSGACTION::SHOOT:
		
		break;
	
	case MSGTYPE::MSGACTION::BUILDTURRET:
		
		break;

	case MSGTYPE::MSGACTION::RELOAD:
		
		break;


	case MSGTYPE::MSGSTATE::AIAGENTINFO:
		break;

	case MSGTYPE::MSGSTATE::AICREATTIONREQUEST:
		break;

	case MSGTYPE::MSGSTATE::CLIENTGAMEOVER:
		break;

	case MSGTYPE::MSGSTATE::CLIENTREADY:
		break;

	case MSGTYPE::MSGSTATE::ROOMCREATION:
		break;


	case MSGTYPE::MSGUPDATE::ADJUSTPOS:
		break;

	case MSGTYPE::MSGUPDATE::CREATEOBJECT:
		break;

	case MSGTYPE::MSGUPDATE::DELETEOBJECT:
		break;

	case MSGTYPE::MSGUPDATE::UPDATEOBJECTSTATE:
		break;

	}
}