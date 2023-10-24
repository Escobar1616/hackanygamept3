#pragma once

namespace mem
{
	//指定されたアドレスを書き換える
	//引数: 書き換えるメモリアドレスへのポインタ, 書き込むデータを含むアドレスへのポインタ, 書き込むデータのサイズ, プロセスへのハンドル
	void PatchEx(BYTE* dst, BYTE* src, unsigned int size, HANDLE hProcess);

	//指定されたアドレスにnop命令を挿入
	//引数: nop命令を挿入するアドレスへのポインタ, 挿入するnop命令の数, プロセスへのハンドル
	void NopEx(BYTE* dst, unsigned int size, HANDLE hProcess);

}