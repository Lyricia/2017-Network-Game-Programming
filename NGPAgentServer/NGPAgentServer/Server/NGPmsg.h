#pragma once
#include "stdafx.h"


#define ACTIONINFOBUFSIZE	128
#define OBJINFOBUFSIZE		256

enum MSGSIZE {
	SIZE_HEADER = 8,
	SIZE_ACTIONINFO = 12,
	SIZE_OBJINFO = 24,
	SIZE_AMMUNITION = 4
};

namespace MSGTYPE {

	/*-----------------
	
	0 - ���ο� ���� �����.
	1 - Ŭ���̾�Ʈ�� �غ�Ǿ���.
	2 - Ŭ���̾�Ʈ�� ������ ����Ǿ���.
	3 - ������Ʈ �� ���� ��û�� �޴´�.
	4 - ������Ʈ�� ����

	-------------------*/
	enum MSGSTATE {
		ROOMCREATION = 10,
		CLIENTREADY,
		CLIENTGAMEOVER,
		AICREATTIONREQUEST,
		AIAGENTINFO
	};

	/*-----------------
	0 - ������
	1 - �߻���
	2 - �ͷ��� ����
	3 - ��������
	------------------*/

	enum MSGACTION {
		MOVE = 20,
		SHOOT,
		BUILDTURRET,
		RELOAD
	};

	/*-----------------
	0 - ��ġ ����
	1 - ������Ʈ ����
	2 - ������Ʈ ����
	3 - ������Ʈ ���� ����
	------------------*/

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
	unsigned char		ROOMNO;
	unsigned char		NUM_OBJINFO;
	unsigned char		NUM_ACTIONINFO;

	unsigned int		OBJECTNO;
};

struct NGPMSG {
	MSGHEADER			header;
	char				objinfo[OBJINFOBUFSIZE];
	char				actioninfo[ACTIONINFOBUFSIZE];
};


// �޼����� ������.
// Ÿ��, ������, �� ����, ������Ʈ ����, ���λ���
inline NGPMSG* CreateMSG(UCHAR type, UCHAR roomno, UINT objectno, UCHAR nObjinfo, UCHAR nActioninfo, void* objinfo, void* actioninfo)
{
	NGPMSG* msg = new NGPMSG();

	MSGHEADER msgHeader = { type, roomno, nObjinfo, nActioninfo , objectno };

	msg->header = msgHeader;
	if (nActioninfo != 0)
		memcpy(msg->actioninfo, actioninfo, MSGSIZE::SIZE_ACTIONINFO*nActioninfo);

	if (nObjinfo != 0)
		memcpy(msg->objinfo, objinfo, MSGSIZE::SIZE_OBJINFO*nObjinfo);

	return msg;
}


inline int DispatchMSG(NGPMSG* msg, ActionInfo& actionlist, ObjInfo& objlist)
{
	int num_actioninfo = msg->header.NUM_ACTIONINFO;
	int num_objinfo = msg->header.NUM_OBJINFO;

	if (num_actioninfo > 0)
		memcpy(&actionlist, msg->actioninfo, MSGSIZE::SIZE_ACTIONINFO * num_actioninfo);

	if (num_objinfo > 0)
		memcpy(&objlist, msg->objinfo, MSGSIZE::SIZE_OBJINFO * num_objinfo);

	return msg->header.MSGTYPE;
}