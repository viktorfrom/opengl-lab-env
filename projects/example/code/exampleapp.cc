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
"layout(location=0) in vec2 pos;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(pos, -1, 1);\n"
"}\n";

const GLchar* ps =
"#version 310 es\n"
"precision mediump float;\n"
"layout(location=0) in vec4 color;\n"
"out vec4 Color;\n"
"void main()\n"
"{\n"
"	Color = vec4(0, 0, 0, 1);\n"
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

	void ExampleApp::generateRandomPoints(int n) 
	{
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

	void ExampleApp::insertVec(glm::vec2 point)
	{
		vecArr.push_back(point);
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

	GLfloat ExampleApp::collinear(glm::vec2 point1, glm::vec2 point2, glm::vec2 point3)
	{
		return (point2.x - point1.x) * (point3.y - point1.y) - (point2.y - point1.y) * (point3.x - point1.x);
	}


	std::vector<glm::vec2> ExampleApp::convexHull(std::vector<glm::vec2> inputVector)
	{
		int n = inputVector.size();
		int k = 0;

		if (n <= 3) {
			return inputVector;
		}
		else {
			std::vector<glm::vec2> hull(2 * n);

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


	void ExampleApp::sortVector(std::vector<glm::vec2> &inputVector)
	{
		std::sort(inputVector.begin(), inputVector.end(),
			[](const glm::vec2& v1, const glm::vec2& v2) {
			if (v1.x == v2.x) {
				return (v1.y < v2.y);
			}
			else {
				return (v1.x < v2.x);
			}});
	}

	glm::vec2 ExampleApp::calcPointsInsideHull(std::vector<glm::vec2> &vecArr, std::vector<glm::vec2> &hull)
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

			// remove selected random point from pointsInsideHull
			pointsInsideHull.erase(std::find(pointsInsideHull.begin(), pointsInsideHull.end(), randomPoint));

			return randomPoint;
		}
	}

	// takes parent node, hull and random point inside hull as argument
	ExampleApp::Node* ExampleApp::buildTree(Node* parent, std::vector<glm::vec2> &hull, glm::vec2 c)
	{
		if (hull.size() == 2) {

			// check if base case triangle contains points
			if (!pointsInsideHull.empty()) {
				for (int i = 0; i < pointsInsideHull.size(); i++) {
					if (isPointInside(hull[0], hull[1], c, pointsInsideHull[i])) {

						std::cout << "point found " << std::endl;
						


						//parent->left = newNode(parent, nullptr, nullptr, nullptr, hull[1], hull[0], pointsInsideHull[i]);
						//parent->middle = newNode(parent, nullptr, nullptr, nullptr, pointsInsideHull[i], hull[1], c);
						//parent->right = newNode(parent, nullptr, nullptr, nullptr, pointsInsideHull[i], hull[1], c);


						test2.push_back(pointsInsideHull[i]);
						test2.push_back(hull[0]);

						test2.push_back(pointsInsideHull[i]);
						test2.push_back(hull[1]);

						test2.push_back(pointsInsideHull[i]);
						test2.push_back(c);
					}
				}
			}
			return newNode(parent, nullptr, nullptr, nullptr, hull[0], hull[1], c);

		} 
		else {
		// create new (empty) binary node
		Node* root = new Node();

		// recursively compute left and right subtree
		root->left = buildTree(root, std::vector<glm::vec2>(hull.begin(), hull.begin() + hull.size() / 2 + 1), c);
		root->right = buildTree(root, std::vector<glm::vec2>(hull.begin() + hull.size() / 2, hull.end()), c);

		return root;
		}
	}

	void ExampleApp::getTriangles(Node* tree) 
	{
		if (tree->left == nullptr && tree->right == nullptr)
		{
			// random point inside hull to first point on hull
			test.push_back(tree->data[1]);
			test.push_back(tree->data[0]);

			// random point inside hull to second point on hull
			test.push_back(tree->data[1]);
			test.push_back(tree->data[2]);
		}
		else {
			ExampleApp::getTriangles(tree->left);
			ExampleApp::getTriangles(tree->right);
		}
	}

	GLfloat ExampleApp::triangleArea(glm::vec2 left, glm::vec2 top, glm::vec2 right)
	{
		return abs((left.x*(top.y - right.y) + top.x * (right.y - left.y) + right.x * (left.y - top.y)) / 2.0f);
	}

	bool ExampleApp::isPointInside(glm::vec2 left, glm::vec2 top, glm::vec2 right, glm::vec2 point)
	{
		// calc left-top-right
		GLfloat A = triangleArea(left, top, right);

		// calc point-top-right
		GLfloat A1 = triangleArea(point, top, right);

		// calc point-left-right
		GLfloat A2 = triangleArea(left, point, right);

		// calc point-point-left-top
		GLfloat A3 = triangleArea(left, top, point);

		// sum of original area = new area
		return (A == A1 + A2 + A3);
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
				auto randomPoint = this->calcPointsInsideHull(vecArr, hull);

				auto tree = this->ExampleApp::buildTree(nullptr, hull, randomPoint);

				test.clear();
				this->ExampleApp::getTriangles(tree);

			}
			else if (key == 50 && action == GLFW_PRESS) {
				this->readFromFile("testFan.txt");
				buf = vecArr;

				hull = this->ExampleApp::convexHull(vecArr);
				// auto randomPoint = this->calcPointsInsideHull(vecArr, hull);


				// HARDCODED: FOR TESTING
				pointsInsideHull.clear();
				pointsInsideHull.push_back(glm::vec2(0.277f, 0.0f));
			    pointsInsideHull.push_back(glm::vec2(0.477f, 0.1f));

				test2.clear();
				auto tree = this->ExampleApp::buildTree(nullptr, hull, glm::vec2(0.0f, 0.0f));

				test.clear();
				this->ExampleApp::getTriangles(tree);
			}

			else if (key == 49 && action == GLFW_PRESS) {
				int n = 10;
				this->generateRandomPoints(n);
				this->bufCollinear(n);
				buf = vecArr;

				hull = this->ExampleApp::convexHull(vecArr);
				auto randomPoint = this->calcPointsInsideHull(vecArr, hull);


				test2.clear();
				auto tree = this->ExampleApp::buildTree(nullptr, hull, randomPoint);

				test.clear();
				this->ExampleApp::getTriangles(tree);


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
			glGenBuffers(1, &this->mesh);
			glBindBuffer(GL_ARRAY_BUFFER, this->mesh);
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
			glBindBuffer(GL_ARRAY_BUFFER, this->mesh);
			glUseProgram(this->program);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, NULL);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, (GLvoid*)(sizeof(float32) * 2));


			glBufferData(GL_ARRAY_BUFFER, test.size() * sizeof(glm::vec2), test.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_LINE_LOOP, 0, this->test.size());

			glBufferData(GL_ARRAY_BUFFER, test2.size() * sizeof(glm::vec2), test2.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_LINE_LOOP, 0, this->test2.size());


			glBufferData(GL_ARRAY_BUFFER, pointsInsideHull.size() * sizeof(glm::vec2), pointsInsideHull.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, this->pointsInsideHull.size());

			// glDrawArrays(GL_TRIANGLES, 0, this->buf.size());
			glBufferData(GL_ARRAY_BUFFER, hull.size() * sizeof(glm::vec2), hull.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_LINE_LOOP, 0, this->hull.size());
			// glDrawArrays(GL_LINE_STRIP, 0, this->buf.size());
			glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(glm::vec2), buf.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, this->buf.size());

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			this->window->SwapBuffers();

		}
	}
} // namespace Example