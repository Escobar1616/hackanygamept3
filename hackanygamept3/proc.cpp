#include "proc.h"

//プロセス名からプロセスIDを取得するための関数
DWORD GetProcId(const wchar_t* procName) //procNameという名前のwchar_t型ポインタを受け取る
{
	DWORD procId = 0; //エラー確認用に0に初期化しとく
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);//プロセスのスナップショットを作る

	if (hSnap != INVALID_HANDLE_VALUE)//hSnapがINVALID_HANDLE_VALUEじゃないか確認
	{
		PROCESSENTRY32 procEntry; //ストラクチャをprocEntryという名前で実体化
		procEntry.dwSize = sizeof(procEntry);//procEntry内のdwSizeというメンバ変数にprocEntryというストラクチャのサイズを代入

		if (Process32First(hSnap, &procEntry))//プロセスのスナップショットの最初のプロセスエントリを取得
		{

			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName))//procEntry内のszExeFileメンバとprocNameを比較。_wcsicmp関数は大文字小文字を区別せずに文字列を比較するためのもの。
				{
					procId = procEntry.th32ProcessID;
					break;//一致するものが見つかったら、そのプロセスのIDをprocIdに代入、ループを終える
				}
			} while (Process32Next(hSnap, &procEntry));//スナップショット内のプロセスエントリを、指定したプロセス名に一致するものが見つかるまで順番に総当たり

		}
	}
	CloseHandle(hSnap);
	return procId;//プロセスidを返す
}

//プロセスIDとモジュール名からモジュールのBaseAddressを取得するための関数
uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
	uintptr_t modBaseAddr = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);//引数として指定されたprocIdのモジュールのスナップショットを作成

	if (hSnap != INVALID_HANDLE_VALUE)//hSnapがINVALID_HANDLE_VALUEじゃないか確認
	{
		MODULEENTRY32 modEntry;//ストラクチャをmodEntryという名前で実体化
		modEntry.dwSize = sizeof(modEntry);//modEntry内のdwSizeというメンバ変数にmodEntryというストラクチャのサイズを代入

		if(Module32First(hSnap,&modEntry))//モジュールのスナップショットの、最初のモジュールエントリを取得
		{
			do
			{
				if (!_wcsicmp(modEntry.szModule, modName))//modEntry内のszModuleメンバとmodNameを比較。
				{
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;//一致するものが見つかったら、そのモジュールのベースアドレスをmodBaseAddrに代入、ループを終える
				}
			} while (Module32Next(hSnap, &modEntry));
		}

	}
	CloseHandle(hSnap);
	return modBaseAddr;


}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
	uintptr_t addr = ptr;

	for (unsigned int i = 0; i < offsets.size(); ++i) //iが0からoffsetsのサイズ未満である間、処理を繰り返す
	{
		ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);//RPM関数を使って、指定されたプロセスのメモリから"変数addr"に格納されている値を読み取って、その結果をaddrに代入する。結果、addrが示すメモリアドレスの値を取得する
		addr += offsets[i];//addrにoffsets[i]を加算する。　つまり、次のアドレスを計算する
	}
	return addr;//オフセットを加算したアドレスをaddrとして返す
}


//とりあえずwindows APIの関数と、変数の形を覚えてればおｋ