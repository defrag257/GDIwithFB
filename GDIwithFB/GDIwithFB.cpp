// GDIwithFB.cpp - ʹ��GDI/GDI+��ʹ��֡������ж�̬����

#include <windows.h>
#include <GdiPlus.h>
#pragma comment(lib, "gdiplus.lib")
#include <math.h>

HINSTANCE hInst;
HWND hMainWnd;

static float locx = 320, locy = 240;

// ���ƺ�����ʹ��framedc�������л�����ƣ�ʹ��hWnd��devdc������ȡ���ں�Ŀ���豸����Ϣ
static void FrameDraw(HWND hWnd, HDC devdc, HDC framedc)
{
	using namespace Gdiplus;

	Graphics g(framedc);
	g.Clear(Color::White);

	Pen blackpen(Color::Black, 1);

	float t = (GetTickCount() % 1000) / 1000.0f;
	float pi = acosf(-1);
	float x = sinf(t * pi * 2) * 100 + locx;
	float y = cosf(t * pi * 2) * 100 + locy;

	g.DrawLine(&blackpen, PointF(locx, locy), PointF(x, y));
	g.DrawEllipse(&blackpen, RectF(locx - 100, locy - 100, 200, 200));
}

// ʵ�ֻ�����Ʊ�׼���̵ĺ���
static void CallFrameDraw(HWND hWnd, HDC devdc)
{
	RECT rc = { 0 };
	GetClientRect(hWnd, &rc);
	HDC framedc = CreateCompatibleDC(devdc);
	HBITMAP framebmp = CreateCompatibleBitmap(devdc, rc.right - rc.left, rc.bottom - rc.top);
	SelectObject(framedc, framebmp);
	FrameDraw(hWnd, devdc, framedc);
	BitBlt(devdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, framedc, 0, 0, SRCCOPY);
	DeleteDC(framedc);
	DeleteObject(framebmp);
}

// ������Ϣ�����������ﴦ������Ϣ
static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_CREATE) // ���ڴ���
	{
		return 0;
	}
	if (msg == WM_KEYDOWN) // ���¼�
	{
		if (wParam == VK_LEFT)
		{
			locx -= 5.0f;
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}
		if (wParam == VK_RIGHT)
		{
			locx += 5.0f;
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}
		if (wParam == VK_UP)
		{
			locy -= 5.0f;
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}
		if (wParam == VK_DOWN)
		{
			locy += 5.0f;
			InvalidateRect(hWnd, NULL, FALSE);
			return 0;
		}
	}
	if (msg == WM_PAINT) // ������Ҫ�ػ�
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		CallFrameDraw(hWnd, hdc);
		EndPaint(hWnd, &ps);
		return 0;
	}
	if (msg == WM_DESTROY) // ���ڱ�����
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// ������ڵ�
int WINAPI WinMain(__in HINSTANCE hInstance, __in_opt HINSTANCE hPrevInstance, __in_opt LPSTR lpCmdLine, __in int nShowCmd)
{
	hInst = hInstance;

#ifdef _GDIPLUS_H
	ULONG_PTR gdip_token = 0;
	Gdiplus::Status gdip_status = Gdiplus::GdiplusStartup(&gdip_token, &Gdiplus::GdiplusStartupInput(), NULL);
	if (gdip_status != Gdiplus::Ok)
	{
		MessageBox(NULL, TEXT("GDI+ initialization failed."), TEXT("Error"), MB_ICONHAND);
		return 0;
	}
#endif

	WNDCLASSEX wcex = {
		sizeof wcex,
		CS_VREDRAW | CS_HREDRAW,
		WndProc,
		0, 0,
		hInstance,
		LoadIcon(NULL, IDI_APPLICATION),
		LoadCursor(NULL, IDC_ARROW),
		NULL, // ���ǽ��Լ���մ��ڣ���˲���Ҫ������ˢ
		NULL,
		TEXT("MainWndClass"),
		LoadIcon(NULL, IDI_APPLICATION),
	};

	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL, TEXT("This program cannot be run in Windows 95/98/Me."), TEXT("Error"), MB_ICONHAND);
		return 0;
	}

	RECT rcWindow = { 0, 0, 640, 480 };
	AdjustWindowRectEx(&rcWindow, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_OVERLAPPEDWINDOW); // ���ͻ�����Сת��Ϊ���ڴ�С
	hMainWnd = CreateWindowEx(
		WS_EX_OVERLAPPEDWINDOW, TEXT("MainWndClass"), TEXT("Main Window"), WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, rcWindow.right - rcWindow.left, rcWindow.bottom - rcWindow.top,
		NULL, NULL, hInstance, NULL);

	if (hMainWnd == NULL)
	{
		MessageBox(NULL, TEXT("CreateWindowEx failed."), TEXT("Error"), MB_ICONHAND);
		return 0;
	}

	ShowWindow(hMainWnd, nShowCmd);
	UpdateWindow(hMainWnd);

	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else // û�л�ȡ����Ϣ
		{
			// ʵʱ����
			HDC hdc = GetDC(hMainWnd);
			CallFrameDraw(hMainWnd, hdc);
			ReleaseDC(hMainWnd, hdc);
		}
	}

#ifdef _GDIPLUS_H
	Gdiplus::GdiplusShutdown(gdip_token);
#endif

	return (int)msg.wParam;
}