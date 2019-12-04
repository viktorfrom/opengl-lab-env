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

		class BSTNode
		{
		private:
			BSTNode* Parent;
			BSTNode* Left;
			BSTNode* Middle;
			BSTNode* Right;

			glm::vec2 c, ci, cm, cj;
			BSTNode();
			BSTNode(BSTNode* Parent, BSTNode* Left, BSTNode* Middle, BSTNode* Right, glm::vec2 c, glm::vec2 ci, glm::vec2 cm, glm::vec2 cj);
		};


		std::vector<glm::vec2> vecArr;
		std::vector<glm::vec2> hull;
		std::vector<glm::vec2> buf;
		std::vector<glm::vec2> pointsInsideHull;
		std::vector<glm::vec2> test;

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

		void ExampleApp::calcPointsInsideHull(std::vector<glm::vec2> &inputVector, std::vector<glm::vec2> &inputVector2);

		void ExampleApp::triangulation(glm::vec2 randomPoint, std::vector<glm::vec2> &inputVector, BSTNode* parent);
	};
} // namespace Example