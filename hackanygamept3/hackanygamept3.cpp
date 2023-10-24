#include <iostream>
#include <vector>
#include <windows.h>
#include "proc.h"
#include "mem.h"

int main()
{
	HANDLE hProcess = 0; //プロセスハンドルを格納する変数 hProcess を宣言し、初期値を0に設定

	uintptr_t moduleBase = 0, localPlayerPtr = 0, healthAddr = 0;//メモリアドレスを格納するための変数を宣言。
	//moduleBase はゲームプロセスのベースアドレス
	//localPlayerPtr はプレイヤーキャラクターのデータを指すポインタ
	//healthAddr はプレイヤーの体力情報を指すアドレス

	bool bHealth = false, bAmmo = false, bRecoil = false;//チートの有効/無効の状態を示すためのブール変数 bHealth, bAmmo, bRecoil を宣言

	const int newValue = 1337;

	DWORD procId = GetProcId(L"ac_client.exe");//GetProcId 関数を使用して、指定した名前のプロセス（ac_client.exe）のプロセスIDを取得

	if (procId)//もしProcIdが0以外なら
	{

		hProcess = OpenProcess(PROCESS_ALL_ACCESS, NULL, procId);//プロセスのハンドルを取得

		moduleBase = GetModuleBaseAddress(procId, L"ac_client.exe");//モジュールのベースアドレス(チートエンジンで言う所のac_client.exe)を取得

		localPlayerPtr = moduleBase + 0x10f4f4;//ローカルプレイヤーのポインタを指定

		healthAddr = FindDMAAddy(hProcess, localPlayerPtr, { 0xf8 });//ローカルプレイヤーのHPアドレスへのポインタをリゾルブ

	}

	else//もしProcIdが0(エラー)なら
	{

		std::cout << "Process not found. Press Enter\n";
		getchar();
		return 0;

	}

	DWORD dwExit = 0;

	while (GetExitCodeProcess(hProcess, &dwExit) && dwExit == STILL_ACTIVE)// このループは、外部プロセスがアクティブ（実行中）である限り続く、
	//GetExitCodeProcess 関数を使用して、プロセスの終了コードを取得し、それが STILL_ACTIVE である場合、プロセスが実行中であることを確認
	{

		if (GetAsyncKeyState(VK_F1) & 1)//F1キーが押された場合、bHealthが反転
		{
			bHealth = !bHealth;
		}

		if (GetAsyncKeyState(VK_F2) & 1)//F2キーが押された場合、bAmmoが反転
		{
			bAmmo = !bAmmo;

			if (bAmmo)//もしbAmmoが1なら
			{	//ff 06 = inc [esi];
				mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x06", 2, hProcess);//inc [esi];のasmをac_client.exe + 0x637e9に書き込み
			}
			else//0なら
			{	//ff 0E = dec [esi];
				mem::PatchEx((BYTE*)(moduleBase + 0x637e9), (BYTE*)"\xFF\x0E", 2, hProcess);//asmを元通りにして、命令を元通りに
			}
		}


		if (GetAsyncKeyState(VK_F3) & 1)//F3キーが押された場合、bRecoilが反転
		{

			bRecoil = !bRecoil;

			if (bRecoil)//もしbRecoilが1なら
			{
				mem::NopEx((BYTE*)(moduleBase + 0x63786), 10, hProcess);//nopを書き込み
			}
			else
			{
				mem::PatchEx((BYTE*)(moduleBase + 0x63786), (BYTE*)"\x50\x8d\x4c\x24\x1c\x51\x8b\xce\xff\xd2", 10, hProcess);//これで元通り
			}

		}

		if (GetAsyncKeyState(VK_INSERT) & 1)//insertが押されたら
		{
			return 0; //main関数を抜ける
		}


		if (bHealth)//bHealthが1なら
		{
			mem::PatchEx((BYTE*)healthAddr, (BYTE*)&newValue, sizeof(newValue), hProcess);//healthAddrにnewValueを書き込み
		}

		Sleep(10);//ループを10ミリ秒遅らせて処理を軽く

	}

	std::cout << "Process not found. Press Enter\n";
	getchar();
	return 0;

		
}

/*

使用した関数

	GetProcId  (ユーザー定義)
		L	CreateToolhelp32Snapshot

	OpenProcess

	GetModuleBaseAddress (ユーザー定義)
		L	CreateToolhelp32Snapshot

	FindDMAAddy (ユーザー定義)
		L	ReadProcessMemory

	GetExitCodeProcess

	GetAsyncKeyState

	mem::PatchEx (ユーザー定義)
		L	VirtualProtectEx

	mem::NopEx (ユーザー定義)
		L	memset
*/