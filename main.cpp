#include "stdafx.h"
#include "MyRender.h"
#include "MyInput.h"

using namespace D3D11Framework;

int main()
{
	Framework framework;
	MyInput* myInput = new MyInput();
	MyRender* myRender = new MyRender();
	
	framework.SetRender(myRender);


	if (framework.Init())
	{
		framework.AddInputListener(myInput);
		framework.Run();
	}
		

	framework.Close();

	return 0;
}