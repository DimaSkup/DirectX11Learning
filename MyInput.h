#pragma once

using namespace D3D11Framework;



class MyInput : public InputListener
{
public:
	bool MouseMove(const MouseEvent &arg)
	{
		printf("the cursor at %d:%d\n", arg.x, arg.y);
		
		POINT cur = { arg.x, arg.y };
		UINT width = 640;
		UINT height = 480;
		UINT midWidth = width / 2;
		UINT midHeight = height / 2;

		
		

		return false;
	}

	
	bool KeyPressed(const KeyEvent &arg)
	{
		printf("%c button is pressed\n", arg.wch);
		button = arg.wch;

		switch (button)
		{
		case 'w':
			Framework::GetCamera()->ModifyCameraView({ 0.0f, 0.0f, 0.3f }, { 0.0f, 0.0f, 0.3f });
			break;
		case 's':
			Framework::GetCamera()->ModifyCameraView({ 0.0f, 0.0f, -0.3f }, { 0.0f, 0.0f, -0.3f });
			break;
		case 'a':
			Framework::GetCamera()->ModifyCameraView({ 0.0f, 0.0f, 0.0f }, { -0.2f, 0.0f, 0.0f });
			break;
		case 'd':
			Framework::GetCamera()->ModifyCameraView({ 0.0f, 0.0f, 0.0f }, { 0.2f, 0.0f, 0.0f });
			break;
		}
		

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