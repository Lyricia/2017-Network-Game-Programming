#pragma once

#define ACTIONINFOBUFSIZE	512
#define OBJINFOBUFSIZE		512
#define MAPINFOBUFSIZE		128	
#define INVALID_OBJECT_ID	-1

enum MSGSIZE {
	SIZE_HEADER = 8
	, SIZE_ACTIONINFO = 16
	, SIZE_OBJINFO = 40
	, SIZE_AMMUNITION = 4
};

namespace MSGTYPE {
	enum MSGSTATE {
		ROOMCREATION = 10
		, CLIENTREADY
		, CLIENTGAMEOVER
		, AICREATTIONREQUEST
		, AIAGENTINFO
	};

	enum MSGACTION {
		MOVE = 20
		, STOP
		, SHOOT
		, RELOAD
		, GRENADE
		, BUILDTURRET
	};

	enum MSGUPDATE {
		ADJUSTPOS = 30
		, CREATEOBJECT
		, DELETEOBJECT
		, UPDATEOBJECTSTATE
		, UPDATEMAPSTATE
	};
}

struct ActionInfo {
	union {
		struct { // MOVE
			D2D_POINT_2F	LookDirection;
			D2D_POINT_2F	MoveDirection;
		};
		struct { // SHOOT
			UINT			TargetID;
			D2D_POINT_2F	TargetHitPos;
			UCHAR			Packer4[4];
		};
		struct { // GRENADE
			D2D_POINT_2F	SetVelocity;
			D2D_POINT_2F	TargetPos;
		};
		struct { // RELOAD
			UCHAR			Packer16[16];
		};
	};
};

enum class OBJECTTYPE : UCHAR {
	Player = 0
	, Agent
	, Brick
	, Grenade
	, Effect_ShootingHit
	, Effect_Explosion
};

struct AMMUNITION {
	UCHAR		Granade;
	UCHAR		GunAmmo;
	UCHAR		TurretKit;
	UCHAR		packer1;
};

struct ObjInfo {
	UINT				ObjectID;
	float				HP;

	OBJECTTYPE			ObjectType;
	bool				Collision;
	USHORT				packer;

	D2D_POINT_2F		Position;
	union {
		AMMUNITION		Ammo;
		UINT			ParentID;
	};
	union {
		D2D_POINT_2F	Direction;
		D2D_POINT_2F	Grenade_ptCurrImg;
	};
	D2D_POINT_2F	Velocity;
};

struct MapInfo {
	UINT				ObjectID;
	float				HP;
};

struct MSGHEADER {
	UCHAR		MSGTYPE;
	UCHAR		ROOMNO;
	UCHAR		NUM_OBJINFO;
	UCHAR		NUM_ACTIONINFO;

	UINT		OBJECTNO;
};

struct NGPMSG {
	MSGHEADER			header;
	union {
		struct {
			char		objinfo[OBJINFOBUFSIZE];									// union member
			char		actioninfo[ACTIONINFOBUFSIZE];								// union member
		};

		MapInfo			Mapdata[MAPINFOBUFSIZE];		// union member
	};
};

inline NGPMSG* CreateMSG(UCHAR type, UCHAR roomno, UINT objectno, UCHAR nObjinfo, UCHAR nActioninfo, void* objinfo, void* actioninfo)
{
	NGPMSG* msg = new NGPMSG();

	MSGHEADER Header = {};

	Header.MSGTYPE = type;
	Header.OBJECTNO = objectno;
	Header.ROOMNO = roomno;
	Header.NUM_ACTIONINFO = nActioninfo;
	Header.NUM_OBJINFO = nObjinfo;

	msg->header = Header;
	if (nActioninfo != 0)
		memcpy(msg->actioninfo, actioninfo, MSGSIZE::SIZE_ACTIONINFO*nActioninfo);

	if (nObjinfo != 0)
		memcpy(msg->objinfo, objinfo, MSGSIZE::SIZE_OBJINFO*nObjinfo);

	return msg;
}

inline NGPMSG* CreateMSG(UCHAR type, UCHAR roomno, UINT objectno, UCHAR nMapinfo, MapInfo* mapdata)
{
	NGPMSG* msg = new NGPMSG();

	MSGHEADER Header = {};

	Header.MSGTYPE = type;
	Header.OBJECTNO = objectno;
	Header.NUM_OBJINFO = nMapinfo;
	Header.ROOMNO = roomno;

	msg->header = Header;
	memcpy(msg->Mapdata, mapdata, sizeof(MapInfo)*nMapinfo);

	return msg;
}

inline UCHAR DispatchMSG(NGPMSG* msg, ActionInfo* actionlist, ObjInfo* objlist)
{
	if (msg->header.NUM_ACTIONINFO > 0)
		memcpy(actionlist, msg->actioninfo, sizeof(ActionInfo) * msg->header.NUM_ACTIONINFO);

	if (msg->header.NUM_OBJINFO > 0)
		memcpy(objlist, msg->objinfo, sizeof(ObjInfo) * msg->header.NUM_OBJINFO);

	return msg->header.MSGTYPE;
}

inline UCHAR DispatchMSG(NGPMSG* msg, MapInfo* mapdata)
{
	memcpy(mapdata, msg->Mapdata, sizeof(MapInfo) * msg->header.NUM_OBJINFO);

	return msg->header.MSGTYPE;
}