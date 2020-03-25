#include "iuserpatch.h"
#include "mg1search.h"

#include <vector>
#include <string>

/*int nFiles = 0;

extern IVoobly *g_pVoobly;

std::vector<std::string> recs;

void __stdcall oos_log(int time)
{
	g_pVoobly->Log("OOS: %d", time);
}

__declspec(naked) void rec_oos_hook() //0061EBE1
{
	__asm
	{
		cmp     esi, 3
		jz      _rec_oos
		mov     eax, 0061EC43h
		jmp     eax
_rec_oos:
		push    ecx
		push    edx
		mov     eax, [ebx]
		push    eax
		call    oos_log
		pop     edx
		pop     ecx
		mov     ecx, 006A3684h
		mov     ecx, [ecx]
		mov     eax, 0061EBECh
		jmp     eax
	}
}

int i = 0;

void __stdcall loadRec(char* str)
{
	if (i < nFiles)
	{
		strcpy(str, recs[i].c_str());
		g_pVoobly->Log("Loading %s (%d/%d)", str, i+1, nFiles);
		i++;
	}
}

__declspec(naked) void onLoadSave() //005EC580
{
	__asm
	{
		mov     eax, [esp + 4]
		push    ecx
		push    eax
		call    loadRec
		pop     ecx
		sub     esp, 104h
		mov     eax, 005EC586h
		jmp     eax
	}
}

void findFiles()
{
	//filenames = 0;
	nFiles = 0;
	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFile("savegame\\*.mg1", &fd);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		g_pVoobly->Log("FindFirstFile returned INVALID_HANDLE_VALUE");
	}
	else
	{
		do
		{
#ifdef _DEBUG
			log("found %s", fd.cFileName);
#endif
			/*nFiles++;
			filenames = (char**)realloc(filenames, sizeof(char*)*nFiles);
			filenames[nFiles - 1] = (char*)malloc(strlen(fd.cFileName) + 1);
			strcpy(filenames[nFiles - 1], fd.cFileName);
			filenames[nFiles - 1][strstr(fd.cFileName, ".rms") - fd.cFileName] = 0;
			recs.push_back(fd.cFileName);
			nFiles++;
		} while (FindNextFile(hFile, &fd));
		int err = GetLastError();
		if (err != ERROR_NO_MORE_FILES)
			g_pVoobly->Log("WARNING: FindNextFile(): unrecognised error %d", err);
		else
			g_pVoobly->Log("Finished listing files");
		FindClose(hFile);
	}
	g_pVoobly->Log("Found %d .mg1 files", nFiles);

	char b[100];
	sprintf(b, "%d", nFiles);
	MessageBox(0, b, "t", 0);
}*/

void setMg1SearchHooks()
{
	//findFiles();
	//setHook((void*)0x0061EBE1, rec_oos_hook);
	//setHook((void*)0x005EC580, onLoadSave);
}
