#include<windows.h>

int WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    int nCmdShow)
{
    MessageBoxA(0, "This is Handmade hero", "Handmade Hero", MB_OK|MB_ICONINFORMATION);
    return(0);
}