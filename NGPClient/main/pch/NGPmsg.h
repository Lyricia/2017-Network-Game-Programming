#pragma once

#define ACTIONINFOBUFSIZE	128
#define OBJINFOBUFSIZE		256

enum MSGSIZE {
	  SIZE_HEADER		= 8
	, SIZE_ACTIONINFO	= 16
	, SIZE_OBJINFO		= 24
	, SIZE_AMMUNITION	= 4
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
	};
}

struct ActionInfo {
	union {
		struct { // MOVE
			D2D_POINT_2F	MoveVelocity;
			UCHAR			Packer8[8];
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
	UCHAR				ObjectID;
	OBJECTTYPE			ObjectType;
	UCHAR				HP;
	bool				Collision;
	D2D_POINT_2F		Position;
	union {
		AMMUNITION		Ammo;
		UINT			ParentID;
	};
	union {
		D2D_POINT_2F	Direction;
		D2D_POINT_2F	Velocity;
	};
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
	char				objinfo[OBJINFOBUFSIZE];
	char				actioninfo[ACTIONINFOBUFSIZE];
};

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