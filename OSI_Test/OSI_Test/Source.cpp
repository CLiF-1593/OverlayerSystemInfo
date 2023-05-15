#include <CPU.h>
#include <RAM.h>
#include <iostream>
#include <Windows.h>
using namespace std;
int main() {
	while(true) {
		cout << OSI_RAM_AvailableMemory_TotalUsagePercent() << endl;
		Sleep(500);
	}
	return 0;
}