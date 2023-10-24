#pragma once
#include <vector>
#include <windows.h>
#include <tlhelp32.h>

//検索するプロセスのお名前 -> プロセスID(見つからないなら0)
DWORD GetProcId(const wchar_t* procName);

//プロセスID -> モジュールのベースアドレス(見つからないなら0)
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName);

//プロセスのハンドル、基準となるアドレス、オフセットのベクタ -> オフセットを適用した後のアドレス 
uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int>);
