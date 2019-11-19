#pragma once
//------------------------------------------------------------------------------
/**
	Application class used for example application.
	
	(C) 2015-2017 Individual contributors, see AUTHORS file
*/
//------------------------------------------------------------------------------
#include "core/app.h"
#include "render/window.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp> // glm::vec3

namespace Example
{
class ExampleApp : public Core::App
{
public:
	/// constructor
	ExampleApp();
	/// destructor
	~ExampleApp();

	/// open app
	bool Open(int depth);
	/// run app
	void Run(int depth);
private:
	std::vector<glm::vec3> vecArr;
	std::vector<glm::vec3> buf;

	GLuint program;
	GLuint vertexShader;
	GLuint pixelShader;
	GLuint triangle;
	Display::Window* window;
	void KochSnowflake(glm::vec2 p0, glm::vec2 p1, glm::vec2 p2, int depth);
	void ExampleApp::insertVec(glm::vec2 first, glm::vec2 second);
	void ExampleApp::findPoints(glm::vec2 c, glm::vec2 b, glm::vec2 d, int n);
};
} // namespace Example