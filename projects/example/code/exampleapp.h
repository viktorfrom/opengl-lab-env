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
		std::vector<glm::vec3> vecArr;
		std::vector<glm::vec3> hull;
		std::vector<glm::vec3> buf;
		std::vector<glm::vec3> pointsInsideHull;
		std::vector<glm::vec3> test;

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


		GLuint program;
		GLuint vertexShader;
		GLuint pixelShader;
		GLuint triangle;
		Display::Window* window;
		void ExampleApp::readFromFile(std::string fileName);
		void ExampleApp::insertVec(glm::vec2 point);
		void ExampleApp::generateRandomPoints(int n);
		void ExampleApp::terminalInput();
		void ExampleApp::bufCollinear(int n);
		std::vector<glm::vec3> ExampleApp::convexHull(std::vector<glm::vec3> inputVector);
		void ExampleApp::sortVector(std::vector<glm::vec3> &inputVector);
		GLfloat ExampleApp::collinear(glm::vec3 point1, glm::vec3 point2, glm::vec3 point3);

		void ExampleApp::calcPointsInsideHull(std::vector<glm::vec3> &inputVector, std::vector<glm::vec3> &inputVector2);


		Node* buildTree(glm::vec3 , const std::vector<glm::vec3>& points, Node* p);
	};
} // namespace Example