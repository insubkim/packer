#include <iostream>
#include <assert.h>
#include <windows.h>
#include <fileapi.h>

void	printCurrentDir(void)
{
	WCHAR currentDirectory[MAX_PATH];
	DWORD result = GetCurrentDirectory(MAX_PATH, currentDirectory);

	if (result == 0) {
		printf("Error getting current directory: %lu\n", GetLastError());
	}
	else if (result > MAX_PATH) {
		printf("Buffer too small for current directory path.\n");
	}
	else {
		printf("Current working directory: %s\n", currentDirectory);
	}
}


int main(void)
{
	const WCHAR *file_name = L"test.exe";

	HANDLE hFile = CreateFile(file_name, GENERIC_READ, FILE_SHARE_READ, NULL,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	
	assert(hFile != INVALID_HANDLE_VALUE);

	HANDLE hFileMap = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	DWORD dwSize = GetFileSize(hFile, 0);
	LPBYTE lpFileBase = (LPBYTE)MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, dwSize);
	//PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)lpFileBase;
	PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)GetModuleHandle(NULL);
	assert(pDosHeader != NULL);

	if (pDosHeader->e_magic != IMAGE_DOS_SIGNATURE)
		printf("NOT A PE FILE FORMAT");
	PIMAGE_NT_HEADERS pNtH = (PIMAGE_NT_HEADERS)((BYTE*)pDosHeader + pDosHeader->e_lfanew);
	PIMAGE_FILE_HEADER pIFH = &pNtH->FileHeader;
	PIMAGE_OPTIONAL_HEADER pIOH = (PIMAGE_OPTIONAL_HEADER)&pNtH->OptionalHeader;
	PIMAGE_SECTION_HEADER pISH = (PIMAGE_SECTION_HEADER)((PBYTE)pIOH + sizeof(IMAGE_OPTIONAL_HEADER));

	/*
		NTHEADER - signature, file header, optional header
		SECTION_HEADER 
	*/

	for (int i = 0; i < pIFH->NumberOfSections; ++i)
	{
		printf(" %02d %s \r\n", i + 1, pISH[i].Name);
	}





	return 0;
}
