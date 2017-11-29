#pragma once
#include "stdafx.h"

namespace MSGTYPE {

	/*-----------------
	
	0 - ���ο� ���� �����.
	1 - Ŭ���̾�Ʈ�� �غ�Ǿ���.
	2 - Ŭ���̾�Ʈ�� ������ ����Ǿ���.
	3 - ������Ʈ �� ���� ��û�� �޴´�.
	4 - ������Ʈ�� ����

	-------------------*/
	enum MSGSTATE {
		ROOMCREATION,
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
		MOVE,
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
		ADJUSTPOS,
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
	short MSGTYPE;
	short DETAILINFOSIZE;
	short ROOMNO;
	short OBJECTNO;
};

struct NGPMSG {
	MSGHEADER	header;
	void*		detailinfo;
};

// �޼����� ������.
// Ÿ��, ������, �� ����, ������Ʈ ����, ���λ���
inline NGPMSG* CreateMSG(short type, short size, short roomno, short objectno, void* detail)
{
	NGPMSG* msg = new NGPMSG();

	// �޼��� ����� Ÿ��, ũ��, 
	MSGHEADER msgheader = { type, size, roomno, objectno };
	msg->header = msgheader;
	msg->detailinfo = detail;

	return msg;
}

// �޼��� ó��
inline void DispatchMSG(NGPMSG* msg)
{

}