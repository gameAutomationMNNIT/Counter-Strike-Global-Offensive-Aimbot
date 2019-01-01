#include <windows.h>
/* 
  Updated for game update 22.12.2018
*/
//base addresses
const DWORD PlayerBase = 0xCBD6B4;		//dwLocalPlayer;
const DWORD EntityBase = 0x4CCDBFC;		//dwEntityList;
const DWORD ClientState = 0x58BCFC;		//dwClientState;
//Offsets with client.dll and playerbase
const DWORD CrossHairOffset = 0xB394;	//m_iCrosshairId;
const DWORD PositionOffset = 0x138;   //m_vecOrigin 
const DWORD TeamOffset = 0xF4;			  //m_iTeamNum
const DWORD HealthOffset = 0x100;     //m_iHealth 
const DWORD MemoryIncrement = 0x10;
//Offsets with engine.dll
const DWORD ClientStateViewAngleOffset = 0x4D10;  //dwClientState_ViewAngles 
