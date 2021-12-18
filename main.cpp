#include "stdafx.h"
#include "MyRender.h"


/*
class MyInput : public InputListener
{
public:
	bool KeyPressed(const KeyEvent &arg)
	{
		printf("key press %c\n", arg.wchar);
		return false;
	}
	bool MouseMove(const MouseEvent &arg)
	{
		printf("mouse %d - %d\n", arg.x, arg.y);
		return false;
	}
};

*/

int main()
{
	
	Framework framework;

	MyRender *render = new MyRender();
	//MyInput *input = new (std::nothrow) MyInput();

	if (!render)
	{
		Log::Get()->Error("main(): can't allocate the memory for the render or input");
		return 1;
	}


	framework.SetRender(render);
	if (framework.Init())
	{
		//framework.AddInputListener(input);
		framework.Run();
	}

	framework.Close();

	return 0;
}
