// Include standard headers
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <Windows.h>
#include "SerialClass.h"	

#include <iostream>     
#include <thread>     
#include <chrono> 

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <glfw3.h>
GLFWwindow* window;

// Include GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/norm.hpp>
using namespace glm;

// Include AntTweakBar
#include <AntTweakBar.h>


#include <common/shader.hpp>
#include <common/texture.hpp>
#include <common/controls.hpp>
#include <common/objloader.hpp>
#include <common/vboindexer.hpp>
#include <common/quaternion_utils.hpp> // See quaternion_utils.cpp for RotationBetweenVectors, LookAt and RotateTowards


//int SCREENWIDTH = 350;
//int SCREENHEIGHT = 250;
int SCREENWIDTH = 128;
int SCREENHEIGHT = 128;


float CUBE_DISTANCE=0.6f;
float SPEED = 40;


glm::vec3 leftEye = vec3(3, 1.0, 0.12);

glm::vec3 rightEye = vec3(3, 1.0, -0.12);

glm::vec3 lookAtPoint = vec3(0, 0, 0);

//vec3 gPosition1(-1.5f, 0.0f, 0.0f);
vec3 gOrientation1;
int BUFFER_SIZE = 0;
int SMALL_BUFFER_SIZE = 0;

int calcBytePos(int x, int y,int width, int height);
void mergeOutput(unsigned char  *image);
void calcDifferentPixels(unsigned char *previousFrame, unsigned char * currentFrame);
void sendPixel(int x, int y, unsigned char * pixelStart);

unsigned char *imageBytesB1;
unsigned char *imageBytesB2;

Serial* SP;


// Our vertices. Tree consecutive floats give a 3D vertex; Three consecutive vertices give a triangle.
// A cube has 6 faces with 2 triangles each, so this makes 6*2=12 triangles, and 12*3 vertices
static const GLfloat g_vertex_buffer_data[] = {
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f
};


static const GLfloat g_color_buffer_data[] = {
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	0.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 0.0f,
	0.0f, 0.8f, 0.8f,
	0.0f, 0.8f, 0.8f,
	0.0f, 0.8f, 0.8f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 0.8f, 0.8f,
	0.0f, 0.8f, 0.8f,
	0.0f, 0.8f, 0.8f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	1.0f, 0.0f, 0.0f,
	0.0f, 0.2f, 0.8f,
	0.0f, 0.2f, 0.8f,
	0.0f, 0.2f, 0.8f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	1.0f, 0.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
	0.0f, 1.0f, 1.0f,
};



int main( void )
{
	// Initialise GLFW
	if( !glfwInit() )
	{
		fprintf( stderr, "Failed to initialize GLFW\n" );
		getchar();
		return -1;
	}

	SP = new Serial("\\\\.\\COM20");    // adjust as needed
	std::this_thread::sleep_for(std::chrono::seconds(5));

	if (SP->IsConnected())
		printf("We're connected");

	unsigned char rgbPixel[3];
	rgbPixel[0] = 250;
	rgbPixel[1] = 0;
	rgbPixel[2] = 0;

	BUFFER_SIZE = (SCREENWIDTH * 2)*SCREENHEIGHT * 3;
	SMALL_BUFFER_SIZE = SCREENWIDTH*SCREENHEIGHT * 3;

	imageBytesB1 = new unsigned char[BUFFER_SIZE];
	imageBytesB2 = new unsigned char[BUFFER_SIZE];

	glfwWindowHint(GLFW_SAMPLES, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Open a window and create its OpenGL context
	window = glfwCreateWindow( SCREENWIDTH*2, SCREENHEIGHT, "Tutorial 04 - Colored Cube", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Dark blue background
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// Enable depth test
	glEnable(GL_DEPTH_TEST);
	// Accept fragment if it closer to the camera than the former one
	glDepthFunc(GL_LESS); 

	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);

	// Create and compile our GLSL program from the shaders
	GLuint programID = LoadShaders( "TransformVertexShader.vertexshader", "ColorFragmentShader.fragmentshader" );

	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");
	GLuint ViewMatrixID = glGetUniformLocation(programID, "V");
	GLuint ModelMatrixID = glGetUniformLocation(programID, "M");

	// Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
	glm::mat4 ProjectionMatrix = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);
	// Camera matrix
	glm::mat4 ViewMatrix = glm::lookAt(
		glm::vec3(4, 3 , -3), // Camera is at (4,3,-3), in World Space
		glm::vec3(0, 0, 0), // and looks at the origin
		glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	// Model matrix : an identity matrix (model will be at the origin)
	glm::mat4 Model      = glm::mat4(1.0f);
	// Our ModelViewProjection : multiplication of our 3 matrices
	glm::mat4 MVP        = ProjectionMatrix * ViewMatrix * Model; // Remember, matrix multiplication is the other way around

	GLuint vertexbuffer;
	glGenBuffers(1, &vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_vertex_buffer_data), g_vertex_buffer_data, GL_STATIC_DRAW);

	GLuint colorbuffer;
	glGenBuffers(1, &colorbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_color_buffer_data), g_color_buffer_data, GL_STATIC_DRAW);

	// For speed computation
	double lastTime = glfwGetTime();
	double lastFrameTime = lastTime;
	int nbFrames = 0;

	float xoff = 0;
	float yoff = 0;
	int i = 0;
	int change = 1;

	glViewport(0, 0, 60, 60);
	glEnable(GL_SCISSOR_TEST);
	
	bool left = true;

	glm::mat4 RotationMatrix;
	glm::mat4 TranslationMatrix;
	glm::mat4 ScalingMatrix;

	bool buffer1 = true;

	do{
		if (left)
		{
			glViewport(0, 0, SCREENWIDTH, SCREENHEIGHT);
			glScissor(0, 0, SCREENWIDTH, SCREENHEIGHT);
		}
		else
		{
			glViewport(SCREENWIDTH, 0, SCREENWIDTH, SCREENHEIGHT);
			glScissor(SCREENWIDTH, 0, SCREENWIDTH, SCREENHEIGHT);
		}

		// Measure speed
		double currentTime = glfwGetTime();
		float deltaTime = (float)(currentTime - lastFrameTime);
		lastFrameTime = currentTime;
		nbFrames++;

		if (left)
		{
			ViewMatrix = glm::lookAt(
			leftEye, // Camera is at...
			lookAtPoint, // and looks at...
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);

			//rotate only once , then render left and right
			// As an example, rotate arount the vertical axis at 180°/sec
			gOrientation1.y += 3.14159f / SPEED * deltaTime;

			// Build the model matrix
			RotationMatrix = eulerAngleYXZ(gOrientation1.y, gOrientation1.x, gOrientation1.z);
			TranslationMatrix = translate(mat4(), vec3(0.0f, 0.0f, 0.0f)); // A bit to the left
			ScalingMatrix = scale(mat4(), vec3(CUBE_DISTANCE, CUBE_DISTANCE, CUBE_DISTANCE));

		}
		else{
		ViewMatrix = glm::lookAt(
			rightEye, // Camera is at ...
			lookAtPoint, // and looks at ...
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
			);
		}

		// Our ModelViewProjection : multiplication of our 3 matrices
		 glm::mat4 ModelMatrix = TranslationMatrix * RotationMatrix * ScalingMatrix;
		 glm::mat4 MVP = ProjectionMatrix * ViewMatrix * ModelMatrix;

		// Clear the screen
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use our shader
		glUseProgram(programID);

		// Send our transformation to the currently bound shader, 
		// in the "MVP" uniform
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID, 1, GL_FALSE, &ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID, 1, GL_FALSE, &ViewMatrix[0][0]);

		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);

		// 2nd attribute buffer : colors
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
		glVertexAttribPointer(
			1,                                // attribute. No particular reason for 1, but must match the layout in the shader.
			3,                                // size
			GL_FLOAT,                         // type
			GL_FALSE,                         // normalized?
			0,                                // stride
			(void*)0                          // array buffer offset
		);

		// Draw the triangle !
		glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles

		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();

		if (left)
		{
			left = false;
		}
		else
		{
			if (buffer1)
			{
				buffer1 = false;
				glReadPixels(0, 0, SCREENWIDTH * 2, SCREENHEIGHT, GL_RGB, GL_UNSIGNED_BYTE, imageBytesB1);
				mergeOutput(imageBytesB1);
				calcDifferentPixels(imageBytesB2,imageBytesB1);
			}
			else
			{
				buffer1 = true;
				glReadPixels(0, 0, SCREENWIDTH * 2, SCREENHEIGHT, GL_RGB, GL_UNSIGNED_BYTE, imageBytesB2);
				mergeOutput(imageBytesB2);
				calcDifferentPixels(imageBytesB1, imageBytesB2);
			}
			left = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
		{
			while (true)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
				if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
				{
					break;
				}
			} 
		}

	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS &&
		   glfwWindowShouldClose(window) == 0 );

	// Cleanup VBO and shader
	glDeleteBuffers(1, &vertexbuffer);
	glDeleteBuffers(1, &colorbuffer);
	glDeleteProgram(programID);
	glDeleteVertexArrays(1, &VertexArrayID);

	SP->~Serial();
	// Close OpenGL window and terminate GLFW
	glfwTerminate();
	std::this_thread::sleep_for(std::chrono::seconds(100));

	return 0;
}


int calcBytePos(int x, int y,int width, int height)
{
	return 3*(y*width+x);
}

void mergeOutput(unsigned char *image)
{
	int targetx = 1;
	for (int i1 = 0; i1 < SCREENHEIGHT; i1++)
	{
		for (int i2 = SCREENWIDTH + 1; i2 < SCREENWIDTH * 2; i2 += 2)
		{
			int target = calcBytePos(targetx, i1, SCREENWIDTH * 2, SCREENHEIGHT);
			int source = calcBytePos(i2, i1, SCREENWIDTH * 2, SCREENHEIGHT);
			image[target] = image[source];
			image[target + 1] = image[source + 1];
			image[target + 2] = image[source + 2];

			targetx += 2;
		}
		targetx = 1;
	}
}

void calcDifferentPixels(unsigned char *previousFrame, unsigned char * currentFrame)
{
	int differentCounter = 0;
	for (int x = 0; x < SCREENWIDTH; x++)
	{
		for (int y = 0; y < SCREENHEIGHT; y++)
		{
			int bytePos = calcBytePos(x,y,SCREENWIDTH*2,SCREENHEIGHT);
			if (!(previousFrame[bytePos] == currentFrame[bytePos] && previousFrame[bytePos + 1] == currentFrame[bytePos + 1] && previousFrame[bytePos + 2] == currentFrame[bytePos + 2]))
			{
				differentCounter++;
				sendPixel( x,(y*-1)+SCREENHEIGHT, &currentFrame[bytePos]);
			}
		}
	}
	fprintf(stdout, "Different Pixels %d\n",differentCounter);
}


void sendPixel(int x, int y, unsigned char * pixelStart)
{
	unsigned char serialBufferOut[4];
	unsigned char serialBufferIn[4];
	serialBufferOut[0] = (unsigned char)x;
	serialBufferOut[1] = (unsigned char)y;
	
	unsigned short rgb;
	rgb = ((pixelStart[0]&0xF8) << 8) | ((pixelStart[1]&0xFC) << 3) | (pixelStart[2]>>3);
	serialBufferOut[2] = rgb&0xFF;
	serialBufferOut[3] = (rgb>>8)&0xFF;

	SP->WriteData((char*)&serialBufferOut[0], 4);
	
	SP->ReadData((char*)&serialBufferIn, 1);
}