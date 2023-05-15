#include <CPU.h>
#include <iostream>
#include <Windows.h>
using namespace std;
int main() {
	while (true) {
		cout << OSI_CPU_TotalUsage() << endl;
		Sleep(100);
	}
	return 0;
}