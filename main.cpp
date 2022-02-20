#include "stdafx.h"
#include "MyRender.h"
#include "MyInput.h"

using namespace D3D11Framework;

int main()
{
	Framework framework;
	MyInput* myInput = new MyInput();
	MyRender* render = new MyRender();

	FrameworkDesc desc;
	desc.render = render;

	if (framework.Init(desc))
	{
		framework.AddInputListener(myInput);
		framework.Run();
	}
		

	framework.Close();

	return 0;
}