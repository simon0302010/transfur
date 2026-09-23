/* nuklear - 1.32.0 - public domain */
#include <stdlib.h>
#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#include <d3d9.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_IMPLEMENTATION
#define NK_D3D9_IMPLEMENTATION
#include "../nuklear.h"
#include "nuklear_d3d9.h"

static IDirect3DDevice9 *device;
static IDirect3DDevice9Ex *deviceEx;
static D3DPRESENT_PARAMETERS present;

static LRESULT CALLBACK WindowProc(HWND wnd, UINT msg, WPARAM wparam,
                                   LPARAM lparam) {
        switch (msg) {
        case WM_DESTROY:
                PostQuitMessage(0);
                return 0;

        case WM_SIZE:
                if (device) {
                        UINT width = LOWORD(lparam);
                        UINT height = HIWORD(lparam);
                        if (width != 0 && height != 0 &&
                            (width != present.BackBufferWidth ||
                             height != present.BackBufferHeight)) {
                                HRESULT hr;
                                nk_d3d9_release();
                                present.BackBufferWidth = width;
                                present.BackBufferHeight = height;
                                hr = IDirect3DDevice9_Reset(device, &present);
                                NK_ASSERT(SUCCEEDED(hr));
                                nk_d3d9_resize(width, height);
                        }
                }
                break;
        }

        if (nk_d3d9_handle_event(wnd, msg, wparam, lparam))
                return 0;

        return DefWindowProcW(wnd, msg, wparam, lparam);
}

static void create_d3d9_device(HWND wnd) {
        HRESULT hr;

        present.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
        present.BackBufferWidth = WINDOW_WIDTH;
        present.BackBufferHeight = WINDOW_HEIGHT;
        present.BackBufferFormat = D3DFMT_X8R8G8B8;
        present.BackBufferCount = 1;
        present.MultiSampleType = D3DMULTISAMPLE_NONE;
        present.SwapEffect = D3DSWAPEFFECT_DISCARD;
        present.hDeviceWindow = wnd;
        present.EnableAutoDepthStencil = TRUE;
        present.AutoDepthStencilFormat = D3DFMT_D24S8;
        present.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
        present.Windowed = TRUE;

        { /* first try to create Direct3D9Ex device if possible (on Windows 7+)
           */
                typedef HRESULT WINAPI Direct3DCreate9ExPtr(UINT,
                                                            IDirect3D9Ex **);
                Direct3DCreate9ExPtr *Direct3DCreate9Ex =
                    (void *)GetProcAddress(GetModuleHandleA("d3d9.dll"),
                                           "Direct3DCreate9Ex");
                if (Direct3DCreate9Ex) {
                        IDirect3D9Ex *d3d9ex;
                        if (SUCCEEDED(
                                Direct3DCreate9Ex(D3D_SDK_VERSION, &d3d9ex))) {
                                hr = IDirect3D9Ex_CreateDeviceEx(
                                    d3d9ex, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
                                    wnd,
                                    D3DCREATE_HARDWARE_VERTEXPROCESSING |
                                        D3DCREATE_PUREDEVICE |
                                        D3DCREATE_FPU_PRESERVE,
                                    &present, NULL, &deviceEx);
                                if (SUCCEEDED(hr)) {
                                        device = (IDirect3DDevice9 *)deviceEx;
                                } else {
                                        /* hardware vertex processing not
                                        supported, no big deal retry with
                                        software vertex processing */
                                        hr = IDirect3D9Ex_CreateDeviceEx(
                                            d3d9ex, D3DADAPTER_DEFAULT,
                                            D3DDEVTYPE_HAL, wnd,
                                            D3DCREATE_SOFTWARE_VERTEXPROCESSING |
                                                D3DCREATE_PUREDEVICE |
                                                D3DCREATE_FPU_PRESERVE,
                                            &present, NULL, &deviceEx);
                                        if (SUCCEEDED(hr)) {
                                                device = (IDirect3DDevice9 *)
                                                    deviceEx;
                                        }
                                }
                                IDirect3D9Ex_Release(d3d9ex);
                        }
                }
        }

        if (!device) {
                /* otherwise do regular D3D9 setup */
                IDirect3D9 *d3d9 = Direct3DCreate9(D3D_SDK_VERSION);

                hr = IDirect3D9_CreateDevice(
                    d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
                    D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_PUREDEVICE |
                        D3DCREATE_FPU_PRESERVE,
                    &present, &device);
                if (FAILED(hr)) {
                        /* hardware vertex processing not supported, no big deal
                        retry with software vertex processing */
                        hr = IDirect3D9_CreateDevice(
                            d3d9, D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, wnd,
                            D3DCREATE_SOFTWARE_VERTEXPROCESSING |
                                D3DCREATE_PUREDEVICE | D3DCREATE_FPU_PRESERVE,
                            &present, &device);
                        NK_ASSERT(SUCCEEDED(hr));
                }
                IDirect3D9_Release(d3d9);
        }
}
int run_gui_nuklear(const char *title,
                    void (*gui)(const char *title, struct nk_context *ctx,
                                int width, int height)) {
        struct nk_context *ctx;
        struct nk_colorf bg;

        WNDCLASSW wc;
        RECT rect = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
        DWORD style = WS_OVERLAPPEDWINDOW;
        DWORD exstyle = WS_EX_APPWINDOW;
        HWND wnd;
        int running = 1;

        wchar_t title_wide[128];

        /* Win32 */
        memset(&wc, 0, sizeof(wc));
        wc.style = CS_DBLCLKS;
        wc.lpfnWndProc = WindowProc;
        wc.hInstance = GetModuleHandleW(0);
        wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wc.hCursor = LoadCursor(NULL, IDC_ARROW);
        wc.lpszClassName = L"NuklearWindowClass";
        RegisterClassW(&wc);

        AdjustWindowRectEx(&rect, style, FALSE, exstyle);

        mbstowcs(title_wide, title, 128);

        wnd = CreateWindowExW(exstyle, wc.lpszClassName, title_wide,
                              style | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT,
                              rect.right - rect.left, rect.bottom - rect.top,
                              NULL, NULL, wc.hInstance, NULL);

        create_d3d9_device(wnd);

        /* GUI */
        ctx = nk_d3d9_init(device, WINDOW_WIDTH, WINDOW_HEIGHT);
        {
                struct nk_font_atlas *atlas;
                nk_d3d9_font_stash_begin(&atlas);
                /*struct nk_font *droid = nk_font_atlas_add_from_file(atlas,
                 * "../../extra_font/DroidSans.ttf", 14, 0);*/
                /*struct nk_font *robot = nk_font_atlas_add_from_file(atlas,
                 * "../../extra_font/Roboto-Regular.ttf", 14, 0);*/
                /*struct nk_font *future = nk_font_atlas_add_from_file(atlas,
                 * "../../extra_font/kenvector_future_thin.ttf", 13, 0);*/
                /*struct nk_font *clean = nk_font_atlas_add_from_file(atlas,
                 * "../../extra_font/ProggyClean.ttf", 12, 0);*/
                /*struct nk_font *tiny = nk_font_atlas_add_from_file(atlas,
                 * "../../extra_font/ProggyTiny.ttf", 10, 0);*/
                /*struct nk_font *cousine = nk_font_atlas_add_from_file(atlas,
                 * "../../extra_font/Cousine-Regular.ttf", 13, 0);*/
                nk_d3d9_font_stash_end();
                /*nk_style_load_all_cursors(ctx, atlas->cursors);*/
                /*nk_style_set_font(ctx, &droid->handle)*/;
        }

        bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;
        while (running) {
                /* Input */
                MSG msg;
                nk_input_begin(ctx);
                while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
                        if (msg.message == WM_QUIT)
                                running = 0;
                        if (msg.message == WM_SYSKEYDOWN && msg.wParam == 'Q' &&
                            GetKeyState(VK_CONTROL) & (1 << 15))
                                running = 0;
                        TranslateMessage(&msg);
                        DispatchMessageW(&msg);
                }
                nk_input_end(ctx);

                /* Completely untested */
                {
                        RECT rect;
                        GetClientRect(wnd, &rect);
                        gui(title, ctx, rect.right - rect.left,
                            rect.bottom - rect.top);
                }

                /* Draw */
                {
                        HRESULT hr;
                        hr = IDirect3DDevice9_Clear(
                            device, 0, NULL,
                            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER |
                                D3DCLEAR_STENCIL,
                            D3DCOLOR_COLORVALUE(bg.r, bg.g, bg.b, bg.a), 0.0f,
                            0);
                        NK_ASSERT(SUCCEEDED(hr));

                        hr = IDirect3DDevice9_BeginScene(device);
                        NK_ASSERT(SUCCEEDED(hr));
                        nk_d3d9_render(NK_ANTI_ALIASING_ON);
                        hr = IDirect3DDevice9_EndScene(device);
                        NK_ASSERT(SUCCEEDED(hr));

                        if (deviceEx) {
                                hr = IDirect3DDevice9Ex_PresentEx(
                                    deviceEx, NULL, NULL, NULL, NULL, 0);
                        } else {
                                hr = IDirect3DDevice9_Present(device, NULL,
                                                              NULL, NULL, NULL);
                        }
                        if (hr == D3DERR_DEVICELOST ||
                            hr == D3DERR_DEVICEHUNG ||
                            hr == D3DERR_DEVICEREMOVED) {
                                /* to recover from this, you'll need to recreate
                                 * device and all the resources */
                                MessageBoxW(NULL,
                                            L"D3D9 device is lost or removed!",
                                            L"Error", 0);
                                break;
                        } else if (hr == S_PRESENT_OCCLUDED) {
                                /* window is not visible, so vsync won't work.
                                 * Let's sleep a bit to reduce CPU usage */
                                Sleep(10);
                        }
                        NK_ASSERT(SUCCEEDED(hr));
                }
        }
        nk_d3d9_shutdown();
        if (deviceEx)
                IDirect3DDevice9Ex_Release(deviceEx);
        else
                IDirect3DDevice9_Release(device);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return 0;
}