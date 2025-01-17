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

		Node* leafNode(Node* parent, Node* left, Node* middle, Node* right, glm::vec2 c, glm::vec2 c_i, glm::vec2 c_j)
		{
			Node* node = new Node();

			node->data.push_back(c);
			node->data.push_back(c_i);
			node->data.push_back(c_j);

			node->parent = parent;
			node->left = left;
			node->middle = middle;
			node->right = right;
			return (node);
		}

		Node* binaryNode(Node* parent, Node* left, Node* middle, Node* right, glm::vec2 c, glm::vec2 c_i, glm::vec2 c_m, glm::vec2 c_j)
		{
			Node* node = new Node();

			node->data.push_back(c);
			node->data.push_back(c_i);
			node->data.push_back(c_m);
			node->data.push_back(c_j);

			node->parent = parent;
			node->left = left;
			node->middle = middle;
			node->right = right;
			return (node);
		};

		std::vector<glm::vec2> vecArr;
		std::vector<glm::vec2> buf;
		std::vector<glm::vec2> hull;
		std::vector<glm::vec2> pointsInsideHull;
		std::vector<glm::vec2> triangleMesh;
		std::vector<glm::vec2> triangleLines;

		// DEBUG VARIABLES
		int trianglesToDraw = 0;

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
		void ExampleApp::pointLocation(Node* tree, glm::vec2 point);
		void ExampleApp::insertPoints(Node* tree, glm::vec2 point);

		void ExampleApp::insertPointsOnLine(Node* tree, glm::vec2 point);

		bool ExampleApp::onSegment(glm::vec2 start, glm::vec2 point, glm::vec2 end);

		// show some ui things
		void RenderUI();

		GLchar *vsBuffer;
		GLchar *fsBuffer;
		std::string compilerLog;
	};
} // namespace Example