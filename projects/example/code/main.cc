//------------------------------------------------------------------------------
// main.cc
// (C) 2015-2017 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"

int depth = 7;

int
main(int argc, const char** argv)
{
	Example::ExampleApp app;
	if (app.Open(depth))
	{
		app.Run(depth);
		app.Close();
	}
	app.Exit();
	
}
