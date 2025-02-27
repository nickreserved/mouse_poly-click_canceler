#include <windows.h>
//#include <stdio.h>

#define TIME_DIFF_MSEC 50


HHOOK hMouseHook;
POINT lastPoint[2];
UINT_PTR timer_id[2] = { 0, 0 };


static VOID CALLBACK TimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	int button_id = idEvent == timer_id[1];
	KillTimer(NULL, idEvent);
	//printf("TimerProc for MouseButton_%d, timer_id %llu\n", button_id, idEvent);

	BOOL swapped = GetSystemMetrics(SM_SWAPBUTTON);
	INPUT input = {0};
	input.type = INPUT_MOUSE;
	input.mi.dx = lastPoint[button_id].x;
	input.mi.dy = lastPoint[button_id].y;
	input.mi.dwFlags = button_id == swapped ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
	SendInput(1, &input, sizeof(INPUT));
}


static LRESULT CALLBACK button_down(int nCode, WPARAM wParam, LPARAM lParam, int button_id)
{
	BOOL running = KillTimer(NULL, timer_id[button_id]);
	//printf("MouseDown_%d with timer %llu running: %d\n", button_id + 1, timer_id[button_id], (int) running);
	if (running) return 1; // block the event
	else return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}


static LRESULT CALLBACK button_up(LPARAM lParam, int button_id)
{
	MSLLHOOKSTRUCT* ms = (MSLLHOOKSTRUCT*) lParam;
	lastPoint[button_id] = ms->pt;
	timer_id[button_id] = SetTimer(NULL, button_id + 1, TIME_DIFF_MSEC, TimerProc);
	//printf("MouseUp_%d postponed with timer_id: %llu\n", button_id + 1, timer_id[button_id]);
	return 1; // block the event
}


static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode >= 0)
	{
		if (wParam == WM_LBUTTONDOWN) return button_down(nCode, wParam, lParam, 0);
		if (wParam == WM_RBUTTONDOWN) return button_down(nCode, wParam, lParam, 1);
		if (wParam == WM_LBUTTONUP)
		{
			if (!(((MSLLHOOKSTRUCT*) lParam)->flags & LLMHF_INJECTED)) // event injected from SendInput
				return button_up(lParam, 0);
			//else printf("injected event\n");
		}
		if (wParam == WM_RBUTTONUP)
		{
			if (!(((MSLLHOOKSTRUCT*) lParam)->flags & LLMHF_INJECTED)) // event injected from SendInput
				return button_up(lParam, 1);
			//else printf("injected event\n");
		}
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}


int main()
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
{
	hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetModuleHandle(NULL), 0);
  
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnhookWindowsHookEx(hMouseHook);
}
