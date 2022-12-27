/*
CPE/CSC 471 Lab base code Wood/Dunn/Eckhardt

The given base code includes all code except: keyCallback, initGeom, init, and render
*/

#include <iostream>
#include <glad/glad.h>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"

#include "Human.h"
#include "Enemy.h"
#include "Platform.h"

#include "WindowManager.h"
#include "Shape.h"
// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
using namespace std;
using namespace glm;

shared_ptr<Shape> shape;

double get_last_elapsed_time()
{
	static double lasttime = glfwGetTime();
	double actualtime =glfwGetTime();
	double difference = actualtime- lasttime;
	lasttime = actualtime;
	return difference;
}
class camera
{
public:
	glm::vec3 pos, rot;
	int w, a, s, d;
	camera()
	{
		w = a = s = d = 0;
		rot = glm::vec3(0, 0, 0);
		pos = glm::vec3(0, 0, -5);
	}
	glm::mat4 process(double ftime)
	{
		float speed = 0;
		if (w == 1)
		{
			speed = 1*ftime;
		}
		else if (s == 1)
		{
			speed = -1*ftime;
		}
		float yangle=0;
		if (a == 1)
			yangle = -1*ftime;
		else if(d==1)
			yangle = 1*ftime;
		rot.y += yangle;
		glm::mat4 R = glm::rotate(glm::mat4(1), rot.y, glm::vec3(0, 1, 0));
		glm::vec4 dir = glm::vec4(0, 0, speed,1);
		dir = dir*R;
		pos += glm::vec3(dir.x, dir.y, dir.z);
		glm::mat4 T = glm::translate(glm::mat4(1), pos);
		return R*T;
	}
};

camera mycam;

class Application : public EventCallbacks
{

public:
	int left = 0, right = 0, jump = 0, punch = 0, kick = 0;
	WindowManager * windowManager = nullptr;

	// Our shader program
	std::shared_ptr<Program> prog, enemyProg, bossProg, platProg, psky;
	Human* testHuman;
	Enemy *testEnemy, * testEnemy2, * testEnemy3;
	Platform *testPlatform1, *testPlatform2, *bottom, *top, *tall, *arena;


	// Contains vertex information for OpenGL
	GLuint VertexArrayID;

	// Data necessary to give our box to OpenGL
	GLuint VertexBufferID, VertexColorIDBox, IndexBufferIDBox;

	//texture data
	GLuint Texture;
	GLuint Texture2;
	GLuint Texture3;

	void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
	{
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		{
			glfwSetWindowShouldClose(window, GL_TRUE);
		}
		
		if (key == GLFW_KEY_W && action == GLFW_PRESS)
		{
			jump = 1;
		}
		if (key == GLFW_KEY_W && action == GLFW_RELEASE)
		{
			jump = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_A && action == GLFW_PRESS)
		{
			left = 1;
		}
		if (key == GLFW_KEY_A && action == GLFW_RELEASE)
		{
			left = 0;
		}
		if (key == GLFW_KEY_D && action == GLFW_PRESS)
		{
			right = 1;
		}
		if (key == GLFW_KEY_D && action == GLFW_RELEASE)
		{
			right = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_PRESS)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_S && action == GLFW_RELEASE)
		{
			mycam.s = 0;
		}
		if (key == GLFW_KEY_P && action == GLFW_PRESS)
		{
			punch = 1;
		}
		if (key == GLFW_KEY_P && action == GLFW_RELEASE)
		{
			punch = 0;
		}
		if (key == GLFW_KEY_K && action == GLFW_PRESS)
		{
			kick = 1;
		}
		if (key == GLFW_KEY_K && action == GLFW_RELEASE)
		{
			kick = 0;
		}
	}

	// callback for the mouse when clicked move the triangle when helper functions
	// written
	void mouseCallback(GLFWwindow *window, int button, int action, int mods)
	{

	}

	//if the window is resized, capture the new size and reset the viewport
	void resizeCallback(GLFWwindow *window, int in_width, int in_height)
	{
		//get the window size - may be different then pixels for retina
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}

	/*Note that any gl calls must always happen after a GL state is initialized */
	void initGeom()
	{
		string resourceDirectory = "../resources";

		shape = make_shared<Shape>();
		shape->loadMesh(resourceDirectory + "/sphere.obj");
		shape->resize();
		shape->init();

		//generate the VAO
		glGenVertexArrays(1, &VertexArrayID);
		glBindVertexArray(VertexArrayID);

		//generate vertex buffer to hand off to OGL
		glGenBuffers(1, &VertexBufferID);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ARRAY_BUFFER, VertexBufferID);

		GLfloat cube_vertices[] = {
			// front
			-1.0, -1.0,  1.0,
			1.0, -1.0,  1.0,
			1.0,  1.0,  1.0,
			-1.0,  1.0,  1.0,
			// back
			-1.0, -1.0, -1.0,
			1.0, -1.0, -1.0,
			1.0,  1.0, -1.0,
			-1.0,  1.0, -1.0,
		};

		glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_DYNAMIC_DRAW);

		//we need to set up the vertex array
		glEnableVertexAttribArray(0);
		//key function to get up how many elements to pull out at a time (3)
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glGenBuffers(1, &IndexBufferIDBox);
		//set the current state to focus on our vertex buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBufferIDBox);
		GLushort cube_elements[] = {
			// front
			0, 1, 2,
			2, 3, 0,
			// top
			1, 5, 6,
			6, 2, 1,
			// back
			7, 6, 5,
			5, 4, 7,
			// bottom
			4, 0, 3,
			3, 7, 4,
			// left
			4, 5, 1,
			1, 0, 4,
			// right
			3, 2, 6,
			6, 7, 3,
		};
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_elements), cube_elements, GL_STATIC_DRAW);

		//assign cube info to Humans
		testHuman->init(VertexArrayID, IndexBufferIDBox, 36);
		testHuman->health = 300;
		testEnemy->init(VertexArrayID, IndexBufferIDBox, 36);
		testEnemy2->init(VertexArrayID, IndexBufferIDBox, 36);
		testEnemy3->init(VertexArrayID, IndexBufferIDBox, 36);
		testEnemy3->health = 250;
		testPlatform1->init(VertexArrayID, IndexBufferIDBox, 36, vec3(10.0, 10.0, 1.5));
		testPlatform2->init(VertexArrayID, IndexBufferIDBox, 36, vec3(10.0, 10.0, 1.5));
		bottom->init(VertexArrayID, IndexBufferIDBox, 36, vec3(30.0, 10.0, 1.5));
		top->init(VertexArrayID, IndexBufferIDBox, 36, vec3(10.0, 20.0, 1.5));
		tall->init(VertexArrayID, IndexBufferIDBox, 36, vec3(10.0, 20.0, 1.5));
		arena->init(VertexArrayID, IndexBufferIDBox, 36, vec3(10.0, 10.0, 1.5));

		testHuman->translate(vec3(-13.0, 0.0, -10.0));
		testEnemy->translate(vec3(13.0, 0.0, -10.0));
		testEnemy2->translate(vec3(105.0, 36.0, -10.0));
		testEnemy3->translate(vec3(130.0, 0.0, -10.0));
		testPlatform1->translate(vec3(-13.0, -13.0, -10.0));
		testPlatform2->translate(vec3(13.0, -13.0, -10.0));
		bottom->translate(vec3(60.0, -13.0, -10.0));
		top->translate(vec3(80.0, 25.0, -10.0));
		tall->translate(vec3(105.0, 13.0, -10.0));
		arena->translate(vec3(130.0, -13.0, -10.0));

		testHuman->enemy[0] = testEnemy;
		testHuman->enemy[1] = testEnemy2;
		testHuman->enemy[2] = testEnemy3;

		testEnemy->enemy[0] = testHuman;
		testEnemy2->enemy[0] = testHuman;
		testEnemy3->enemy[0] = testHuman;

		testHuman->platforms[0] = testPlatform1;
		testHuman->platforms[1] = testPlatform2;
		testHuman->platforms[2] = bottom;
		testHuman->platforms[3] = top;
		testHuman->platforms[4] = tall;
		testHuman->platforms[5] = arena;

		testEnemy->platforms[0] = testPlatform1;
		testEnemy->platforms[1] = testPlatform2;
		testEnemy->platforms[2] = bottom;
		testEnemy->platforms[3] = top;
		testEnemy->platforms[4] = tall;
		testEnemy->platforms[5] = arena;


		testEnemy2->platforms[0] = testPlatform1;
		testEnemy2->platforms[1] = testPlatform2;
		testEnemy2->platforms[2] = bottom;
		testEnemy2->platforms[3] = top;
		testEnemy2->platforms[4] = tall;
		testEnemy2->platforms[5] = arena;

		testEnemy3->platforms[0] = testPlatform1;
		testEnemy3->platforms[1] = testPlatform2;
		testEnemy3->platforms[2] = bottom;
		testEnemy3->platforms[3] = top;
		testEnemy3->platforms[4] = tall;
		testEnemy3->platforms[5] = arena;

		testEnemy->setMovementMode(HUMAN_WALK);

		glBindVertexArray(0);

		int width, height, channels;
		char filepath[1000];

		string str = resourceDirectory + "/nightsky.jpg";
		strcpy(filepath, str.c_str());
		unsigned char* data = stbi_load(filepath, &width, &height, &channels, 4);
		glGenTextures(1, &Texture);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	//General OGL initialization - set OGL state here
	void init(const std::string& resourceDirectory)
	{
		GLSL::checkVersion();

		// Set background color.
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		// Enable z-buffer test.
		glEnable(GL_DEPTH_TEST);

		// Initialize the GLSL program.
		prog = std::make_shared<Program>();
		prog->setVerbose(true);
		prog->setShaderNames(resourceDirectory + "/shader_vertex.glsl", resourceDirectory + "/shader_fragment.glsl");
		if (!prog->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		prog->addUniform("P");
		prog->addUniform("V");
		prog->addUniform("M");
		prog->addAttribute("vertPos");


		enemyProg = std::make_shared<Program>();
		enemyProg->setVerbose(true);
		enemyProg->setShaderNames(resourceDirectory + "/shader_vertex_enemy.glsl", resourceDirectory + "/shader_fragment_enemy.glsl");
		if (!enemyProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		enemyProg->addUniform("P");
		enemyProg->addUniform("V");
		enemyProg->addUniform("M");
		enemyProg->addAttribute("vertPos");

		bossProg = std::make_shared<Program>();
		bossProg->setVerbose(true);
		bossProg->setShaderNames(resourceDirectory + "/shader_vertex_boss.glsl", resourceDirectory + "/shader_fragment_boss.glsl");
		if (!bossProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		bossProg->addUniform("P");
		bossProg->addUniform("V");
		bossProg->addUniform("M");
		bossProg->addAttribute("vertPos");


		platProg = std::make_shared<Program>();
		platProg->setVerbose(true);
		platProg->setShaderNames(resourceDirectory + "/shader_vertex_plat.glsl", resourceDirectory + "/shader_fragment_plat.glsl");
		if (!platProg->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		platProg->addUniform("P");
		platProg->addUniform("V");
		platProg->addUniform("M");
		platProg->addAttribute("vertPos");



		psky = std::make_shared<Program>();
		psky->setVerbose(true);
		psky->setShaderNames(resourceDirectory + "/skyvertex.glsl", resourceDirectory + "/skyfrag.glsl");
		if (!psky->init())
		{
			std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
			exit(1);
		}
		psky->addUniform("P");
		psky->addUniform("V");
		psky->addUniform("M");
		psky->addUniform("campos");
		psky->addAttribute("vertPos");
		psky->addAttribute("vertNor");
		psky->addAttribute("vertTex");

		testHuman = new Human();
		testEnemy = new Enemy();
		testEnemy2 = new Enemy();
		testEnemy3 = new Enemy();
		testPlatform1 = new Platform();
		testPlatform2 = new Platform();
		bottom = new Platform();
		top = new Platform();
		tall = new Platform();
		arena = new Platform();
	}


	/****DRAW
	This is the most important function in your program - this is where you
	will actually issue the commands to draw any geometry you have set up to
	draw
	********/
	void render()
	{
		double frametime = get_last_elapsed_time();
		// Get current frame buffer size.
		int width, height;
		glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
		float aspect = width / (float)height;
		glViewport(0, 0, width, height);

		// Clear framebuffer.
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//check for movement
		if (left)
		{
			testHuman->move(-frametime);
			testHuman->setMovementMode(HUMAN_WALK);
		}
		if (right)
		{
			testHuman->move(frametime);
			testHuman->setMovementMode(HUMAN_WALK);
		}
		if (jump)
			testHuman->setMovementMode(HUMAN_JUMP);
		if (!left && !right && !jump)
			testHuman->setMovementMode(HUMAN_STAND);

		//check for attacks
		if (punch)
			testHuman->setAttackMode(HUMAN_PUNCH);
		if (kick)
			testHuman->setAttackMode(HUMAN_KICK);

		glm::vec3 playerPos = testHuman->getPos();

		mycam.pos.x = -playerPos.x;
		mycam.pos.y = -playerPos.y;

		glm::mat4 V, M, P; //View, Model and Perspective matrix
		V = mycam.process(frametime);
		P = glm::perspective((float)(3.14159 / 4.), (float)((float)width / (float)height), 0.1f, 1000.0f);

		float sangle = 3.1415926 / 2.;
		glm::mat4 RotateXSky = glm::rotate(glm::mat4(1.0f), sangle, glm::vec3(1.0f, 0.0f, 0.0f));
		glm::vec3 camp = -mycam.pos;
		glm::mat4 TransSky = glm::translate(glm::mat4(1.0f), camp);
		glm::mat4 SSky = glm::scale(glm::mat4(1.0f), glm::vec3(0.8f, 0.8f, 0.8f));

		M = TransSky * RotateXSky * SSky;

		psky->bind();

		
		//send the matrices to the shaders
		glUniformMatrix4fv(psky->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(psky->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		glUniformMatrix4fv(psky->getUniform("M"), 1, GL_FALSE, &M[0][0]);
		glUniform3fv(psky->getUniform("campos"), 1, &mycam.pos[0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, Texture);
		glDisable(GL_DEPTH_TEST);
		shape->draw(psky, FALSE);
		glEnable(GL_DEPTH_TEST);

		psky->unbind();

		platProg->bind();
		glUniformMatrix4fv(platProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(platProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		testPlatform1->draw(platProg);
		testPlatform2->draw(platProg);
		bottom->draw(platProg);
		top->draw(platProg);
		tall->draw(platProg);
		arena->draw(platProg);
		platProg->unbind();

		prog->bind();
		glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		testHuman->draw(prog, frametime);
		prog->unbind();

		enemyProg->bind();
		glUniformMatrix4fv(enemyProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(enemyProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);
		testEnemy->update(frametime);
		testEnemy2->update(frametime);

		testEnemy->draw(enemyProg, frametime);
		testEnemy2->draw(enemyProg, frametime);
		enemyProg->unbind();
		
		bossProg->bind();
		glUniformMatrix4fv(bossProg->getUniform("P"), 1, GL_FALSE, &P[0][0]);
		glUniformMatrix4fv(bossProg->getUniform("V"), 1, GL_FALSE, &V[0][0]);

		testEnemy3->update(frametime);
		testEnemy3->draw(bossProg, frametime);

		bossProg->unbind();

	}
};

//******************************************************************************************
int main(int argc, char **argv)
{
	std::string resourceDir = "../resources"; // Where the resources are loaded from
	if (argc >= 2)
	{
		resourceDir = argv[1];
	}

	Application *application = new Application();

	/* your main will always include a similar set up to establish your window
		and GL context, etc. */
	WindowManager * windowManager = new WindowManager();
	windowManager->init(1920, 1080);
	windowManager->setEventCallbacks(application);
	application->windowManager = windowManager;

	/* This is the code that will likely change program to program as you
		may need to initialize or set up different data and state */
	// Initialize scene.
	application->init(resourceDir);
	application->initGeom();

	// Loop until the user closes the window.
	while(! glfwWindowShouldClose(windowManager->getHandle()))
	{
		// Render scene.
		application->render();

		// Swap front and back buffers.
		glfwSwapBuffers(windowManager->getHandle());
		// Poll for and process events.
		glfwPollEvents();
	}

	// Quit program.
	windowManager->shutdown();
	return 0;
}
