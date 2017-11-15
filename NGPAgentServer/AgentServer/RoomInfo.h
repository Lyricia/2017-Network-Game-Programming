#pragma once
#include"stdafx.h"
#include "Server.h"
#include "Agent.h"

class RoomInfo {

private:
	UINT 	            	id;
	ConnectedServerInfo    		server_info;
	//std::list<ClientInfo*>    		client_list;
	std::list<Agent*>         	agent_list;
	//GameWorld	           		gameworld;
	//MsgQueue	            	msg_queue;
	HANDLE 	            	hGameWorld;
public:

	void UpdateWorld(void);
	void SendMsgs(void);
	
		


};