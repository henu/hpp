// This file may be included only once! It offers multiplatform
// main()-function. You have to implement hppMain() and this file
// ensures it will be called in Unix, and in Windows environment.
#ifdef WIN32
#include <windows.h>
#endif

int hppMain(int argc, char** argv);

int main(int argc, char** argv)
{
	return hppMain(argc, argv);
}

#ifdef WIN32
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	int argc = 0;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	return hppMain(argc, (char **)argv);
}
#endif
