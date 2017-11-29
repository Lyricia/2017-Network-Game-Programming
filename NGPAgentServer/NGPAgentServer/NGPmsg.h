#pragma once
#include "stdafx.h"

namespace MSGTYPE {

	/*-----------------
	
	0 - 새로운 룸을 만든다.
	1 - 클라이언트가 준비되었다.
	2 - 클라이언트의 게임이 종료되었다.
	3 - 에이전트 방 생성 요청을 받는다.
	4 - 에이전트의 정보

	-------------------*/
	enum MSGSTATE {
		ROOMCREATION,
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
		MOVE,
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

// 메세지를 만들어낸다.
// 타입, 사이즈, 룸 정보, 오브젝트 정보, 세부사항
inline NGPMSG* CreateMSG(short type, short size, short roomno, short objectno, void* detail)
{
	NGPMSG* msg = new NGPMSG();

	// 메세지 헤더에 타입, 크기, 
	MSGHEADER msgheader = { type, size, roomno, objectno };
	msg->header = msgheader;
	msg->detailinfo = detail;

	return msg;
}

// 메세지 처리
inline void DispatchMSG(NGPMSG* msg)
{

}