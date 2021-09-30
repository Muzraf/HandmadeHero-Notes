#include<windows.h>
#include<tchar.h>
#include<stdint.h>
// #include<Xinput.h>
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
};

struct win32_window_dimension
{
    int Width;
    int Height;
};

win32_window_dimension
Win32GetWindowDimension(HWND Window)
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
    int BytesPerPixel=4;

    Buffer->Info.bmiHeader.biSize= sizeof(Buffer->Info.bmiHeader);
    Buffer->Info.bmiHeader.biWidth=Buffer->Width;
    Buffer->Info.bmiHeader.biHeight=-Buffer->Height;
    Buffer->Info.bmiHeader.biPlanes=1;
    Buffer->Info.bmiHeader.biBitCount = 32;
    Buffer->Info.bmiHeader.biCompression= BI_RGB;

    int BitmapMemorySize = (Buffer->Width*Buffer->Height)*BytesPerPixel;
    Buffer->Memory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    Buffer->Pitch = Width*BytesPerPixel; 
}
 

internal void Win32DisplayBufferInWindow( HDC DeviceContext,int WindowWidth, int WindowHeight,
                                        win32_offscreen_buffer Buffer
                                        )
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
   
    case WM_SYSKEYDOWN:
    case WM_SYSKEYUP:
    case WM_KEYDOWN:
    case WM_KEYUP:
    {
        uint32 VKCode = WParam;
        bool WasDown = ((LParam & (1 << 30))!=0);
        bool IsDown = ((LParam & (1 << 31)) ==0);
        if (IsDown != WasDown){
        if(VKCode == 'W')
        {
            
        }
        else if(VKCode == 'A')
        {

        }else if(VKCode == 'S')
        {
            
        }else if(VKCode == 'D')
        {
            
        }else if(VKCode == VK_UP)
        {
            
        }else if(VKCode == VK_DOWN)
        {
            
        }else if(VKCode == VK_RIGHT)
        {
            
        }else if(VKCode == VK_LEFT)
        {
            
        }else if(VKCode == VK_SPACE)
        {
            
        }else if(VKCode ==VK_ESCAPE)
        {
            OutputDebugStringA("Escape Key: ");
            if(IsDown)
            {
                OutputDebugStringA("IsDown");
            }
            if(WasDown)
            {
                OutputDebugStringA("WasDown");
            }
            OutputDebugStringA("\n");
        }
    }}
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
        
        win32_window_dimension Dimension = Win32GetWindowDimension(Window);
        Win32DisplayBufferInWindow(DeviceContext, Dimension.Width, Dimension.Height , GlobalBackbuffer);  
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
            
            HDC DeviceContext = GetDC(Window);
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
                // XInput                
                // for(DWORD ControllerIndex =0 ; 
                //     ControllerIndex<XUSER_MAX_COUNT;
                //     ++ControllerIndex
                //     )
                // {
                //     XINPUT_STATE ControllerState;
                //     if (XInputGetState(ControllerIndex, &ControllerState)==ERROR_SUCCESS)
                //     {
                //         XINPUT_GAMEPAD *Pad = &ControllerState.Gamepad;
                //         bool GPadUp = (Pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                //         int16 StickX = Pad->sThumbLX;
                //     }else
                //     {
                //         //TODO
                //     }
                // }
                
                RenderWeirdGradient(GlobalBackbuffer, XOffset, YOffset);
                
                
                win32_window_dimension Dimension = Win32GetWindowDimension(Window);
                Win32DisplayBufferInWindow(DeviceContext, Dimension.Width,Dimension.Height,GlobalBackbuffer);
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