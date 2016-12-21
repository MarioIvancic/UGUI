#define STRICT 1
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include "ugui.h"
#include "simulation.h"

short display_size_x = 800;
short display_size_y = 480;

void win32gdi_pset(UG_S16 x, UG_S16 y, UG_COLOR c);

short touch_x;
short touch_y;
short R, G, B, disp_x, disp_y, display;
HWND hwnd;               /* This is the handle for our window */

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

/*  Make the class name into a global variable  */
char class_name[ ] = "uGUI-Sim";

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nCmdShow)
{
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl = { 0 };/* Data structure for the windowclass */

    /* The Window structure */
    wincl.hInstance = hThisInstance;
    wincl.lpszClassName = class_name;
    wincl.lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl.style = CS_HREDRAW | CS_VREDRAW; /* CS_DBLCLKS Catch double-clicks */
    wincl.cbSize = sizeof (wincl);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl.lpszMenuName = NULL;                 /* No menu */
    wincl.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    //wincl.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
    wincl.hbrBackground  = (HBRUSH) GetStockObject( BLACK_BRUSH );

    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl)) return 0;

    RECT apprect;
    unsigned dwStyle = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    unsigned dwExStyle = 0;
    HDC hDC = GetDC(NULL);
    int w = GetDeviceCaps(hDC, HORZRES);
	int h = GetDeviceCaps(hDC, VERTRES);
	ReleaseDC(NULL, hDC);
    apprect.left = (w >> 1) - (display_size_x >> 1);
	apprect.top = (h >> 1) - (display_size_y >> 1);
	apprect.right = apprect.left + display_size_x;
	apprect.bottom = apprect.top + display_size_y;
	AdjustWindowRectEx(&apprect, dwStyle, 0, dwExStyle);

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           dwExStyle,           /* Extended possibilites for variation */
           class_name,          /* Classname */
           class_name,          /* Title Text */
           dwStyle,
           apprect.left,        /* Windows decides the position */
           apprect.top,         /* where the window ends up on the screen */
           apprect.right - apprect.left, // display_size_x,      /* The programs width */
           apprect.bottom - apprect.top, // display_size_y,      /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    if(hwnd == 0) return 0;

    /* Make the window visible on the screen */
    ShowWindow (hwnd, nCmdShow);
    UpdateWindow( hwnd );

    // Setup uGUI
    GUI_Setup(&win32gdi_pset, display_size_x, display_size_y);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
        GUI_Process();
    }

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return messages.wParam;
}



/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)                  /* handle the messages */
    {
        case WM_DESTROY:
            PostQuitMessage (0);       /* send a WM_QUIT to the message queue */
            break;
        //case WM_NCHITTEST:
         //   return HTCAPTION;

        case WM_PAINT:
            /*
            PAINTSTRUCT ps = { 0 };
            HDC hDC = BeginPaint( hwnd, &ps );
            RECT rc = { 0 };
            GetClientRect( hwnd, &rc );
            HBRUSH hbrOld = (HBRUSH) SelectObject( hDC, GetStockObject( WHITE_BRUSH ) );
            Ellipse( hDC, rc.left, rc.top, rc.right, rc.bottom );
            SelectObject( hDC, hbrOld );
            EndPaint( hwnd, &ps );
            */
//GetClientRect( hwnd, &apprect );
//printf("RECT: L=%d, R=%d, T=%d, B=%d\n", (int)apprect.left, (int)apprect.right, (int)apprect.top, (int)apprect.bottom);

            if(display)
            {
                HDC hdc = GetDC(hwnd);
                SetPixel(hdc, disp_x, disp_y, RGB(R, G, B));
                ReleaseDC(hwnd, hdc);
                display = 0;
                return 0;
            }
            return DefWindowProc (hwnd, message, wParam, lParam);


        case WM_LBUTTONDBLCLK:
            // desiæe se 4 poruke: WM_LBUTTONDOWN, WM_LBUTTONUP, WM_LBUTTONDBLCLK, and WM_LBUTTONUP.
            // touch_x = GET_X_LPARAM(lParam);
            // touch_y = GET_Y_LPARAM(lParam);
            touch_x = lParam & 0x0ffff;
            touch_y = (lParam >> 16) & 0x0ffff;
            return 0;

        case WM_LBUTTONDOWN:
            touch_x = lParam & 0x0ffff;
            touch_y = (lParam >> 16) & 0x0ffff;
            UG_TouchUpdate(touch_x, touch_y, TOUCH_STATE_PRESSED);
//printf("Touch @ %d, %d, TOUCH_STATE_PRESSED\n", touch_x, touch_y);
            return 0;

        case WM_LBUTTONUP:
            touch_x = lParam & 0x0ffff;
            touch_y = (lParam >> 16) & 0x0ffff;
            UG_TouchUpdate(-1, -1, TOUCH_STATE_RELEASED);
//printf("Touch @ %d, %d, TOUCH_STATE_RELEASED\n", touch_x, touch_y);
            return 0;

        case WM_MOUSEMOVE:
            if(wParam & MK_LBUTTON)
            {
                touch_x = lParam & 0x0ffff;
                touch_y = (lParam >> 16) & 0x0ffff;
                UG_TouchUpdate(touch_x, touch_y, TOUCH_STATE_PRESSED);
                //UG_TouchUpdate(touch_x, touch_y, TOUCH_STATE_PRESSED);
//printf("Touch @ %d, %d, TOUCH_STATE_MOVED\n", touch_x, touch_y);
                return 0;
            }
            return DefWindowProc (hwnd, message, wParam, lParam);

        default:                      /* for messages that we don't deal with */
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}


void win32gdi_pset(UG_S16 x, UG_S16 y, UG_COLOR c)
{
    disp_x = x;
    disp_y = y;

#ifdef USE_COLOR_RGB888
    // ARGB8888
    B = c & 0xff;
    G = (c >> 8) & 0xff;
    R = (c >> 16) & 0xff;
#endif // USE_COLOR_RGB888

#ifdef USE_COLOR_RGB565
    // // ARGB0565
    B = c & 0x1f;
    G = (c >> 5) & 0x3f;
    R = (c >> 9) & 0x1f;
#endif // USE_COLOR_RGB565
    display = 1;
    //InvalidateRect(hwnd, 0, 0);

    HDC hdc = GetDC(hwnd);
    SetPixel(hdc, disp_x, disp_y, RGB(R, G, B));
    ReleaseDC(hwnd, hdc);
    display = 0;
}



static const char* message_type[3] = {"NONE", "WINDOW", "OBJECT"};
static const char* event_type[4] = {"NONE", "TOUCH", "PRERENDER", "POSTRENDER"};

void decode_msg(UG_MESSAGE* msg)
{
    printf("%s %s for ID %d (SubId %d)\n",
        message_type[msg->type], event_type[msg->event], msg->id, msg->sub_id);
}
