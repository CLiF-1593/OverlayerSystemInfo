#include "pch.h"
#include "RAM.h"

#define GB(x) ((double)(x) / 1024.0 / 1024.0 / 1024.0)

class RAM {
public:
	static void LoadRamInfo();
	static double PhyTotSize();
	static double PhyTotUsage();
	static double PhyAdofaiUsage();
	static double VrtTotSize();
	static double VrtTotUsage();
	static double VrtAdofaiUsage();

private:
	static MEMORYSTATUSEX memInfo;
	static HANDLE handle;
	static bool loaded;
};

MEMORYSTATUSEX RAM::memInfo;
HANDLE RAM::handle;
bool RAM::loaded = false;

const double OSI_RAM_PhysicalMemory_TotalSize() {
    return RAM::PhyTotSize();
}

const double OSI_RAM_PhysicalMemory_TotalUsage() {
	return RAM::PhyAdofaiUsage();
}

const double OSI_RAM_PhysicalMemory_TotalUsagePercent() {
	return RAM::PhyTotUsage() / RAM::PhyTotSize() * 100.0;
}

const double OSI_RAM_PhysicalMemory_AdofaiUsage() {
	return RAM::PhyAdofaiUsage();
}

const double OSI_RAM_PhysicalMemory_AdofaiUsagePercent() {
	return RAM::PhyAdofaiUsage() / RAM::PhyTotSize() * 100.0;
}

const double OSI_RAM_AvailableMemory_TotalSize() {
	return RAM::VrtTotSize();
}

const double OSI_RAM_AvailableMemory_TotalUsage() {
	return RAM::VrtTotUsage();
}

const double OSI_RAM_AvailableMemory_TotalUsagePercent() {
	return RAM::VrtTotUsage() / RAM::VrtTotSize() * 100.0;
}

const double OSI_RAM_AvailableMemory_AdofaiUsage() {
	return RAM::VrtAdofaiUsage();
}

const double OSI_RAM_AvailableMemory_AdofaiUsagePercent() {
	return RAM::VrtAdofaiUsage() / RAM::VrtTotSize() * 100.0;
}

void RAM::LoadRamInfo() {
	RAM::memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&RAM::memInfo);

	auto hwnd = FindWindow(NULL, ADOFAI_CAPTION);
	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (handle) RAM::loaded = true;
}

double RAM::PhyTotSize() {
	if (!RAM::loaded) RAM::LoadRamInfo();
	return GB(RAM::memInfo.ullTotalPhys);
}

double RAM::PhyTotUsage() {
	if (!RAM::loaded) RAM::LoadRamInfo();
	return GB(memInfo.ullTotalPhys - memInfo.ullAvailPhys);
}

double RAM::PhyAdofaiUsage() {
	if (!RAM::loaded) {
		RAM::LoadRamInfo();
		return 0;
	}
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(RAM::handle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return GB(pmc.WorkingSetSize);
}

double RAM::VrtTotSize() {
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return GB(memInfo.ullTotalPageFile);
}

double RAM::VrtTotUsage() {
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);
	return GB(memInfo.ullTotalPageFile - memInfo.ullAvailPageFile);
}

double RAM::VrtAdofaiUsage() {
	if (!RAM::loaded) {
		RAM::LoadRamInfo();
		return 0;
	}
	PROCESS_MEMORY_COUNTERS_EX pmc;
	GetProcessMemoryInfo(RAM::handle, (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
	return GB(pmc.PrivateUsage);
}