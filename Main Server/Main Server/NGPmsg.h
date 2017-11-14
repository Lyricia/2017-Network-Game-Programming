#pragma once

struct D2D_POINT_2F {};

namespace MSGTYPE {
	enum MSGSTATE {
		ROOMCREATION,
		CLIENTREADY,
		CLIENTGAMEOVER,
		AICREATTIONREQUEST,
		AIAGENTINFO
	};

	enum MSGACTION {
		MOVE,
		SHOOT,
		BUILDTURRET,
		RELOAD
	};

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

inline NGPMSG* CreateMSG(short type, short size, short roomno, short objectno, void* detail)
{
	NGPMSG* msg = new NGPMSG();
	
	MSGHEADER msgheader = { type, size, roomno, objectno };
	msg->header = msgheader;
	msg->detailinfo = detail;

	return msg;
}

inline void DispatchMSG(NGPMSG* msg)
{

}