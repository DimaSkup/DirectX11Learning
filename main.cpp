#include "stdafx.h"
#include "MyRender.h"

int main()
{
	Framework framework;

	MyRender* render = new MyRender();

	framework.SetRender(render);
	if (framework.Init())
	{
		framework.Run();
	}

	

	framework.Close();

	return 0;
}