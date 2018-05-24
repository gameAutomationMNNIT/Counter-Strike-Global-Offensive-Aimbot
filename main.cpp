#include <windows.h>
#include <iostream>
#include <TlHelp32.h>
#include <sstream>

//Memory vars
DWORD ClientMemoryAddress;
DWORD ServerMemoryAddress;
DWORD EngineMemoryAddress;

//Global vars
DWORD LocalPlayer;
DWORD client;
int LocalTeam;
int CrossHairID;
int NearestPlayer = -1;
//Function Definations
void Trigger();
void getAllData();
void getMyData();
void AimtoNearest();
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

bool getAllData()
{
	float min = 9999999;
	NearestPlayer = -1;
	bool flag = false;
	for(int i=0;i<32;i++)
	{
		DWORD CurrentBaseAddress = Memory.Read<DWORD>(ClientMemoryAddress + EntityBase + (i * MemoryIncrement));
		entity[i].x = Memory.Read<float>(CurrentBaseAddress + PositionOffset);
		entity[i].y = Memory.Read<float>(CurrentBaseAddress + PositionOffset + 4);
		entity[i].z = Memory.Read<float>(CurrentBaseAddress + PositionOffset + 8) - 10;
		entity[i].health = Memory.Read<int>(CurrentBaseAddress + HealthOffset);
		entity[i].teamID = Memory.Read<int>(CurrentBaseAddress + TeamOffset);
		entity[i].distance = sqrt(  pow((player.x-entity[i].x),2) + pow((player.y-entity[i].y),2) + pow((player.z-entity[i].z),2)  );
		if(entity[i].health > 0 && player.teamID != entity[i].teamID && entity[i].distance < min)
		{
			flag = true;
			NearestPlayer = i;
			min = entity[i].distance;
		}
	}
	return flag;
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

void AimtoNearest()
{
	float distance_X = entity[NearestPlayer].x - player.x;
    float distance_Y = entity[NearestPlayer].y - player.y;
	float distance_Z = entity[NearestPlayer].z - player.z;
    float distance_XY_Plane = sqrt(pow(distance_X, 2) + pow(distance_Y, 2));
	if((distance_X / distance_XY_Plane) > 1 || (distance_X / distance_XY_Plane) < -1)
		return;
    float x_r = acos(distance_X / distance_XY_Plane) * 180 / 3.141592;
    x_r *= (entity[NearestPlayer].y < player.y) ? -1 : 1;
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
	std::cout<<"Counter Strike Global Offensive Aimbot\n";
	std::cout<<"Executable version:Client v1.36.3.4 (05.05.2018)\n";
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
	flag:
	std::cout<<"1: AutoAim only\n";
	std::cout<<"2: AutoFire only\n";
	std::cout<<"3: AutoAim and AutoFire\n";
	std::cout<<"4: Exit\n";
	int choice=0;
	bool autofire=false, autoaim=false;
	std::cin>>choice;
	switch(choice)
	{
	case 1:autoaim = true;
		break;
	case 2:autofire = true;
		break;
	case 3:autoaim = true;	autofire = true;
		break;
	case 4:system("pause");
		exit(0);
		break;
	default:std::cout<<"Enter a valid choice\n";
		goto flag;
		break;
	}
	system("pause");
	autoaim?std::cout<<"AutoAim: on\n":std::cout<<"AutoAim: off\n";
	autofire?std::cout<<"AutoFire: on\n":std::cout<<"AutoFire: off\n";
	std::cout<<"Note: Please disable friendly fire.";
	while(true)
    {
		getMyData();
		if(autoaim && getAllData())
			AimtoNearest();
		if(autofire)
			Trigger();
	}
	return 0;
}


