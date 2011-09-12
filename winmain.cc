#ifdef WIN32
#include <windows.h>

int main(int argc, char** argv);

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	int argc = 0;
	LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &argc);
	return main(argc, (char **)argv);
}
#endif
