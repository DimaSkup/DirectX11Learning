#include "stdafx.h"
#include "D3D11_Framework.h"

using namespace D3D11Framework;

class MyInput : public InputListener
{
public:
	bool KeyPressed(const KeyEvent &arg)
	{
		printf("key pressed: %c\n", arg.wc);
		return false;
	}

	bool MouseMove(const MouseEvent &arg)
	{
		printf("mouse move: %d : %d\n", arg.x, arg.y);
		return false;
	}

	bool MousePressed(const MouseEventClick &arg)
	{
		if (arg.btn == MOUSE_LEFT)
		{
			printf("mouse click - %s a %d : %d\n", "MOUSE_LEFT", arg.x, arg.y);
			return false;
		}
	}
};

class MyRender : public Render
{
public:
	bool Init(HWND hWnd) { return true; }
	bool Draw(void) { return true; }
	void Close(void) {};
};

int main()
{
	MyInput* inputListener = new MyInput();
	MyRender* render = new MyRender();


	Framework framework;

	
	
	framework.SetRender(render);
	framework.Init();
	framework.AddInputListener(inputListener);
	
	

	framework.Run();

	framework.Close();


	return 0;
}