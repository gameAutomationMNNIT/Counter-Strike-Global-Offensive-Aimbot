#include <windows.h>

//base addresses
const DWORD PlayerBase = 0xA9ADEC;
const DWORD EntityBase = 0x4A77B6C;
const DWORD ClientState = 0x57D844;
//Offsets with client.dll and playerbase
const DWORD CrossHairOffset = 0xB2A4;
const DWORD PositionOffset = 0x134;
const DWORD TeamOffset = 0xF0;
const DWORD HealthOffset = 0xFC;
const DWORD MemoryIncrement = 0x10;
//Offsets with engine.dll
const DWORD ClientStateViewAngleOffset = 0x4D10;