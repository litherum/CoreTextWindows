// CoreTextWindows.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "CoreTextWindows.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <CoreGraphics/CoreGraphics.h>
#include <CoreFoundation/CoreFoundation.h>
#include <CoreText/CoreText.h>

extern "C" {

	typedef const struct __CTFont* CTFontRef;
	typedef const struct __CTLine* CTLineRef;
	typedef const struct __CTRun* CTRunRef;
	typedef const struct __CTFontDescriptor* CTFontDescriptorRef;

	const CFStringRef kCTFontAttributeName = CFSTR("NSFont");
	const CFStringRef kCTForegroundColorAttributeName = CFSTR("CTForegroundColor");

	CTFontRef CTFontCreateWithName(CFStringRef name, CGFloat size, const CGAffineTransform* matrix);
	CTLineRef CTLineCreateWithAttributedString(CFAttributedStringRef);
	void CTLineDraw(CTLineRef, CGContextRef);
	CFArrayRef CTLineGetGlyphRuns(CTLineRef line);
	CFIndex CTRunGetGlyphCount(CTRunRef run);
	void CTRunGetGlyphs(CTRunRef run, CFRange range, CGGlyph buffer[]);
	void CTRunGetPositions(CTRunRef run, CFRange range, CGPoint buffer[]);
	CGFontRef CTFontCopyGraphicsFont(CTFontRef font, CTFontDescriptorRef* attributes);
	void CTRunDraw(CTRunRef run, CGContextRef context, CFRange range);
}

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CORETEXTWINDOWS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CORETEXTWINDOWS));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CORETEXTWINDOWS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CORETEXTWINDOWS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			RECT windowRect;
			::GetClientRect(hWnd, &windowRect);
			LONG width = windowRect.right - windowRect.left;
			LONG height = windowRect.bottom - windowRect.top;

			BITMAPINFO bitmapInfo = { 0 };
			bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
			bitmapInfo.bmiHeader.biWidth = width;
			bitmapInfo.bmiHeader.biHeight = height;
			bitmapInfo.bmiHeader.biPlanes = 1;
			bitmapInfo.bmiHeader.biBitCount = 32;
			bitmapInfo.bmiHeader.biCompression = BI_RGB;

			void* bits;
			HBITMAP bitmap = CreateDIBSection(hdc, &bitmapInfo, DIB_RGB_COLORS, &bits, 0, 0);

			HDC compatibleDC = ::CreateCompatibleDC(hdc);
			HGDIOBJ oldBitmap = ::SelectObject(compatibleDC, bitmap);

			assert(GetCurrentObject(compatibleDC, OBJ_BITMAP) == bitmap);
			BITMAP usedBitmap = { 0 };
			GetObject(bitmap, sizeof(usedBitmap), &usedBitmap);
			assert(usedBitmap.bmBits == bits);
			assert(usedBitmap.bmBitsPixel == 32);

			CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
			CGContextRef context = CGBitmapContextCreate(usedBitmap.bmBits, usedBitmap.bmWidth, usedBitmap.bmHeight, 8, usedBitmap.bmWidthBytes, colorSpace, kCGBitmapByteOrder32Little | kCGImageAlphaNoneSkipFirst);
			CGContextTranslateCTM(context, 0, usedBitmap.bmHeight);
			CGContextScaleCTM(context, 1, -1);
			SetGraphicsMode(compatibleDC, GM_ADVANCED);

			CGFloat redComponents[] = { 1, 0, 0, 1 };
			CGColorRef red = CGColorCreate(colorSpace, redComponents);
			CGContextSetFillColorWithColor(context, red);
			CGContextSetStrokeColorWithColor(context, red);

			CGContextFillRect(context, CGRectMake(100, 200, 300, 400));

			CGFloat greyComponents[] = { 0.5, 0.5, 0.5, 1 };
			CGColorRef grey = CGColorCreate(colorSpace, greyComponents);
			CGContextSetFillColorWithColor(context, grey);
			CGContextSetStrokeColorWithColor(context, grey);

			CTFontRef font = CTFontCreateWithName(CFSTR("Arial"), 100, nullptr);
			CFTypeRef keys[] = { kCTFontAttributeName, kCTForegroundColorAttributeName };
			CFTypeRef values[] = { font, grey };
			CFDictionaryRef attributes = CFDictionaryCreate(kCFAllocatorDefault, keys, values, sizeof(keys) / sizeof(keys[0]), &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
			CFAttributedStringRef attributedString = CFAttributedStringCreate(kCFAllocatorDefault, CFSTR("Hello, World!"), attributes);
			CTLineRef line = CTLineCreateWithAttributedString(attributedString);
			CGFontRef cgFont = CTFontCopyGraphicsFont(font, nullptr);

			CGContextSetFont(context, cgFont);
			CGContextSetFontSize(context, 100);
			CGContextSetTextMatrix(context, CGAffineTransformIdentity);
			CGContextSetTextPosition(context, 100, 100);

			CTLineDraw(line, context);

			CFArrayRef runs = CTLineGetGlyphRuns(line);
			CFIndex runCount = CFArrayGetCount(runs);
			for (CFIndex i = 0; i < runCount; ++i) {
				CTRunRef run = static_cast<CTRunRef>(CFArrayGetValueAtIndex(runs, i));
				CFIndex glyphCount = CTRunGetGlyphCount(run);
				std::cout << "Run " << i << " has " << glyphCount << " glyphs." << std::endl;
				std::vector<CGGlyph> glyphs(glyphCount);
				CTRunGetGlyphs(run, CFRangeMake(0, 0), glyphs.data());
				std::vector<CGPoint> positions(glyphCount);
				CTRunGetPositions(run, CFRangeMake(0, 0), positions.data());
				for (CFIndex j = 0; j < glyphCount; ++j) {
					std::cout << "Glyph index " << j << ": " << glyphs[j] << " at position (" << positions[j].x << ", " << positions[j].y << ")" << std::endl;
				}
				//CGContextShowGlyphsAtPositions(context, glyphs.data(), positions.data(), glyphCount);
				//CTRunDraw(run, context, CFRangeMake(0, 0));
			}

			CGFontRelease(cgFont);
			CFRelease(line);
			CFRelease(attributedString);
			CFRelease(attributes);
			CFRelease(font);

			CGColorRelease(red);
			CGColorRelease(grey);

			CGContextRelease(context);
			CGColorSpaceRelease(colorSpace);

			GdiFlush();

			assert(windowRect.left == 0 && windowRect.top == 0);
			BitBlt(hdc, windowRect.left, windowRect.top, width, height, compatibleDC, windowRect.left, windowRect.top, SRCCOPY);

			::SelectObject(compatibleDC, oldBitmap);
			::DeleteDC(compatibleDC);
			::DeleteObject(bitmap);

            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
