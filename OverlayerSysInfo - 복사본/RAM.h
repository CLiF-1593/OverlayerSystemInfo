#pragma once

#ifdef OVERLAYERSYSINFO_EXPORTS
#define RAM_DECLSPEC __declspec(dllexport)
#else
#define RAM_DECLSPEC __declspec(dllimport)
#endif

extern "C" {
	RAM_DECLSPEC const double OSI_RAM_PhysicalMemory_TotalSize();			//Get Physical RAM Total Size (GB)
	RAM_DECLSPEC const double OSI_RAM_PhysicalMemory_TotalUsage();			//Get Total Physical RAM Usage (GB)
	RAM_DECLSPEC const double OSI_RAM_PhysicalMemory_TotalUsagePercent();	//Get Total Physical RAM Usage Per Total Physical RAM Size (%)
	RAM_DECLSPEC const double OSI_RAM_PhysicalMemory_AdofaiUsage();			//Get Physical RAM Usage of ADOFAI (GB)
	RAM_DECLSPEC const double OSI_RAM_PhysicalMemory_AdofaiUsagePercent();	//Get Physical RAM Usage of ADOFAI Per Total Physical RAM Size (%)
	RAM_DECLSPEC const double OSI_RAM_AvailableMemory_TotalSize();			//Get Available RAM Size (GB) (Available RAM : Physical + Virtual) 
	RAM_DECLSPEC const double OSI_RAM_AvailableMemory_TotalUsage();			//Get Total Available RAM Usage (GB) (Available RAM : Physical + Virtual)
	RAM_DECLSPEC const double OSI_RAM_AvailableMemory_TotalUsagePercent();	//Get Total Available RAM Usage Per Total Available RAM Size (%) (Available RAM : Physical + Virtual)
	RAM_DECLSPEC const double OSI_RAM_AvailableMemory_AdofaiUsage();		//Get Available RAM Usage of ADOFAI (GB) (Available RAM : Physical + Virtual)
	RAM_DECLSPEC const double OSI_RAM_AvailableMemory_AdofaiUsagePercent();	//Get Available RAM Usage of ADOFAI Per Total Available RAM Size (%) (Available RAM : Physical + Virtual)
}