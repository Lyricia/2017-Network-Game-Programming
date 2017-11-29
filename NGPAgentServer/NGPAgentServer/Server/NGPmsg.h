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
	unsigned char	Granade;
	unsigned char	GunAmmo;
	unsigned char	TurretKit;
};

struct ObjInfo {
	unsigned char		ObjectID;
	D2D_POINT_2F		Position;
	D2D_POINT_2F		Direction;
	unsigned char		HP;
	AMMUNITION			Ammo;
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
	char				objinfo[OBJINFOBUFSIZE];
	char				actioninfo[ACTIONINFOBUFSIZE];
};


// �޼����� ������.
// Ÿ��, ������, �� ����, ������Ʈ ����, ���λ���
inline NGPMSG* CreateMSG(char type, short size, char roomno, char objectno, char nObjinfo, char nActioninfo, void* objinfo, void* actioninfo)
{
	NGPMSG* msg = new NGPMSG();

	// �޼��� ����� Ÿ��, ũ��, �� ��ȣ, ������Ʈ ��ȣ, ������Ʈ ���� ����, �׼� ���� ����
	MSGHEADER msgHeader = { type, size, roomno, objectno, nObjinfo, nActioninfo };

	msg->header = msgHeader;
	memcpy(msg->actioninfo, actioninfo, MSGSIZE::SIZE_ACTIONINFO*nActioninfo);
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