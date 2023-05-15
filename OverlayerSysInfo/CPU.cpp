#include "pch.h"
#include "CPU.h"
#include <intrin.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <Pdh.h>
using namespace std;

#define ADOFAI_CAPTION L"A Dance of Fire and Ice"

class CPU_Info {
public:
	int logical_processor_cnt = 0;
	int numa_node_cnt = 0;
	int processor_core_cnt = 0;
	int processor_L1_cache_cnt = 0;
	int processor_L2_cache_cnt = 0;
	int processor_L3_cache_cnt = 0;
	int processor_package_cnt = 0;
};

DWORD CountSetBits(ULONG_PTR bitMask)
{
	DWORD LSHIFT = sizeof(ULONG_PTR) * 8 - 1;
	DWORD bitSetCount = 0;
	ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;
	DWORD i;

	for (i = 0; i <= LSHIFT; ++i) {
		bitSetCount += ((bitMask & bitTest) ? 1 : 0);
		bitTest /= 2;
	}

	return bitSetCount;
}

int GetProcessorInfo(CPU_Info& info) {
	typedef BOOL(WINAPI* LPFN_GLPI)(PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
	LPFN_GLPI glpi;

	DWORD returnLength = 0;
	info.logical_processor_cnt = 0;
	info.numa_node_cnt = 0;
	info.processor_core_cnt = 0;
	info.processor_L1_cache_cnt = 0;
	info.processor_L2_cache_cnt = 0;
	info.processor_L3_cache_cnt = 0;
	info.processor_package_cnt = 0;
	DWORD byteOffset = 0;
	PCACHE_DESCRIPTOR Cache;

	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer = NULL;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ptr = NULL;

	glpi = (LPFN_GLPI)GetProcAddress(GetModuleHandle(L"kernel32"), "GetLogicalProcessorInformation");
	if (NULL == glpi) {
		return 1;
	}

	DWORD rc = glpi(buffer, &returnLength);
	if (FALSE == rc) {
		if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
			if (buffer)
				free(buffer);

			buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(returnLength);
		}
		rc = glpi(buffer, &returnLength);
	}
	if (FALSE == rc) {
		return 1;
	}

	ptr = buffer;
	while (byteOffset < returnLength) {
		switch (ptr->Relationship)
		{
		case RelationNumaNode:
			info.numa_node_cnt++;
			break;

		case RelationProcessorCore:
			info.processor_core_cnt++;
			info.logical_processor_cnt += CountSetBits(ptr->ProcessorMask);
			break;

		case RelationCache:
			Cache = &ptr->Cache;
			if (Cache->Level == 1) {
				info.processor_L1_cache_cnt++;
			}
			else if (Cache->Level == 2) {
				info.processor_L2_cache_cnt++;
			}
			else if (Cache->Level == 3) {
				info.processor_L3_cache_cnt++;
			}
			break;

		case RelationProcessorPackage:
			info.processor_package_cnt++;
			break;
		}
		byteOffset += sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
		ptr++;
	}
	free(buffer);
	return 0;
}

class CPU {
public:
	static void LoadCpuInfo() {
		int CPUInfo[4] = { -1 };
		unsigned   nExIds, i = 0;
		char CPUBrandString[0x40];
		// Get the information associated with each extended ID.
		__cpuid(CPUInfo, 0x80000000);
		nExIds = CPUInfo[0];
		for (i = 0x80000000; i <= nExIds; ++i)
		{
			__cpuid(CPUInfo, i);
			// Interpret CPU brand string
			if (i == 0x80000002) {
				memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
			}
			else if (i == 0x80000003) {
				memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
			}
			else if (i == 0x80000004) {
				memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
			}
		}
		CPU::name = CPUBrandString;
		GetProcessorInfo(CPU::info);
		int cpuInfo[4] = { 0, 0, 0, 0 };
		__cpuid(cpuInfo, 0);
		if (cpuInfo[0] >= 0x16) {
			__cpuid(cpuInfo, 0x16);
			CPU::base_clock = cpuInfo[0];
			CPU::max_clock = cpuInfo[1];
		}

		SYSTEM_INFO sysInfo;
		FILETIME ftime, fsys, fuser;

		GetSystemInfo(&sysInfo);
		CPU::numProcessors = sysInfo.dwNumberOfProcessors;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&CPU::lastCPU, &ftime, sizeof(FILETIME));

		auto hwnd = FindWindow(NULL, ADOFAI_CAPTION);
		DWORD pid;
		GetWindowThreadProcessId(hwnd, &pid);
		CPU::self = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		GetProcessTimes(self, &ftime, &ftime, &fsys, &fuser);
		memcpy(&CPU::lastSysCPU, &fsys, sizeof(FILETIME));
		memcpy(&CPU::lastUserCPU, &fuser, sizeof(FILETIME));
		CPU::loaded = true;
	}
	static CPU_Info GetCpuInfo() { if (!loaded) CPU::LoadCpuInfo(); return CPU::info; }
	static string GetCpuName() { if (!loaded) CPU::LoadCpuInfo(); return CPU::name; }
	static double GetBaseClock() { if (!loaded) CPU::LoadCpuInfo(); return CPU::base_clock; }
	static double GetMaxClock() { if (!loaded) CPU::LoadCpuInfo(); return CPU::max_clock; }
	static double GetAdofaiUsage() {
		if (!loaded) {
			CPU::LoadCpuInfo();
			return 0;
		}

		PdhOpenQuery(NULL, NULL, &cpuQuery);
		PdhAddEnglishCounter(cpuQuery, L"\\Processor(_Total)\\% Processor Time", NULL, &cpuTotal);
		PdhCollectQueryData(cpuQuery);

		FILETIME ftime, fsys, fuser;
		ULARGE_INTEGER now, sys, user;
		double percent;

		GetSystemTimeAsFileTime(&ftime);
		memcpy(&now, &ftime, sizeof(FILETIME));

		if (CPU::self == 0) {
			auto hwnd = FindWindow(NULL, ADOFAI_CAPTION);
			DWORD pid;
			GetWindowThreadProcessId(hwnd, &pid);
			CPU::self = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
		}

		GetProcessTimes(CPU::self, &ftime, &ftime, &fsys, &fuser);
		memcpy(&sys, &fsys, sizeof(FILETIME));
		memcpy(&user, &fuser, sizeof(FILETIME));
		percent = (sys.QuadPart - CPU::lastSysCPU.QuadPart) + (user.QuadPart - CPU::lastUserCPU.QuadPart);
		percent /= (now.QuadPart - CPU::lastCPU.QuadPart);
		percent /= numProcessors;
		CPU::lastCPU = now;
		CPU::lastUserCPU = user;
		CPU::lastSysCPU = sys;

		return percent * 100.0;
	}
	static double GetTotUsage() {
		PDH_FMT_COUNTERVALUE counterVal;
		PdhCollectQueryData(CPU::cpuQuery);
		PdhGetFormattedCounterValue(CPU::cpuTotal, PDH_FMT_DOUBLE, NULL, &counterVal);
		return counterVal.doubleValue;
	}

private:
	static bool loaded;
	static CPU_Info info;
	static string name;
	static double base_clock;
	static double max_clock;

	static PDH_HQUERY cpuQuery;
	static PDH_HCOUNTER cpuTotal;
	static ULARGE_INTEGER lastCPU, lastSysCPU, lastUserCPU;
	static int numProcessors;
	static HANDLE self;
};

bool CPU::loaded = false;
CPU_Info CPU::info = CPU_Info();
string CPU::name = "";
double CPU::base_clock = 0;
double CPU::max_clock = 0;
ULARGE_INTEGER CPU::lastCPU, CPU::lastSysCPU, CPU::lastUserCPU;
int CPU::numProcessors;
HANDLE CPU::self;
PDH_HQUERY CPU::cpuQuery;
PDH_HCOUNTER CPU::cpuTotal;

const char* OSI_CPU_Name() {
	const char* str = CPU::GetCpuName().c_str();
	char* ret = new char[strlen(str)];
	strcpy(ret, str);
	return ret;
}

const int OSI_CPU_CoreNumber() {
	return CPU::GetCpuInfo().processor_core_cnt;
}

const int OSI_CPU_LogicalProcessorNumber() {
	return CPU::GetCpuInfo().logical_processor_cnt;
}

const double OSI_CPU_BaseClockSpeed() {
	return CPU::GetBaseClock();
}

const double OSI_CPU_MaxClockSpeed() {
	return CPU::GetMaxClock();
}

const double OSI_CPU_TotalUsage() {
	return CPU::GetTotUsage();
}

const double OSI_CPU_AdofaiUsage() {
	return CPU::GetAdofaiUsage();
}
