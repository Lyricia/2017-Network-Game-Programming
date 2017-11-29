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
	
	0 - 새로운 룸을 만든다.
	1 - 클라이언트가 준비되었다.
	2 - 클라이언트의 게임이 종료되었다.
	3 - 에이전트 방 생성 요청을 받는다.
	4 - 에이전트의 정보

	-------------------*/
	enum MSGSTATE {
		ROOMCREATION = 10,
		CLIENTREADY,
		CLIENTGAMEOVER,
		AICREATTIONREQUEST,
		AIAGENTINFO
	};

	/*-----------------
	0 - 움직여
	1 - 발사해
	2 - 터렛을 지어
	3 - 재장전해
	------------------*/

	enum MSGACTION {
		MOVE = 20,
		SHOOT,
		BUILDTURRET,
		RELOAD
	};

	/*-----------------
	0 - 위치 보정
	1 - 오브젝트 생성
	2 - 오브젝트 삭제
	3 - 오브젝트 상태 갱신
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


// 메세지를 만들어낸다.
// 타입, 사이즈, 룸 정보, 오브젝트 정보, 세부사항
inline NGPMSG* CreateMSG(char type, short size, char roomno, char objectno, char nObjinfo, char nActioninfo, void* objinfo, void* actioninfo)
{
	NGPMSG* msg = new NGPMSG();

	// 메세지 헤더의 타입, 크기, 방 번호, 오브젝트 번호, 오브젝트 정보 갯수, 액션 정보 갯수
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