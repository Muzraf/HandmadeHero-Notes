#include<windows.h>
#include<tchar.h>
#include<stdint.h>

#define internal static
#define local_persist static
#define global_variable static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

typedef int8_t int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

struct win32_offscreen_buffer
{
    BITMAPINFO Info;
    void *Memory;
    int Height;
    int Width;
    int Pitch;
    int BytesPerPixel ;

};

struct win32_window_dimension
{
    int Width;
    int Height;
};

win32_window_dimension
GetWindowDimension(HWND Window)
{
    win32_window_dimension Result;

    RECT ClientRect;
    GetClientRect(Window, &ClientRect);
    Result.Width = ClientRect.right - ClientRect.left;
    Result.Height = ClientRect.bottom - ClientRect.top;
    return(Result);
}
global_variable bool GlobalRunning;
global_variable win32_offscreen_buffer GlobalBackbuffer;

internal void RenderWeirdGradient(win32_offscreen_buffer Buffer, int BlueOffset, int GreenOffset)
{

    uint8 *Row = (uint8 *)Buffer.Memory;
    for(int Y=0; Y<Buffer.Height; ++Y)
    {
        uint32 *Pixel =(uint32 *)Row;

        for(int X=0; X<Buffer.Width; ++X)
        {
            // *Pixel =(uint8)(X+BlueOffse t);
            // ++Pixel;

            // *Pixel = (uint8)(Y+GreenOffset);
            // ++Pixel;

            // *Pixel =0;
            // ++Pixel;

            // *Pixel =0;
            // ++Pixel;
            uint8 Blue = (X+ BlueOffset);
            uint8 Green = (Y + GreenOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }
        Row += Buffer.Pitch;
    }
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *Buffer, int Width, int Height)
{
    if(Buffer->Memory){
        VirtualFree(Buffer->Memory, 0, MEM_RELEASE);
    }
     
    Buffer->Width = Width;
    Buffer->Height = Height;
    Buffer->BytesPerPixel=4;

    Buffer->Info.bmiHeader.biSize= sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth=Buffer->Width;
    Buffer->Info.bmiHeader.biHeight=-Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes=1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression= BI_RGB;

    int BitmapMemorySize = (Buffer->Width*Buffer->Height)*Buffer->BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Width*Buffer->BytesPerPixel; 
}
 

internal void Win32DisplayBufferInWindow( HDC DeviceContext,int WindowWidth, int WindowHeight,
                                        win32_offscreen_buffer Buffer,
                                        int X, int Y, int Width, int Height)
{

    StretchDIBits(DeviceContext, 
                  /*X, Y, Width, Height, // destination rectangle (window)
                  X, Y, Width, Height, // source rectangle (bitmap buffer)
                  */
                  
                  0, 0, WindowWidth , WindowHeight,
                  0, 0, Buffer.Width, Buffer.Height,
                  Buffer.Memory,
                  &Buffer.Info, // TODO!
                  DIB_RGB_COLORS, SRCCOPY);  
}

LRESULT CALLBACK Win32MainWindowCallback(HWND Window, UINT Message, WPARAM WParam, LPARAM LParam)
{
    LRESULT Result = 0;
    switch (Message)
    {
    case WM_SIZE:
    {
       }break;
    case WM_DESTROY:
    {
        GlobalRunning=false;
    }break;
    case WM_CLOSE:
    {
        GlobalRunning=false;
    }break;
    case WM_ACTIVATEAPP:
    {
        
    }break;
    case WM_PAINT:
    {
        PAINTSTRUCT Paint;
        HDC DeviceContext = BeginPaint(Window, &Paint);
        int X = Paint.rcPaint.left;
        int Y = Paint.rcPaint.top;
        int Width = Paint.rcPaint.right - Paint.rcPaint.left;
        int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
        
        RECT ClientRect;
        GetClientRect(Window,&ClientRect);
        win32_window_dimension Dimension = GetWindowDimension(Window);
        Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height , GlobalBackbuffer,X, Y, Width, Height);  
        EndPaint(Window, &Paint);
    }break;
    default:
    {
        Result = DefWindowProc(Window,Message,WParam,LParam);
    }break;
    }
    return(Result);
}

int WinMain(HINSTANCE Instance,HINSTANCE PrevInstance,LPSTR CommandLine,int ShowCode)
{
    WNDCLASS WindowClass={};
   
    Win32ResizeDIBSection(&GlobalBackbuffer, 1280, 720);
    
    WindowClass.style=CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc = Win32MainWindowCallback;
    WindowClass.hInstance= Instance;
    WindowClass.lpszClassName=_T("HandmadeHeroWindowClass");
    if (RegisterClass(&WindowClass))
    {
        HWND Window = CreateWindowEx(0,WindowClass.lpszClassName, _T("Handmade Hero"), WS_OVERLAPPEDWINDOW|WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,0,0,Instance,0);
        if(Window){
            int XOffset=0;
            int YOffset=0;
            GlobalRunning=true;
            
            while(GlobalRunning)
            {   
                
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                    {
                        GlobalRunning = false;
                    } 
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }
                RenderWeirdGradient(GlobalBackbuffer, XOffset, YOffset);
                
                HDC DeviceContext= GetDC(Window);
                
                win32_window_dimension Dimension = GetWindowDimension(Window);
                Win32DisplayBufferInWindow(DeviceContext, Dimension.Width,Dimension.Height,GlobalBackbuffer, 0, 0, Dimension.Width, Dimension.Height);
                ReleaseDC(Window,DeviceContext);
                ++XOffset;
                YOffset += 2;
            }
        }
        else
        {

        }
    }
    else
    {

    }

    return(0);
}