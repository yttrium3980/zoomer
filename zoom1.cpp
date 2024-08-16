/**************************
 * Includes
 *
 **************************/

#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "winuser.h"
#include <gl/gl.h>


 /**************************
  * Function Declarations
  *
  **************************/

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam);
void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

unsigned int screentexture;

int screenwidth, screenheight;

void inittexture(void)
{
    char* texture = new char[1024 * 1024 * 3];
    HDC hdc;
    hdc = GetDC(NULL);
    for (int y = 0; y < screenheight; y++)
        for (int x = 0; x < screenwidth; x++)
        {
            COLORREF c = GetPixel(hdc, x, y);
            texture[y * 1024 * 3 + x * 3] = GetRValue(c);
            texture[y * 1024 * 3 + x * 3 + 1] = GetGValue(c);
            texture[y * 1024 * 3 + x * 3 + 2] = GetBValue(c);
        }

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1, &screentexture);
    glBindTexture(GL_TEXTURE_2D, screentexture);
    glTexImage2D(GL_TEXTURE_2D, 0, 3, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, texture);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


    delete[] texture;
}

float mikescale = 1.0f;
float focalpointx = 0.0;
float focalpointy = 0.0;

float focalpointvx = 0.0;
float focalpointvy = 0.0;

/**************************
 * WinMain
 *
 **************************/

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int iCmdShow)
{
    WNDCLASS wc;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = "GLSample";
    RegisterClass(&wc);

    screenwidth = GetSystemMetrics(SM_CXSCREEN);
    screenheight = GetSystemMetrics(SM_CYSCREEN);

    if (screenwidth > 1024 || screenheight > 1024)
    {
        MessageBox(NULL, "Sorry, your screen is too big.  Width and height must be no greater than 1024 pixels.", "Screen too big", MB_OK);
        return 1;
    }


    /*char string[89];
    sprintf(string, "%d", screenheight);
    MessageBox(NULL, string, string, MB_OK);*/



    /* create main window */
    hWnd = CreateWindow(
        "GLSample", "OpenGL Sample",
        WS_POPUPWINDOW | WS_VISIBLE,
        0, 0, screenwidth, screenheight,
        NULL, NULL, hInstance, NULL);

    /* enable OpenGL for the window */
    EnableOpenGL(hWnd, &hDC, &hRC);
    ShowWindow(hWnd, SW_HIDE);
    inittexture();
    ShowWindow(hWnd, SW_SHOWNORMAL);
    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            float xmax = float(screenwidth) / 1024.0;
            float ymax = float(screenheight) / 1024.0;
            glPushMatrix();

            if (GetAsyncKeyState(VK_UP)) focalpointy += 0.1 / mikescale;
            if (GetAsyncKeyState(VK_DOWN)) focalpointy -= 0.1 / mikescale;
            if (GetAsyncKeyState(VK_RIGHT)) focalpointx += 0.1 / mikescale;
            if (GetAsyncKeyState(VK_LEFT)) focalpointx -= 0.1 / mikescale;

            focalpointx += focalpointvx;
            focalpointy += focalpointvy;

            //            focalpointvx-=0.1/mikescale;
            //            focalpointvy-=0.1/mikescale;


            if (GetAsyncKeyState(VK_LBUTTON)) {
                POINT point;
                GetCursorPos(&point);

                float gridx = float(point.x) / float(screenwidth) * 2.0 - 1.0;
                float gridy = -(float(point.y) / float(screenheight) * 2.0 - 1.0);

                focalpointx += gridx / mikescale / 12.0;
                focalpointy += gridy / mikescale / 12.0;

                mikescale += 0.1;

            }

            if (GetAsyncKeyState(VK_RBUTTON))
            {

                POINT point;
                GetCursorPos(&point);

                float gridx = float(point.x) / float(screenwidth) * 2.0 - 1.0;
                float gridy = -(float(point.y) / float(screenheight) * 2.0 - 1.0);

                focalpointx += gridx / mikescale / 12.0;
                focalpointy += gridy / mikescale / 12.0;
                if (mikescale > .8) mikescale -= 0.1;


            }

            if (focalpointx > 1.0) focalpointx = 1.0;
            else if (focalpointx < -1.0) focalpointx = -1.0;
            if (focalpointy > 1.0) focalpointy = 1.0;
            else if (focalpointy < -1.0) focalpointy = -1.0;

            //            glScalef(mikescale, mikescale, 1);

            //            glTranslatef(-focalpointx/mikescale, -focalpointy/mikescale, 0);

                        //glTranslatef(-focalpointx, -focalpointy, 0);

                        //mikescale+=0.1;
            glBegin(GL_QUADS);
            glTexCoord2f(0, ymax);
            //            glVertex2f (-1*mikescale-focalpointx, -1*mikescale-focalpointy);

            glVertex2f((-1 - focalpointx) * mikescale, (-1 - focalpointy) * mikescale);


            glTexCoord2f(xmax, ymax);
            //          glVertex2f (1*mikescale-focalpointx, -1*mikescale-focalpointy);
            glVertex2f((1 - focalpointx) * mikescale, (-1 - focalpointy) * mikescale);


            glTexCoord2f(xmax, 0);
            //        glVertex2f (1*mikescale-focalpointx, 1*mikescale-focalpointy);

            glVertex2f((1 - focalpointx) * mikescale, (1 - focalpointy) * mikescale);


            glTexCoord2f(0, 0);
            //      glVertex2f (-1*mikescale-focalpointx, 1*mikescale-focalpointy);

            glVertex2f((-1 - focalpointx) * mikescale, (1 - focalpointy) * mikescale);


            glEnd();
            glPopMatrix();

            SwapBuffers(hDC);

            Sleep(1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hWnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hWnd);

    return msg.wParam;
}


/********************
 * Window Procedure
 *
 ********************/

LRESULT CALLBACK WndProc(HWND hWnd, UINT message,
    WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
        return 0;
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;

    case WM_DESTROY:
        return 0;

    case WM_LBUTTONDOWN:
    {

        int xPos = lParam & 0x0000ffff;
        int yPos = lParam >> 16;
        char string[80];

        //         float focalpointx = float(xPos)/800.0f*2.0 - 1.0;
        //         float focalpointy = float(yPos)/600.0f*2.0 - 1.0;

        /*           float gridx = float(xPos)/799.0*2.0-1.0;
                   float gridy = -(float(yPos)/599.0*2.0-1.0);

                   focalpointx+=gridx/mikescale/2;
                   focalpointy+=gridy/mikescale/2;

                   mikescale+=0.5;*/

        sprintf_s(string, "%d %d", xPos, yPos);
        //         MessageBox(NULL, string, string, MB_OK);
    }
    return 0;

    case WM_RBUTTONDOWN:
        //mikescale-=0.5;
        return 0;

    case WM_KEYDOWN:
        switch (wParam)
        {
        case VK_ESCAPE:
            PostQuitMessage(0);
            return 0;
        }
        return 0;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
}


/*******************
 * Enable OpenGL
 *
 *******************/

void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hWnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
        PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;
    iFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);
    wglMakeCurrent(*hDC, *hRC);

}


/******************
 * Disable OpenGL
 *
 ******************/

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}
