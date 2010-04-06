#include "Test_Main.h"

#include <iostream>
#include "UPlugin/Env.h"

using namespace std;
using namespace UPlugin;

int test_env() {
	cout << Env::GetInstance()->getInstallDirectory() << endl;
	cout << Env::GetInstance()->getAppBaseDirectory() << endl;
	cout << Env::GetInstance()->getDataDirectory() << endl;
	cout << Env::GetInstance()->getDotNetSupportDLL() << endl;

	return 0;
}