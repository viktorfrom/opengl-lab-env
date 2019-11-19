//------------------------------------------------------------------------------
// exampleapp.cc
// (C) 2015-2017 Individual contributors, see AUTHORS file
//------------------------------------------------------------------------------
#include "config.h"
#include "exampleapp.h"
#include <cstring>
#include <glm/glm.hpp>
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4
#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/gtc/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include <cmath>

const GLchar* vs =
"#version 310 es\n"
"precision mediump float;\n"
"layout(location=0) in vec3 pos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(pos, 1);\n"
"}\n";

const GLchar* ps =
"#version 310 es\n"
"precision mediump float;\n"
"out vec4 Color;\n"
"void main()\n"
"{\n"
"	Color = vec4(1,0,0,1);\n"
"}\n";

using namespace Display;

namespace Example
{
	//------------------------------------------------------------------------------
	/**
	*/
	ExampleApp::ExampleApp() // Constructor 
	{
		// empty
	}

	//------------------------------------------------------------------------------
	/**
	*/
	ExampleApp::~ExampleApp() // Destructor
	{
		// empty	
	}

	void ExampleApp::KochSnowflake(glm::vec2 top, glm::vec2 left, glm::vec2 right, int n)
	{
		if (n == 1) {
			ExampleApp::insertVec(top, right);

		} else if(n >= 2) {
			glm::vec2 q0 = (2.0f * top + right) / 3.0f;
			glm::vec2 q1 = (top + 2.0f * right) / 3.0f;

			glm::vec2 left_q0 = (2.0f * top + left) / 3.0f;
			ExampleApp::KochSnowflake(top, left_q0, q0, n - 1);

			glm::vec2 m = glm::vec2((top.x + right.x) / 2.0f, (top.y + right.y) / 2.0f);
			GLfloat a_height = sqrtf(powf((q0.x - q1.x), 2.0f) + powf((q0.y - q1.y), 2.0f));
			glm::vec2 a = (glm::normalize(m - left) * (float)(a_height)) + m;

			glm::vec2 test = (2.0f * q1 + a) / 3.0f;

			ExampleApp::KochSnowflake(q0, test, a, n - 1);

			glm::vec2 test2 = (2.0f * q0 + a) / 3.0f;
			ExampleApp::KochSnowflake(a, test2, q1, n - 1);

			glm::vec2 bottom_q1 = (left + 2.0f * right) / 3.0f;
			ExampleApp::KochSnowflake(q1, bottom_q1, right, n - 1);
			
		}
	}

	void ExampleApp::insertVec(glm::vec2 first, glm::vec2 second) {
		GLfloat z = -1.0f;

		vecArr.push_back(glm::vec3(first, z));
		vecArr.push_back(glm::vec3(second, z));

	}

	//------------------------------------------------------------------------------
	/**
	*/


	bool
		ExampleApp::Open(int depth)
	{
		App::Open();
		this->window = new Display::Window;
		this->window->SetSize(800, 800);
		window->SetKeyPressFunction([this](int32, int32, int32, int32)
		{
			this->window->Close();
		});


		// Orders of insert matters!
		glm::vec2 p0 = glm::vec2(0.0f/1.5f, 1.0f/1.5f);		 // pos0, top
		glm::vec2 p1 = glm::vec2(-0.866f/1.5f, -0.5f/1.5f);  // pos1, left
		glm::vec2 p2 = glm::vec2(0.866f/1.5f, -0.5f/1.5f);   // pos2, right

		KochSnowflake(p0, p1, p2, depth);
		KochSnowflake(p2, p0, p1, depth);
		KochSnowflake(p1, p2, p0, depth);

		buf = vecArr;

		
		if (this->window->Open())
		{
			// set clear color to gray
			glClearColor(1.0f, 1.0f, 0.6f, 1.0f);

			// setup vertex shader
			this->vertexShader = glCreateShader(GL_VERTEX_SHADER);
			GLint length = (GLint)std::strlen(vs);
			glShaderSource(this->vertexShader, 1, &vs, &length);
			glCompileShader(this->vertexShader);

			// get error log
			GLint shaderLogSize;
			glGetShaderiv(this->vertexShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
			if (shaderLogSize > 0)
			{
				GLchar* buf = new GLchar[shaderLogSize];
				glGetShaderInfoLog(this->vertexShader, shaderLogSize, NULL, buf);
				printf("[SHADER COMPILE ERROR]: %s", buf);
				delete[] buf;
			}

			// setup pixel shader
			this->pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
			length = (GLint)std::strlen(ps);
			glShaderSource(this->pixelShader, 1, &ps, &length);
			glCompileShader(this->pixelShader);

			// get error log
			shaderLogSize;
			glGetShaderiv(this->pixelShader, GL_INFO_LOG_LENGTH, &shaderLogSize);
			if (shaderLogSize > 0)
			{
				GLchar* buf = new GLchar[shaderLogSize];
				glGetShaderInfoLog(this->pixelShader, shaderLogSize, NULL, buf);
				printf("[SHADER COMPILE ERROR]: %s", buf);
				delete[] buf;
			}

			// create a program object
			this->program = glCreateProgram();
			glAttachShader(this->program, this->vertexShader);
			glAttachShader(this->program, this->pixelShader);
			glLinkProgram(this->program);
			glGetProgramiv(this->program, GL_INFO_LOG_LENGTH, &shaderLogSize);
			if (shaderLogSize > 0)
			{
				GLchar* buf = new GLchar[shaderLogSize];
				glGetProgramInfoLog(this->program, shaderLogSize, NULL, buf);
				printf("[PROGRAM LINK ERROR]: %s", buf);
				delete[] buf;
			}

			// setup vbo
			glGenBuffers(1, &this->triangle);
			glBindBuffer(GL_ARRAY_BUFFER, this->triangle);
			glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(glm::vec3), &buf[0], GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			return true;

		}
		return false;
	}

	//------------------------------------------------------------------------------
	/**
	*/
	void
		ExampleApp::Run(int depth)
	{
		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		while (this->window->IsOpen())
		{
			glClear(GL_COLOR_BUFFER_BIT);
			this->window->Update();

			// do stuff
			glBindBuffer(GL_ARRAY_BUFFER, this->triangle);
			glUseProgram(this->program);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
			// glDrawArrays(GL_TRIANGLES, 0, this->vecArr.size());	
			// glDrawArrays(GL_LINE_LOOP, 0, this->vecArr.size());
			glDrawArrays(GL_LINE_STRIP, 0, this->vecArr.size());
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			this->window->SwapBuffers();
		}
	}
} // namespace Example