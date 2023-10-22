#include <iostream>
#include <iostream>
#include <vector>
#include <windows.h>
#include "proc.h"



int main()
{
   //ProcessIdを取得
	DWORD procId = GetProcId(L"ac_client.exe");

	std::cout << "procID = " << procId << std::endl;

	//モジュールのbaseaddrを取得
	uintptr_t moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");

	std::cout << "moduleBase = " << moduleBase << std::endl;


	//プロセスへのハンドルを取得
	HANDLE hProcess = 0;
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);
	
	std::cout << "hProcess = " << hProcess << std::endl;

	//ammoポインターチェーンのbaseaddrをリゾルブ
	uintptr_t dynamicPtrBaseAddr = moduleBase + 0x10f4f4;

	std::cout << "DynamicPtrBaseAddr = " << "0x" << std::hex << dynamicPtrBaseAddr << std::endl;

	//ammoポインターチェーンをリゾルブ
	std::vector<unsigned int>ammoOffsets = {0x374, 0x14, 0x0};
	uintptr_t ammoAddr = FindDMAAddy(hProcess, dynamicPtrBaseAddr, ammoOffsets);

	std::cout << "ammoAddr = " << "0x" << std::hex << ammoAddr << std::endl;

	//ammoのvalueを読む
	int ammoValue = 0;

	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);
	std::cout << "Current ammo = " << std::dec << ammoValue << std::endl;



	//それに書き込む
	int newAmmo = 1337;
	WriteProcessMemory(hProcess, (BYTE*)ammoAddr, &newAmmo, sizeof(newAmmo), nullptr);


	//もう一度読む
	ReadProcessMemory(hProcess, (BYTE*)ammoAddr, &ammoValue, sizeof(ammoValue), nullptr);

	std::cout << "New ammo = " << std::dec << ammoValue << std::endl;


	//Enter待機
	getchar();
	return 0;
}

/*
	使用した関数

	GetProcId (ユーザー定義)
	GetModuleBaseAddress (ユーザー定義)
	OpenProcess
	FindDMAAddy (ユーザー定義)
	ReadProcessMemory
	WriteProcessMemory

*/
