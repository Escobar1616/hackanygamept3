#include <windows.h>
#include <iostream>
#include "mem.h"

void mem::PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess)
{

	DWORD oldprotect; //後でメモリアクセス許可を元に戻す時に使う
	VirtualProtectEx(hProcess, dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);//メモリアクセス許可を変更。変更前の許可情報はoldprotect変数に保存される
	WriteProcessMemory(hProcess, dst, src, size, nullptr);//外部プロセスのメモリにデータを書き込み
	VirtualProtectEx(hProcess, dst, size, oldprotect, &oldprotect);//変更前のアクセス許可をoldprotectから復元。この操作により、メモリアクセス許可が元の状態に戻る

}

void mem::NopEx(BYTE* dst, unsigned int size, HANDLE hProcess)
{

	BYTE* nopArray = new BYTE[size];//nopArrayという名前のBYTE型の配列を作成し、そのサイズをsizeに設定
	memset(nopArray, 0x90, size);//nopArrayの各要素を0x90（NOP命令のバイナリ表現）で初期化し、メモリアドレスにNOP命令を配置

	PatchEx(dst, nopArray, size, hProcess);//PatchEx関数を呼び出し、dstメモリアドレスにnopArray（NOP命令の配列）を指定されたsizeとhProcessで書き込み
	delete[] nopArray;//動的に割り当てられたnopArrayメモリを解放。メモリリーク対策

}