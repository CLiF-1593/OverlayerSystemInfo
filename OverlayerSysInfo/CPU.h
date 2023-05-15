#pragma once

#ifdef OVERLAYERSYSINFO_EXPORTS
#define CPU_DECLSPEC __declspec(dllexport)
#else
#define CPU_DECLSPEC __declspec(dllimport)
#endif

extern "C" {
	CPU_DECLSPEC const char* OSI_CPU_Name();				//Get Cpu Name
	CPU_DECLSPEC const int OSI_CPU_CoreNumber();			//Get the Number of Cpu Core
	CPU_DECLSPEC const int OSI_CPU_LogicalProcessorNumber();//Get the Number of Cpu Logical Processor
	CPU_DECLSPEC const double OSI_CPU_TotalUsage();			//Get Total Cpu Usage (%)
	CPU_DECLSPEC const double OSI_CPU_AdofaiUsage();		//Get Cpu Usage of ADOFAI (%)
	//CPU_DECLSPEC const double OSI_CPU_ClockSpeed();			//Get Cpu Current Clock Speed (Mhz)
	CPU_DECLSPEC const double OSI_CPU_BaseClockSpeed();		//Get Cpu Theoretical Base Clock Speed (Mhz)
	CPU_DECLSPEC const double OSI_CPU_MaxClockSpeed();		//Get Cpu Theoretical Maximum Clock Speed (Mhz)
}