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
		bool Open();
		/// run app
		void Run();
	private:
		struct Node
		{
			Node* parent = nullptr;
			Node* left = nullptr;
			Node* middle = nullptr;
			Node* right = nullptr;
			std::vector<glm::vec2> data;
		};

		Node* newNode(Node* parent, Node* left, Node* middle, Node* right, glm::vec2 &point1, glm::vec2 &point2, glm::vec2 c)
		{
			// Node* node = (Node*)malloc(sizeof(Node));
			Node* node = new Node();

			node->data.push_back(point1);
			node->data.push_back(c);
			node->data.push_back(point2);
			// node->data.push_back(point1);

			node->left = left;
			node->middle = middle;
			node->right = right;
			return (node);
		};

		std::vector<glm::vec2> vecArr;
		std::vector<glm::vec2> hull;
		std::vector<glm::vec2> buf;
		std::vector<glm::vec2> pointsInsideHull;
		std::vector<glm::vec2> test;

		std::vector<glm::vec2> leftSide;
		std::vector<glm::vec2> rightSide;


		std::vector<glm::vec2> testHull;
		std::vector<glm::vec2> test2;



		GLuint program;
		GLuint vertexShader;
		GLuint pixelShader;
		GLuint mesh;
		Display::Window* window;
		void ExampleApp::readFromFile(std::string fileName);
		void ExampleApp::insertVec(glm::vec2 point);
		void ExampleApp::generateRandomPoints(int n);
		void ExampleApp::terminalInput();
		void ExampleApp::bufCollinear(int n);
		std::vector<glm::vec2> ExampleApp::convexHull(std::vector<glm::vec2> inputVector);
		void ExampleApp::sortVector(std::vector<glm::vec2> &inputVector);
		GLfloat ExampleApp::collinear(glm::vec2 point1, glm::vec2 point2, glm::vec2 point3);
		glm::vec2 ExampleApp::calcPointsInsideHull(std::vector<glm::vec2> &inputVector, std::vector<glm::vec2> &inputVector2);

		Node* ExampleApp::buildTree(Node* parent, std::vector<glm::vec2> &hull, glm::vec2 c);
		void ExampleApp::getTriangles(Node* tree);

		// point location inside triangle
		GLfloat ExampleApp::triangleArea(glm::vec2 left, glm::vec2 top, glm::vec2 right);
		bool ExampleApp::isPointInside(glm::vec2 left, glm::vec2 top, glm::vec2 right, glm::vec2 point);

	};
} // namespace Example