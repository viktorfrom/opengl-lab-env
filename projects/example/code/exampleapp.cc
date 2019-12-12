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
"	Color = vec4(1, 0, 0, 0.75);\n"
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


	// if the return value is 0 the third point is collinear to the first two
	// if the return value is positive the third point is to the 'left', negative it is to the 'right'
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
		}

		if (!pointsInsideHull.empty()) {
			std::srand(std::time(NULL));
			int Random = rand() % pointsInsideHull.size();
			auto randomPoint = pointsInsideHull[Random];

			// remove selected random point from pointsInsideHull
			pointsInsideHull.erase(std::find(pointsInsideHull.begin(), pointsInsideHull.end(), randomPoint));

			return randomPoint;
		}
	}

	ExampleApp::Node* ExampleApp::buildTree(Node* parent, std::vector<glm::vec2> &hull, glm::vec2 c)
	{
		if (hull.size() == 2) {
			return leafNode(parent, nullptr, nullptr, nullptr, c, hull[0], hull[1]);
			
		}
		else {
			auto c_i = hull[0];
			auto c_m = hull[hull.size() / 2];
			auto c_j = hull[hull.size() - 1];

			// create new parent binary node
			Node* root = binaryNode(parent, nullptr, nullptr, nullptr, c, c_i, c_m, c_j);

			// recursively compute left and right subtree
			root->left = buildTree(root, std::vector<glm::vec2>(hull.begin(), hull.begin() + hull.size() / 2 + 1), c);
			root->right = buildTree(root, std::vector<glm::vec2>(hull.begin() + hull.size() / 2, hull.end()), c);

			return root;
		}
	}

	void ExampleApp::getTriangles(Node* tree)
	{
		if (tree->left == nullptr && tree->right == nullptr && tree->middle == nullptr)
		{

			triangleMesh.push_back(tree->data[1]);
			triangleMesh.push_back(tree->data[0]);

			triangleMesh.push_back(tree->data[1]);
			triangleMesh.push_back(tree->data[2]);

			triangleMesh.push_back(tree->data[0]);
			triangleMesh.push_back(tree->data[2]);

			// for triangles 
			//triangleMesh.push_back(tree->data[0]);
			//triangleMesh.push_back(tree->data[1]);
			//triangleMesh.push_back(tree->data[2]);
		}
		else {
			ExampleApp::getTriangles(tree->left);
			if (tree->middle != nullptr) {
				ExampleApp::getTriangles(tree->middle);
			}
			ExampleApp::getTriangles(tree->right);
		}
	}

	void ExampleApp::insertPoints(Node* tree, glm::vec2 point)
	{
		if (tree->left == nullptr && tree->right == nullptr && tree->middle == nullptr)
		{
			std::vector<glm::vec2> temp;

			temp.push_back(tree->data[0]); // randompoint inside hull
			temp.push_back(tree->data[1]); // "left" point on hull
			temp.push_back(tree->data[2]); // "right" point on hull


			if (!pointsInsideHull.empty()) {
				auto left = leafNode(tree, nullptr, nullptr, nullptr, point, temp[0], temp[1]);
				auto middle = leafNode(tree, nullptr, nullptr, nullptr, point, temp[1], temp[2]);
				auto right = leafNode(tree, nullptr, nullptr, nullptr, point, temp[2], temp[0]);

				tree->left = left;
				tree->middle = middle;
				tree->right = right;
			}
			
		}
		else {
			this->ExampleApp::pointLocation(tree, point);
		}
	}

	void ExampleApp::pointLocation(Node * tree, glm::vec2 point)
	{
		glm::vec2 c = tree->data[0];
		glm::vec2 c_i = tree->data[1];
		glm::vec2 c_m = tree->data[2];


		if (tree->middle != nullptr) {
			std::cout << "treneary node found!" << std::endl;


			glm::vec2 c = tree->left->data[0]; // p
			glm::vec2 c_i = tree->data[0];
			glm::vec2 c_j = tree->data[1];
			glm::vec2 c_m = tree->data[2];
			
			if (collinear(c_i, c, point) < 0 && collinear(c_j, c, point) >= 0) {
				// std::cout << "left" << std::endl;

				ExampleApp::insertPoints(tree->left, point);
			}
			 
			
			else if (collinear(c_j, c, point) < 0 && collinear(c_m, c, point) >= 0) {
				// std::cout << "middle" << std::endl;

				ExampleApp::insertPoints(tree->middle, point);
			}
			
			else if (collinear(c_m, c, point) < 0 && collinear(c_i, c, point) >= 0) {
				// std::cout << "right" << std::endl;

				ExampleApp::insertPoints(tree->right, point);
			}
		} else if (collinear(c, c_m, point) == 0 || collinear(c_m, c_i, point) == 0 || collinear(c_i, c, point) == 0) {
			// std::cout << "point is on a line!" << std::endl;

			if (collinear(c, c_m, point) == 0 && tree->left != nullptr) {
				std::cout << "point is on left line!" << std::endl;
				ExampleApp::pointOnLine(tree, point);
			}

			if (collinear(c_m, c_i, point) == 0 && tree->middle != nullptr) {
				std::cout << "point is on middle line!" << std::endl;
				ExampleApp::pointOnLine(tree, point);
			}

			if (collinear(c_i, c, point) == 0 && tree->right != nullptr) {
				std::cout << "point is on right line!" << std::endl;
				ExampleApp::pointOnLine(tree, point);
			}
		}
		else {
			if (collinear(c_m, c, c_i) >= 0) { // case 1
				if (collinear(c_i, c, point) < 0 && collinear(c_m, c, point) >= 0) {
					ExampleApp::insertPoints(tree->left, point);
					// std::cout << "left" << std::endl;
				}
				else {
					ExampleApp::insertPoints(tree->right, point);
					// std::cout << "right" << std::endl;
				}
			}
			else if (collinear(c_m, c, c_i) < 0) { // case 2
				if (collinear(c_i, c, point) < 0 || collinear(c_m, c, point) >= 0) {
					ExampleApp::insertPoints(tree->left, point);
					// std::cout << "left" << std::endl;
				}
				else {
					ExampleApp::insertPoints(tree->right, point);
					// std::cout << "right" << std::endl;
				}
			}
		}
	}

	void ExampleApp::pointOnLine(Node* tree, glm::vec2 point) {
			//auto newLeftNode = binaryNode(tree->parent, nullptr, nullptr, nullptr, tree->parent->data[0], tree->parent->data[1], tree->parent->data[2], tree->parent->data[3]);

			//auto left123 = leafNode(newLeftNode, nullptr, nullptr, nullptr, point, tree->data[1], tree->data[0]);
			//auto right123 = leafNode(newLeftNode, nullptr, nullptr, nullptr, point, tree->data[1], tree->data[2]);


			//newLeftNode->left = left123;
			//newLeftNode->right = right123;


			//auto newRightNode = binaryNode(tree->parent, nullptr, nullptr, nullptr, tree->parent->data[0], tree->parent->data[1], tree->parent->data[2], tree->parent->data[3]);

			//auto left456 = leafNode(newRightNode, nullptr, nullptr, nullptr, point, tree->data[2], tree->data[1]);
			//auto right456 = leafNode(newRightNode, nullptr, nullptr, nullptr, point, tree->data[2], tree->data[0]);


			//newRightNode->left = left456;
			//newRightNode->right = right456;

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
					hull.push_back(hull[0]);
					auto randomPoint = this->calcPointsInsideHull(vecArr, hull);

					auto tree = this->ExampleApp::buildTree(nullptr, hull, randomPoint);

					triangleMesh.clear();
					this->ExampleApp::getTriangles(tree);

				}
				else if (key == 50 && action == GLFW_PRESS) {
					vecArr.clear();
					buf.clear();
					hull.clear();
					pointsInsideHull.clear();
					triangleMesh.clear();


					this->readFromFile("testFan.txt");

					buf = vecArr;


					hull = this->ExampleApp::convexHull(vecArr);
					// add first point of hull again to wrap around
					hull.push_back(hull[0]);
					auto randomPoint = this->calcPointsInsideHull(vecArr, hull);

					this->sortVector(pointsInsideHull);

					// HARD CODED FOR TESTING
					pointsInsideHull.clear();

					// pointsInsideHull.push_back(glm::vec2(0.277f, 0.277f));
					//pointsInsideHull.push_back(glm::vec2(-0.277f, 0.277f));
					//pointsInsideHull.push_back(glm::vec2(0.277f, -0.277f));
					//pointsInsideHull.push_back(glm::vec2(-0.277f, -0.277f));
					//pointsInsideHull.push_back(glm::vec2(-0.277f, 0.0f));
					//pointsInsideHull.push_back(glm::vec2(0.277f, 0.0f));

					//pointsInsideHull.push_back(glm::vec2(0.3f, 0.1f));
					//pointsInsideHull.push_back(glm::vec2(0.477f, 0.1f));
					//pointsInsideHull.push_back(glm::vec2(0.377f, -0.05f));
					//pointsInsideHull.push_back(glm::vec2(0.5f, -0.1f));
					//pointsInsideHull.push_back(glm::vec2(0.45f, 0.15f));
					//pointsInsideHull.push_back(glm::vec2(0.55f, 0.0f));
					//pointsInsideHull.push_back(glm::vec2(0.25f, -0.05f));

					pointsInsideHull.push_back(glm::vec2(0.577/2.0f, 0.333/2.0f));
					buf.push_back(glm::vec2(0.577 / 2.0f, 0.333 / 2.0f));


					auto tree = this->ExampleApp::buildTree(nullptr, hull, glm::vec2(0.0f, 0.0f));


					while (!pointsInsideHull.empty()) {
						auto selectedPoint = pointsInsideHull[0];
						this->ExampleApp::insertPoints(tree, selectedPoint);
						pointsInsideHull.erase(std::find(pointsInsideHull.begin(), pointsInsideHull.end(), pointsInsideHull[0]));
					}

					// remove point from pointsInsideHull




					triangleMesh.clear();
					this->ExampleApp::getTriangles(tree);
				}

				else if (key == 49 && action == GLFW_PRESS) {
					int n = 10;
					this->generateRandomPoints(n);
					this->bufCollinear(n);

					buf = vecArr;

					hull = this->ExampleApp::convexHull(vecArr);
					hull.push_back(hull[0]);
					auto randomPoint = this->calcPointsInsideHull(vecArr, hull);


					auto tree = this->ExampleApp::buildTree(nullptr, hull, randomPoint);


					while (!pointsInsideHull.empty()) {
						auto selectedPoint = pointsInsideHull[0];
						this->ExampleApp::insertPoints(tree, selectedPoint);
						pointsInsideHull.erase(std::find(pointsInsideHull.begin(), pointsInsideHull.end(), pointsInsideHull[0]));
					}

					// remove point from pointsInsideHull

					if (pointsInsideHull.empty()) {
						std::cout << "empty" << std::endl;
					}


					triangleMesh.clear();
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


			glBufferData(GL_ARRAY_BUFFER, triangleMesh.size() * sizeof(glm::vec2), triangleMesh.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_LINES, 0, this->triangleMesh.size());

			glBufferData(GL_ARRAY_BUFFER, triangleMesh.size() * sizeof(glm::vec2), triangleMesh.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_TRIANGLES, 0, this->triangleMesh.size());

			glBufferData(GL_ARRAY_BUFFER, pointsInsideHull.size() * sizeof(glm::vec2), pointsInsideHull.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, this->pointsInsideHull.size());

			glBufferData(GL_ARRAY_BUFFER, hull.size() * sizeof(glm::vec2), hull.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_LINE_LOOP, 0, this->hull.size());

			glBufferData(GL_ARRAY_BUFFER, buf.size() * sizeof(glm::vec2), buf.data(), GL_STATIC_DRAW);
			glDrawArrays(GL_POINTS, 0, this->buf.size());

			glBindBuffer(GL_ARRAY_BUFFER, 0);

			this->window->SwapBuffers();

		}
	}
} // namespace Example