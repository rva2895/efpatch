#pragma once
void initialSetup();

void __stdcall update_editor_bk();

int CALLBACK WndProc_dll(HWND hWnd,
    UINT msg,
    WPARAM wParam,
    LPARAM lParam);
