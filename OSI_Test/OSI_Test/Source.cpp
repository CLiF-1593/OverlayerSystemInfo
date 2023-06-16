#include <CPU.h>
#include <RAM.h>
#include <GPU.h>
#include <iostream>
#include <Windows.h>
using namespace std;
int main() {
	while (true) {
		cout << OSI_GPU_Number() << endl;
		cout << OSI_GPU_Name(0) << endl;
		cout << OSI_GPU_Name(1) << endl;
		Sleep(500);
	}
	return 0;
}