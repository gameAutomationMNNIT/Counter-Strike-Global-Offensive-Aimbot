#include <windows.h>
// for Executable version:Client v1.36.6.9 (14.12.2018)
//base addresses
const DWORD PlayerBase = 0xCB3694;		//dwLocalPlayer;
const DWORD EntityBase = 0x4CC3564;		//dwEntityList;
const DWORD ClientState = 0x58ACFC;		//dwClientState;
//Offsets with client.dll and playerbase
const DWORD CrossHairOffset = 0xB390;	//m_iCrosshairId;
const DWORD PositionOffset = 0x138;   //m_vecOrigin 
const DWORD TeamOffset = 0xF4;			  //m_iTeamNum
const DWORD HealthOffset = 0x100;     //m_iHealth 
const DWORD MemoryIncrement = 0x10;
//Offsets with engine.dll
const DWORD ClientStateViewAngleOffset = 0x4D10;  //dwClientState_ViewAngles 
