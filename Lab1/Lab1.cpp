#include "pch.h"

void GlobalMemoryInfoPrint();
void GetSystemInfoPrint();
bool ThrowOutOfMemoryError();
void ShowError();

int main(int argc, char* argv[])
{
	char cKey;

	setlocale(LC_ALL, "Rus");

	if (argc != 2)
	{
		std::cout << "Error! Вы должны ввести один из предложенных ключей!" << std::endl;
		std::cout << "\tКлючи:\n\t\t-e – печать ошибки;\n \t\t-s – печать информации о системе.\n"
			<< "Введите ключ для выбора операции -";
		return 0;
	}
	else
	{
		if (strcmp(argv[1], "-e") == 0) {
			if (ThrowOutOfMemoryError())
				ShowError();
			else
				printf("Слишком много свободного места.");
		}
		else if (strcmp(argv[1], "-s") == 0) {
			GlobalMemoryInfoPrint();
			GetSystemInfoPrint();
		}
		else
			printf("Вы должны ввести один из предложенных ключей!\n");
	}
}

// function for print information by GlobalMemoryStatus()
void GlobalMemoryInfoPrint()
{
	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);
	std::cout << std::endl;
	std::cout << "    *** GlobalMemoryStatus ***" << std::endl;
	std::cout << "  Approximate percentage of physical memory that is in use: " << ms.dwMemoryLoad << "%" << std::endl;
	// phisical memory
	std::cout << "  The  amount of actual physical memory: " << ms.dwTotalPhys << " bytes" << std::endl;
	std::cout << "  The amount of physical memory currently available: " << ms.dwAvailPhys << " bytes" << std::endl;
	// memory limit for process
	std::cout << "  The current size of the committed memory limit: " << ms.dwTotalPageFile << " bytes" << std::endl;
	std::cout << "  The maximum amount of memory the current process can commit: " << ms.dwAvailPageFile << " bytes" << std::endl;
	// virtual memory
	std::cout << "  The total amount of virtual memory: " << ms.dwTotalVirtual << " bytes" << std::endl;
	std::cout << "  The amount of availvable virtual memory: " << ms.dwAvailVirtual << " bytes" << std::endl;
	std::cout << std::endl;
}

// function for print information by GetSystemInfo()
void GetSystemInfoPrint()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	std::cout << "    *** GetSystemInfo ***" << std::endl;
	// processor
	std::cout << "  Processor architecture: ";
	switch (si.wProcessorArchitecture)
	{
		case 0: std::cout << "x86" << std::endl;
		break;
		case 5: std::cout << "ARM" << std::endl;
			break;
		case 6: std::cout << "Intel Itanium-based" << std::endl;
			break;
		case 9: std::cout << "x64(AMD or Intel)" << std::endl;
			break;
		case 12: std::cout << "ARM64" << std::endl;
			break;
		default: std::cout << "Unknown architecture" << std::endl;
			break;
	}
	std::cout << "  Processor type: ";
	switch (si.dwProcessorType)
	{
	case 386: std::cout << "Intel 386" << std::endl;
		break;
	case 486: std::cout << "Intel 486" << std::endl;
		break;
	case 586: std::cout << "Intel Pentium" << std::endl;
		break;
	case 2200: std::cout << "Intel IA64" << std::endl;
		break;
	case 8664: std::cout << "AMD X8664" << std::endl;
		break;
	default: std::cout << "ARM" << std::endl;
		break;
	}
	std::cout << "  Number of processors: " << si.dwNumberOfProcessors << std::endl;
	std::cout << "  Active processor mask: " << si.dwActiveProcessorMask << std::endl;
	// address
	std::cout << "  Page size: " << si.dwPageSize << std::endl;
	std::cout << "  Minimum application address: " << si.lpMinimumApplicationAddress << std::endl;
	std::cout << "  Maximum application address: " << si.lpMaximumApplicationAddress << std::endl;
	std::cout << "  The granularity for the starting address at which virtual memory can be allocated: " 
		<< si.dwAllocationGranularity << std::endl;
	std::cout << std::endl;
}

bool ThrowOutOfMemoryError()
{
	MEMORYSTATUS ms;
	GlobalMemoryStatus(&ms);
	HLOCAL pAllocMem = LocalAlloc(LMEM_FIXED, ms.dwTotalVirtual);
	if (pAllocMem == NULL)
		return true;
	else
	{
		LocalFree(pAllocMem);
		return false;
	}
}

void ShowError()
{
	LPVOID lpMsgBuf;
	DWORD dw = GetLastError();
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, dw,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPSTR)&lpMsgBuf, 0, NULL);
	printf("\n%s\n", lpMsgBuf);
	LocalFree(lpMsgBuf);
	return;
}