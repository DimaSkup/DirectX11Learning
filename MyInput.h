#pragma once
#include "MyRender.h"

class MyInput : public InputListener
{
public:

	bool KeyPressed(const KeyEvent &arg)
	{
		printf("%c button is pressed\n", arg.wch);
		button = arg.wch;
		return false;
	}

	bool KeyReleased(const KeyEvent &arg)
	{
		printf("%c button is released\n", arg.wch);
		button = NULL;
		return false;
	}

	bool MousePressed(const MouseClickEvent& arg)
	{
		switch (arg.button)
		{
		case MOUSE_LEFT:
			printf("LEFT mouse button is pressed at %d:%d\n", arg.x, arg.y);
			break;
		case MOUSE_RIGHT:
			printf("RIGHT mouse button is pressed at %d:%d\n", arg.x, arg.y);
			break;
		case MOUSE_MIDDLE:
			printf("MIDDLE mouse button is pressed at %d:%d\n", arg.x, arg.y);
			break;
		default:
			printf("UNKNOWN MOUSE BUTTON\n");
		}

		return false;
	}

	wchar_t button;
};