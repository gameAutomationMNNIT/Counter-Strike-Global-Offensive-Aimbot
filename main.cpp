#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <sstream>
#include "constant.cpp"

//Memory vars
DWORD ClientMemoryAddress;
DWORD ServerMemoryAddress;
DWORD EngineMemoryAddress;
//Global vars
DWORD LocalPlayer;
DWORD client;
int LocalTeam;
int CrossHairID;
int NthNearest = 0;	//0 means nearest enemy
int ycorr = 0;
int enemyID[32];
float enemyDistance[32];
//Function Definations
void Trigger();
int getAllData();
void getMyData();
void AimtoNearest(int);
void changeAngle(float, float);
//externs
extern const DWORD PlayerBase;
extern const DWORD EntityBase;
extern const DWORD ClientState;
extern const DWORD CrossHairOffset;
extern const DWORD PositionOffset;
extern const DWORD TeamOffset;
extern const DWORD HealthOffset;
extern const DWORD MemoryIncrement;
extern const DWORD AngleXOffset;
extern const DWORD AngleYOffset;
extern const DWORD ClientStateViewAngleOffset;

struct EntityInfo
{
	float x, y, z, distance;
	int health, teamID;
}entity[32],player;

class mProcess
{
	protected:

		HANDLE hProcess;
		DWORD dwPID, dwProtection, dwCaveAddress;
		BOOL bPOn, bIOn, bProt;

	public:
		~mProcess()
		{
			CloseHandle(hProcess);
		}


		template <class cData>
		cData Read(DWORD dwAddress)
		{
			cData cRead;
			ReadProcessMemory(hProcess, (LPVOID)dwAddress, &cRead, sizeof(cData), NULL);
			return cRead;
		}

		template<class c>
		BOOL Write(DWORD dwAddress, c ValueToWrite)
		{
			return WriteProcessMemory(hProcess, (LPVOID)dwAddress, &ValueToWrite, sizeof(c), NULL);
		}

		virtual bool Process(char* ProcessName)
		{
			HANDLE hPID = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
			PROCESSENTRY32 ProcEntry;
			ProcEntry.dwSize = sizeof(ProcEntry);
			do
			{
				if (!strcmp(ProcEntry.szExeFile, ProcessName))
				{
				dwPID = ProcEntry.th32ProcessID;
				CloseHandle(hPID);


				hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID);
				return false;
				}
			}
			while (Process32Next(hPID, &ProcEntry));

			std::cout << "\nPROCESS ERROR: Is csgo.exe running?\n";
			system("pause");
			return true;
		}

		virtual DWORD Module(LPSTR ModuleName)
		{
			HANDLE hModule = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);
			MODULEENTRY32 mEntry;
			mEntry.dwSize = sizeof(mEntry);
			do
			{
				if (!strcmp(mEntry.szModule, ModuleName))
				{
				CloseHandle(hModule);
				return (DWORD)mEntry.modBaseAddr;
				}
			}
			while (Module32Next(hModule, &mEntry));
			std::cout << "\nACCESS DENIDED: Please run this as an administrator!!\n";
			system("pause");
			exit(0);
			return 0;
		}

}Memory;

void Trigger()
{
	DWORD LocalPlayer = Memory.Read<DWORD>(ClientMemoryAddress + PlayerBase);
	CrossHairID = Memory.Read<int>(LocalPlayer + CrossHairOffset);
/*				NOT WORKING			*/
//	DWORD EnemyInCrossHair = Memory.Read<DWORD>(ClientMemoryAddress + EntityBase + ((CrossHairID - 1) * MemoryIncrement));
//	int EnemyHealth = Memory.Read<int>(EnemyInCrossHair + HealthOffset);
//	int EnemyTeam = Memory.Read<int>(EnemyInCrossHair + TeamOffset);
	if (CrossHairID > 0)
    {
		Sleep(10);
		mouse_event(MOUSEEVENTF_LEFTDOWN, NULL, NULL, NULL, NULL);
		mouse_event(MOUSEEVENTF_LEFTUP, NULL, NULL, NULL, NULL);
	}
}

int getAllData()
{
	int c=0;
	bool flag = false;
	for(int i=1 ; i<32 ; i++)
	{
		DWORD CurrentBaseAddress = Memory.Read<DWORD>(ClientMemoryAddress + EntityBase + (i * MemoryIncrement));
		entity[i].x = Memory.Read<float>(CurrentBaseAddress + PositionOffset);
		entity[i].y = Memory.Read<float>(CurrentBaseAddress + PositionOffset + 4);
		entity[i].z = Memory.Read<float>(CurrentBaseAddress + PositionOffset + 8);
		entity[i].health = Memory.Read<int>(CurrentBaseAddress + HealthOffset);
		entity[i].teamID = Memory.Read<int>(CurrentBaseAddress + TeamOffset);
		entity[i].distance = sqrt(  pow((player.x-entity[i].x),2) + pow((player.y-entity[i].y),2) + pow((player.z-entity[i].z),2)  );
		if(entity[i].health > 0 && player.teamID != entity[i].teamID)
		{
			enemyDistance[c] = entity[i].distance;
			enemyID[c++] = i;
		}
	}
	for(int i=0;i<c-1;i++)
	{
		for(int j=i+1;j<c;j++)
		{
			if(enemyDistance[j]<enemyDistance[i])
			{
				enemyDistance[i]=enemyDistance[i]+enemyDistance[j];
				enemyDistance[j]=enemyDistance[i]-enemyDistance[j];
				enemyDistance[i]=enemyDistance[i]-enemyDistance[j];
				enemyID[i] = enemyID[i] + enemyID[j];
				enemyID[j] = enemyID[i] - enemyID[j];
				enemyID[i] = enemyID[i] - enemyID[j];
			}
		}
	}
	return c;
}

void getMyData()
{
	DWORD CurrentBaseAddress = Memory.Read<DWORD>(ClientMemoryAddress + PlayerBase);
	player.x = Memory.Read<float>(CurrentBaseAddress + PositionOffset);
	player.y = Memory.Read<float>(CurrentBaseAddress + PositionOffset + 4);
	player.z = Memory.Read<float>(CurrentBaseAddress + PositionOffset + 8);
	player.health = Memory.Read<int>(CurrentBaseAddress + HealthOffset);
	player.teamID = Memory.Read<int>(CurrentBaseAddress + TeamOffset);
	player.distance = -1;
}

void AimtoNearest(int i)
{
	float distance_X = entity[i].x - player.x;
    float distance_Y = entity[i].y - player.y;
	float distance_Z = entity[i].z - player.z - ycorr;
    float distance_XY_Plane = sqrt(pow(distance_X, 2) + pow(distance_Y, 2));
	if((distance_X / distance_XY_Plane) > 1 || (distance_X / distance_XY_Plane) < -1)
		return;
    float x_r = acos(distance_X / distance_XY_Plane) * 180 / 3.141592;
    x_r *= (entity[i].y < player.y) ? -1 : 1;
    float y_r = -1 * atan(distance_Z / distance_XY_Plane) * 180 / 3.141592;
	changeAngle((float)x_r, (float)y_r);
}

void changeAngle(float xAngle, float yAngle)
{
    Memory.Write(client + ClientStateViewAngleOffset + 4, xAngle);
	Memory.Write(client + ClientStateViewAngleOffset, yAngle);
}

int main()
{
	int aimto = 0;
	std::cout<<"Counter Strike Global Offensive Aimbot\n";
	std::cout<<"Finding csgo.exe\n";
	while(Memory.Process("csgo.exe"));
	std::cout<<"Found csgo.exe\n";
	ClientMemoryAddress = Memory.Module("client.dll");
	ServerMemoryAddress = Memory.Module("server.dll");
	EngineMemoryAddress = Memory.Module("engine.dll");
	client = Memory.Read<DWORD>(EngineMemoryAddress + ClientState);
	DWORD LocalPlayer = Memory.Read<DWORD>(ClientMemoryAddress + PlayerBase);
	LocalTeam = Memory.Read<int>(LocalPlayer + TeamOffset);
	CrossHairID = Memory.Read<int>(LocalPlayer + CrossHairOffset);
	int choice=0;
	bool autofire=false, autoaim=false;
	system("pause");
	while(true)
    {
		if(GetAsyncKeyState(VK_NUMPAD6)&1)
			aimto++;
		if(GetAsyncKeyState(VK_NUMPAD4)&1)
			aimto--;
		if(GetAsyncKeyState(VK_NUMPAD5)&1)
			autoaim=!autoaim;
		if(GetAsyncKeyState(VK_NUMPAD0)&1)
			autofire=!autofire;
		if(GetAsyncKeyState(VK_NUMPAD8)&1)
			ycorr--;
		if(GetAsyncKeyState(VK_NUMPAD2)&1)
			ycorr++;
		if(ycorr < -20)
			ycorr = -20;
		if(ycorr > 50)
			ycorr = 50;
		system("cls");
		autoaim?std::cout<<"AutoAim: on\t\t[NUMPAD5 to toggle]\n":std::cout<<"AutoAim: off\t\t[NUMPAD5 to toggle]\n";
		autofire?std::cout<<"AutoFire: on\t\t[NUMPAD0 to toggle]\n":std::cout<<"AutoFire: off\t\t[NUMPAD0 to toggle]\n";
		if(autoaim)
			std::cout<<"Aimed to "<<aimto+1<<" nearest\t[NUMPAD4 to nearer | NUMPAD6 to farther player]\n";
		std::cout<<"Aim correction "<<ycorr<<"\t[NUMPAD8,2 to adjust autoaim]\n"<<std::endl;
		getMyData();
		int noofActivePlayers = getAllData();
		if(aimto<0)
			aimto = 0;
		if(aimto>noofActivePlayers-1)
			aimto=noofActivePlayers-1;
		if(autoaim && noofActivePlayers)
		{	
			AimtoNearest(enemyID[aimto]);
		}
		if(autofire)
			Trigger();
	}
	return 0;
}

