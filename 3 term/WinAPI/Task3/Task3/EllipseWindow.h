#pragma once

class CMovingEllipse {
public:
	LONG xCoord, yCoord, figWidth, figHeight;
	LONG vx, vy;

	CMovingEllipse() : xCoord(0), yCoord(0), \
		figWidth(50), figHeight(50), vx(10), vy(10) {};
	~CMovingEllipse() {};

	void setEllipse(LONG _xCoord, LONG _yCoord, LONG _figWidth, LONG _figHeight, LONG _vx, LONG _vy)
	{
		xCoord = _xCoord;
		yCoord = _yCoord;
		figWidth = _figWidth;
		figHeight = _figHeight;
		vx = _vx;
		vy = _vy;
	}
};

class CEllipseWindow {
public:
	CEllipseWindow() {};
	~CEllipseWindow() {};

	static bool RegisterClass(HINSTANCE hInstance, const wchar_t CLASS_NAME[]);

	bool Create(HINSTANCE hInstance, const wchar_t CLASS_NAME[], HWND handleParent, int x, int y, int width, int height);

	void Show(int cmdShow);

	HWND getHandle() { return handle; }

protected:
	void OnDestroy();

private:
	HWND handle;

	CMovingEllipse myEllipse;

	static LRESULT __stdcall MsgRouter(HWND handle, UINT uMsg, WPARAM wParam, LPARAM lParam);
	
	LRESULT __stdcall wndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void storeHandle(HWND _handle);

	void onTimer();

	void onClose();

	void onClick();

	void handlePaint();
};
