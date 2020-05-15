#include "pch.h"

#define BUFF_SIZE 1500

void ANSI_To_Unicode(LPCSTR);
void Unicode_To_ANSI(LPCSTR);
void ShowError();

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, "rus");
	std::cout << "Для запуска программы вам нужно ввести:" << std::endl;
	std::cout << "\t+ ключи исходной кодировки файла:\n\t\t-a – ANSI файл;\n \t\t-u – Юникод файл.\n" ;
	std::cout << "\t+ название файла для записи." << std::endl;

	if (argc != 3)
	{
		std::cout << "Error! Вы должны ввести один из предложенных ключей!" << std::endl;
		std::cout << "\t+ ключи исходной кодировки файла:\n\t\t-a – ANSI файл;\n \t\t-u – Юникод файл.\n";
		std::cout << "\t+ название файла для записи." << std::endl;
		return 0;
	}
	else
	{
		if (strcmp(argv[1], "-a") == 0)
			ANSI_To_Unicode(argv[2]);
		else if (strcmp(argv[1], "-u") == 0)
			Unicode_To_ANSI(argv[2]);
		else
			printf("Введены неверные данные\n");
	}
	return 0;
}

void ANSI_To_Unicode(LPCSTR fileName)
{
	HANDLE handleSourceFile, handleDestinationFile; // handles to files 

	CHAR ansiBuff[BUFF_SIZE]; // buff to read text from source file
	WCHAR unicodeBuff[BUFF_SIZE]; // buff contain encoded text

	DWORD countRdChars, countWrWChars; // counters of symbols in ReadFile and symbols in WriteFile  
	BOOL resultFile = true;

	CHAR nameDestinationFile[] = "C:\\Users\\tom03\\source\\repos\\Lab1\\ANSI_Text_To_Unicode.txt"; // name to file with recoded text
	printf("\nРезультат можно увидеть -> %s\n", nameDestinationFile);

	handleSourceFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handleSourceFile == INVALID_HANDLE_VALUE)
	{
		ShowError();
		exit(EXIT_FAILURE);
	}

	handleDestinationFile = CreateFile(nameDestinationFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handleDestinationFile == INVALID_HANDLE_VALUE)
	{
		ShowError();
		CloseHandle(handleSourceFile);
		exit(EXIT_FAILURE);
	}

	// cycle read all file in blocks
	while (((resultFile = ReadFile(handleSourceFile, &ansiBuff, sizeof(ansiBuff), &countRdChars, NULL)) != false) 
		&& (countRdChars != 0))
	{
		int sizeToAllocate = MultiByteToWideChar(CP_UTF8, 0, ansiBuff, countRdChars, NULL, 0);

		// recode text to Unicode from ANSI
		MultiByteToWideChar(CP_UTF8, 0, ansiBuff, sizeToAllocate, unicodeBuff, 1024);

		// write recoded block to new file
		if ((!WriteFile(handleDestinationFile, unicodeBuff, 1024 * sizeof(WCHAR), &countWrWChars, NULL)) ||
			(countWrWChars != 1024 * sizeof(WCHAR)))
		{
			ShowError();
			CloseHandle(handleSourceFile);
			exit(EXIT_FAILURE);
		}
	}
	if (resultFile == false && countRdChars != 0)
	{
		ShowError();
		exit(EXIT_FAILURE);
	}

	CloseHandle(handleSourceFile);
	CloseHandle(handleDestinationFile);
}

void Unicode_To_ANSI(LPCSTR fileName)
{
	HANDLE handleSourceFile, handleDestinationFile; // handles to files 

	WCHAR unicodeBuff[BUFF_SIZE]; // buff to read text from source file
	CHAR  ansiBuff[BUFF_SIZE]; // buff contain encoded ASCI text to Unicode

	DWORD countReadBytes, countWriteBytes; // counters of read Symbols(in ReadFIle) and writen Symbols(in WriteFile)  
	BOOL resultFile = true;

	CHAR nameDestinationFile[] = "C:\\Users\\tom03\\source\\repos\\Lab1\\Unicode_Text_To_ANSI.txt"; // name to file with recoded text
	printf("\nРезультат можно увидеть -> %s\n", nameDestinationFile);

	handleSourceFile = CreateFile(fileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handleSourceFile == INVALID_HANDLE_VALUE)
	{
		ShowError();
		exit(EXIT_FAILURE);
	}

	handleDestinationFile = CreateFile(nameDestinationFile, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (handleDestinationFile == INVALID_HANDLE_VALUE)
	{
		ShowError();
		CloseHandle(handleSourceFile);
		exit(EXIT_FAILURE);
	}

	// read first 2 bytes which mean ending style (BigEndian or LittleEndian), 'cause it marker bytes for unicode
	CHAR rawsBytesBuff[2];
	if (!ReadFile(handleSourceFile, &rawsBytesBuff, 2, &countReadBytes, NULL))
	{
		ShowError();
		exit(EXIT_FAILURE);
	}
	if (!isalnum(rawsBytesBuff[0]))
		SetFilePointer(handleSourceFile, 2, NULL, FILE_BEGIN);
	else
		SetFilePointer(handleSourceFile, 0, NULL, FILE_BEGIN);

	 //cycle read all file in blocks
	while (((resultFile = ReadFile(handleSourceFile, &unicodeBuff, 1024, &countReadBytes, NULL)) != false)
		&& (countReadBytes != 0))
	{
		int ansiLength = WideCharToMultiByte(CP_ACP, 0, unicodeBuff, countReadBytes/2, NULL, 0, NULL, NULL);

		WideCharToMultiByte(CP_ACP, 0, unicodeBuff, 1024, ansiBuff, ansiLength, NULL, NULL);

		// write recoded block to new file
		if ((!WriteFile(handleDestinationFile, ansiBuff, ansiLength, &countWriteBytes, NULL)) || (countWriteBytes != ansiLength))
		{
			ShowError();
			CloseHandle(handleSourceFile);
			exit(EXIT_FAILURE);
		}

		if (GetFileSize(handleDestinationFile, NULL) >= GetFileSize(handleSourceFile, NULL)/2)
			resultFile = false;
	}
	if (resultFile == false && countReadBytes != 0)
	{
		ShowError();
		exit(EXIT_FAILURE);
	}

	CloseHandle(handleSourceFile);
	CloseHandle(handleDestinationFile);
}

void ShowError()
{
	LPVOID lpMsgBuf;
	DWORD dwLastError = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dwLastError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);
	printf("\n%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
	return;
}