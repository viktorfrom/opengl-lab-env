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

#include <iostream>
#include <iomanip>
#include <fstream>
#include <random>
#include <chrono>

#include "glm/gtx/string_cast.hpp"
#include <algorithm>
#include <vector>
// using namespace std;

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
"	Color = vec4(0,0,0,1);\n"
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

	void ExampleApp::readFromFile(std::string fileName)
	{
		std::ifstream inputFile(fileName);
		vecArr.clear();

		if (inputFile.is_open()) {
			GLfloat x_value;
			GLfloat y_value;

			while (inputFile >> x_value >> y_value) {
				ExampleApp::insertVec(glm::vec2(x_value, y_value));
				// cout << "x-value = " << x_value << " y-value =  " << y_value << endl;
			}
		}
		else {
			printf("File not found!");
			exit(1);
		}

		inputFile.close();
	}

	void ExampleApp::generateRandomPoints(int n) {
		if (n >= 3) {
			std::mt19937_64 rng;

			uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
			std::seed_seq ss{ uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32) };
			rng.seed(ss);

			std::uniform_real_distribution<GLfloat> unif(-0.8, 0.8);

			vecArr.clear();
			for (int i = 0; i < n; i++)
			{

				GLfloat x_value = unif(rng);
				GLfloat y_value = unif(rng);
				ExampleApp::insertVec(glm::vec2(x_value, y_value));
				// cout << "x-value = " << x_value << " y-value =  " << y_value << endl;
			}
		}
	}

	void ExampleApp::insertVec(glm::vec2 point) {
		GLfloat z = -1.0f;

		vecArr.push_back(glm::vec3(point, z));
	}

	void ExampleApp::terminalInput() {
		system("CLS");
		std::string input;
		std::cout << "Input a number of points 3 or more: ";
		std::getline(std::cin, input);
		this->generateRandomPoints(std::stoi(input));
		this->bufCollinear(std::stoi(input));
		system("CLS");
	}

	void ExampleApp::bufCollinear(int n) {
		for (int i = 0; i < vecArr.size() - 2; i++) {
			auto point1 = vecArr.at(i);
			auto point2 = vecArr.at(i + 1);
			auto point3 = vecArr.at(i + 2);

			GLfloat res = this->collinear(point1, point2, point3);

			// cout << "a = " << a;

			if (res == 0.0f) {
				this->bufCollinear(n);
			};
		};
	}

	GLfloat ExampleApp::collinear(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3)
	{
		return (point2.x - point1.x) * (point3.y - point1.y) - (point2.y - point1.y) * (point3.x - point1.x);
	}


	std::vector<glm::vec3> ExampleApp::convexHull(std::vector<glm::vec3> inputVector)
	{
		int n = inputVector.size();
		int k = 0;

		if (n <= 3) {
			return inputVector;
		}
		else {
			std::vector<glm::vec3> hull(2 * n);

			this->sortVector(inputVector);

			// Lower hull
			for (int i = 0; i < n; ++i) {
				while (k >= 2 && this->collinear(hull[k - 2], hull[k - 1], inputVector[i]) <= 0) k--;
				hull[k++] = inputVector[i];
			}

			// Upper hull
			for (int i = n - 1, t = k + 1; i > 0; --i) {
				while (k >= t && this->collinear(hull[k - 2], hull[k - 1], inputVector[i - 1]) <= 0) k--;
				hull[k++] = inputVector[i - 1];
			}

			hull.resize(k - 1);
			return hull;
		}
	}


	void ExampleApp::sortVector(std::vector<glm::vec3> &inputVector)
	{
		std::sort(inputVector.begin(), inputVector.end(),
			[](const glm::vec3& v1, const glm::vec3& v2) {
			if (v1.x == v2.x) {
				return (v1.y < v2.y);
			}
			else {
				return (v1.x < v2.x);
			}});
	}

	void ExampleApp::calcPointsInsideHull(std::vector<glm::vec3> &vecArr, std::vector<glm::vec3> &hull)
	{
		pointsInsideHull.clear();
		for (int i = 0; i < vecArr.size(); i++) {
			if (std::find(hull.begin(), hull.end(), vecArr.at(i)) != hull.end()) {
				continue;
			}
			else {
				pointsInsideHull.push_back(vecArr.at(i));
			};
		};

		if (!pointsInsideHull.empty()) {
			int Random = std::rand() % pointsInsideHull.size();
			auto randomPoint = pointsInsideHull[Random];

			pointsInsideHull.clear();
			test.push_back(randomPoint);
		}
	}



	//declaration for new tree node
	struct node {
		int data;
		struct node *left;
		struct node *right;
	};

	//allocates new node 
	struct node* newNode(int data) {
		// declare and allocate new node  
		struct node* node = new struct node();

		node->data = data;    // Assign data to this node

		// Initialize left and right children as NULL 
		node->left = NULL;
		node->right = NULL;
		return(node);
	}

	//------------------------------------------------------------------------------
	/**
	*/

	bool
		ExampleApp::Open()
	{
		App::Open();
		this->window = new Display::Window;
		this->window->SetSize(800, 800);
		window->SetKeyPressFunction([this](int32 key, int32 scancode, int32 action, int32 mods)
		{
			if (key == 256 && action == GLFW_PRESS) {
				this->window->Close();
			}
			else if (key == 51 && action == GLFW_PRESS) {
				this->terminalInput();
				buf = vecArr;

				hull = this->ExampleApp::convexHull(vecArr);
				this->calcPointsInsideHull(vecArr, hull);

			}
			else if (key == 50 && action == GLFW_PRESS) {
				this->readFromFile("test.txt");
				buf = vecArr;

				hull = this->ExampleApp::convexHull(vecArr);
				this->calcPointsInsideHull(vecArr, hull);


			}
			else if (key == 49 && action == GLFW_PRESS) {
				int n = 10;
				this->generateRandomPoints(n);
				this->bufCollinear(n);
				buf = vecArr;

				hull = this->ExampleApp::convexHull(vecArr);
				this->calcPointsInsideHull(vecArr, hull);



			}
		});

		if (this->window->Open())
		{
			// set background color
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
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glPointSize(5);
			return true;

		}
		return false;
	}



	//------------------------------------------------------------------------------
	/**
	*/
	void
		ExampleApp::Run()
	{
		// Enable blending
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		while (this->window->IsOpen())
		{
			glClear(GL_COLOR_BUFFER_BIT);
			this->window->Update();
			glBindBuffer(GL_ARRAY_BUFFER, this->triangle);
			glUseProgram(this->program);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, NULL);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (GLvoid*)(sizeof(float32) * 3));


			glBufferData(GL_ARRAY_BUFFER, pointsInsideHull.size() * sizeof(glm::vec3), pointsInsideHull.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, this->pointsInsideHull.size());

			// glDrawArrays(GL_TRIANGLES, 0, this->buf.size());
			glBufferData(GL_ARRAY_BUFFER, hull.size() * sizeof(glm::vec3), hull.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_LINE_LOOP, 0, this->hull.size());
			// glDrawArrays(GL_LINE_STRIP, 0, this->buf.size());
			glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(glm::vec3), buf.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, this->buf.size());

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			this->window->SwapBuffers();

		}
	}
} // namespace Example