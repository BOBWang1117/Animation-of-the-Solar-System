/*
* Team Work
* Student Name:Wang Hong Bo
* Student ID:18098532-I011-0018
* 
* Student Name:Liu Jun Feng
* Student ID:18098537-I011-0067
* 
* Data:2020/12/12
*/

#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <SOIL2\soil2.h>
#include <string>
#include <iostream>
#include <fstream>
#include <cmath>
#include <stack>
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp> // glm::value_ptr
#include <glm\gtc\matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale, glm::perspective
#include "Sphere.h"
#include "Utils.h"
#include "Torus.h"
using namespace std;

float toRadians(float degrees) { return (degrees * 2.0f * 3.14159f) / 360.0f; }

#define numVAOs 1
#define numVBOs 7

float cameraX, cameraY, cameraZ;
float sphLocX, sphLocY, sphLocZ;
float torLocX, torLocY, torLocZ;
float lastTime = 0.0f;
GLuint renderingProgram;
GLuint renderingProgramColorful;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];
/*
plane
*/
GLuint sunTexture;		float sunSize;		float sunRevolutionAroundRadius;		float sunRevolutionAroundSpeed;		float sunSelfAroundSpeed;
GLuint skyTexture;
GLuint mercuryTexture;	float mercurySize;	float mercuryRevolutionAroundRadius;	float mercuryRevolutionAroundSpeed;	float mercurySelfAroundSpeed;
GLuint venusTexture;	float venusSize;	float venusRevolutionAroundRadius;		float venusRevolutionAroundSpeed;	float venusSelfAroundSpeed;
GLuint earthTexture;	float earthSize;	float earthRevolutionAroundRadius;		float earthRevolutionAroundSpeed;	float earthSelfAroundSpeed;
GLuint moonTexture;		float moonSize;		float moonRevolutionAroundRadius;		float moonRevolutionAroundSpeed;	float moonSelfAroundSpeed;
GLuint marsTexture;		float marsSize;		float marsRevolutionAroundRadius;		float marsRevolutionAroundSpeed;	float marsSelfAroundSpeed;
GLuint jupiterTexture;	float jupiterSize;	float jupiterRevolutionAroundRadius;	float jupiterRevolutionAroundSpeed;	float jupiterSelfAroundSpeed;
GLuint saturnTexture;	float saturnSize;	float saturnRevolutionAroundRadius;		float saturnRevolutionAroundSpeed;	float saturnSelfAroundSpeed;
GLuint uranusTexture;	float uranusSize;	float uranusRevolutionAroundRadius;		float uranusRevolutionAroundSpeed;	float uranusSelfAroundSpeed;
GLuint neptuneTexture;	float neptuneSize;	float neptuneRevolutionAroundRadius;	float neptuneRevolutionAroundSpeed;	float neptuneSelfAroundSpeed;
GLuint plutoTexture;	float plutoSize;	float plutoRevolutionAroundRadius;		float plutoRevolutionAroundSpeed;	float plutoSelfAroundSpeed;



float rotAmt = 0.0f;
GLfloat yaw = -90.0f;
GLfloat pitch = 0.0f;

// variable allocation for display
GLfloat lastX;
GLfloat lastY;
GLuint mvLoc, projLoc,nLoc;
GLuint mvLocColorful, projLocColorful;
int width, height;
float aspect;
//glm::mat4 pMat, vMat, mMat, mvMat;
bool firstMouse = true;
stack<glm::mat4> mvStack;

glm::vec3 initialLightLoc = glm::vec3(0.0f, 0.0f, 0.0f);

Sphere mySphere = Sphere(48);
Torus myTorus(1.5f, 0.05f, 48);

glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 32.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

// variable allocation for display
GLuint globalAmbLoc, ambLoc, diffLoc, specLoc, posLoc, mAmbLoc, mDiffLoc, mSpecLoc, mShiLoc;
glm::mat4 pMat, vMat, mMat, mvMat, invTrMat, rMat;
glm::vec3 currentLightPos, lightPosV;
float lightPos[3];

// white light
float globalAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightAmbient[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
float lightDiffuse[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightSpecular[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

float* matAmb = Utils::silverAmbient();
float* matDif = Utils::silverDiffuse();
float* matSpe = Utils::silverSpecular();
float matShi = Utils::silverShininess();

void installLights(glm::mat4& vMatrix) {
	lightPosV = glm::vec3(vMatrix * glm::vec4(currentLightPos, 1.0));
	lightPos[0] = lightPosV.x;
	lightPos[1] = lightPosV.y;
	lightPos[2] = lightPosV.z;

	// get the locations of the light and material fields in the shader
	globalAmbLoc = glGetUniformLocation(renderingProgram, "globalAmbient");
	ambLoc = glGetUniformLocation(renderingProgram, "light.ambient");
	diffLoc = glGetUniformLocation(renderingProgram, "light.diffuse");
	specLoc = glGetUniformLocation(renderingProgram, "light.specular");
	posLoc = glGetUniformLocation(renderingProgram, "light.position");
	mAmbLoc = glGetUniformLocation(renderingProgram, "material.ambient");
	mDiffLoc = glGetUniformLocation(renderingProgram, "material.diffuse");
	mSpecLoc = glGetUniformLocation(renderingProgram, "material.specular");
	mShiLoc = glGetUniformLocation(renderingProgram, "material.shininess");


	//  set the uniform light and material values in the shader
	glProgramUniform4fv(renderingProgram, globalAmbLoc, 1, globalAmbient);
	glProgramUniform4fv(renderingProgram, ambLoc, 1, lightAmbient);
	glProgramUniform4fv(renderingProgram, diffLoc, 1, lightDiffuse);
	glProgramUniform4fv(renderingProgram, specLoc, 1, lightSpecular);
	glProgramUniform3fv(renderingProgram, posLoc, 1, lightPos);
	glProgramUniform4fv(renderingProgram, mAmbLoc, 1, matAmb);
	glProgramUniform4fv(renderingProgram, mDiffLoc, 1, matDif);
	glProgramUniform4fv(renderingProgram, mSpecLoc, 1, matSpe);
	glProgramUniform1f(renderingProgram, mShiLoc, matShi);
}

void setupVertices(void) {

	//ball
	std::vector<int> ind = mySphere.getIndices();
	std::vector<glm::vec3> vert = mySphere.getVertices();
	std::vector<glm::vec2> tex = mySphere.getTexCoords();
	std::vector<glm::vec3> norm = mySphere.getNormals();

	std::vector<float> pvalues;
	std::vector<float> tvalues;
	std::vector<float> nvalues;

	int numIndices = mySphere.getNumIndices();
	for (int i = 0; i < numIndices; i++) {
		pvalues.push_back((vert[ind[i]]).x);
		pvalues.push_back((vert[ind[i]]).y);
		pvalues.push_back((vert[ind[i]]).z);
		tvalues.push_back((tex[ind[i]]).s);
		tvalues.push_back((tex[ind[i]]).t);
		nvalues.push_back((norm[ind[i]]).x);
		nvalues.push_back((norm[ind[i]]).y);
		nvalues.push_back((norm[ind[i]]).z);
	}

	glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glBufferData(GL_ARRAY_BUFFER, pvalues.size()*4, &pvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glBufferData(GL_ARRAY_BUFFER, tvalues.size()*4, &tvalues[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glBufferData(GL_ARRAY_BUFFER, nvalues.size()*4, &nvalues[0], GL_STATIC_DRAW);

	//Donuts

	std::vector<int> ind1 = myTorus.getIndices();
	std::vector<glm::vec3> vert1 = myTorus.getVertices();
	std::vector<glm::vec2> tex1 = myTorus.getTexCoords();
	std::vector<glm::vec3> norm1 = myTorus.getNormals();

	std::vector<float> pvalues1;
	std::vector<float> tvalues1;
	std::vector<float> nvalues1;

	for (int i = 0; i < myTorus.getNumVertices(); i++) {
		pvalues1.push_back(vert1[i].x);
		pvalues1.push_back(vert1[i].y);
		pvalues1.push_back(vert1[i].z);
		tvalues1.push_back(tex1[i].s);
		tvalues1.push_back(tex1[i].t);
		nvalues1.push_back(norm1[i].x);
		nvalues1.push_back(norm1[i].y);
		nvalues1.push_back(norm1[i].z);
	}
	/*glGenVertexArrays(1, vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs, vbo);*/

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glBufferData(GL_ARRAY_BUFFER, pvalues1.size() * 4, &pvalues1[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glBufferData(GL_ARRAY_BUFFER, tvalues1.size() * 4, &tvalues1[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[5]);
	glBufferData(GL_ARRAY_BUFFER, nvalues1.size() * 4, &nvalues1[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, ind1.size() * 4, &ind1[0], GL_STATIC_DRAW);
}

void init(GLFWwindow* window) {
	renderingProgram = Utils::createShaderProgram("vertShader.glsl", "fragShader.glsl");
	//renderingProgramColorful = Utils::createShaderProgram("vertShaderColorful.glsl", "fragShaderColorful.glsl");

	cameraX = 0.0f; cameraY = 5.0f; cameraZ = 32.0f;
	torLocX = 0.0f; torLocY = 0.0f; torLocZ = -0.05f;
	//sphLocX = 0.0f; sphLocY = 0.0f; sphLocZ = -1.0f;

	glfwGetFramebufferSize(window, &width, &height);
	aspect = (float)width / (float)height;
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);

	setupVertices();

	sunTexture = Utils::loadTexture("sun.jpg");
	sunSize = 4.266f;
	sunRevolutionAroundRadius = 0.0f;
	sunSelfAroundSpeed = 2.1f;
	sunRevolutionAroundSpeed = 0.0f;

	skyTexture = Utils::loadTexture("sky.jpg");

	mercuryTexture = Utils::loadTexture("mercury.bmp");
	mercurySize = 0.8f;
	mercuryRevolutionAroundRadius = 3.0f + sunSize;
	mercurySelfAroundSpeed = 1.77f;
	mercuryRevolutionAroundSpeed = 1.429f;

	venusTexture = Utils::loadTexture("venus.bmp");
	venusSize = 1.0f;
	venusRevolutionAroundRadius = 5.19f + sunSize;
	venusSelfAroundSpeed = 1.22f;
	venusRevolutionAroundSpeed = 1.103f;

	earthTexture = Utils::loadTexture("earth.bmp");
	earthSize = 1.0f;
	earthRevolutionAroundRadius = 6.2f + sunSize;
	earthSelfAroundSpeed = 4.8f;
	earthRevolutionAroundSpeed = 1.0f;

	moonTexture = Utils::loadTexture("moon.bmp");
	moonSize = 0.4f;
	moonRevolutionAroundRadius = 2.0f;
	moonSelfAroundSpeed = 2.3f;
	moonRevolutionAroundSpeed = 1.912f;

	marsTexture = Utils::loadTexture("mars.bmp");
	marsSize = 0.866f;
	marsRevolutionAroundRadius = 7.9f + sunSize;
	marsSelfAroundSpeed = 4.8f;
	marsRevolutionAroundSpeed = 0.927f;

	jupiterTexture = Utils::loadTexture("jupiter.bmp");
	jupiterSize = 1.857f;
	jupiterRevolutionAroundRadius = 11.7f + sunSize;
	jupiterSelfAroundSpeed = 6.1f;
	jupiterRevolutionAroundSpeed = 0.537f;

	saturnTexture = Utils::loadTexture("saturn.bmp");
	saturnSize = 1.771f;
	saturnRevolutionAroundRadius = 14.4f + sunSize;
	saturnSelfAroundSpeed = 6.0f;
	saturnRevolutionAroundSpeed = 0.427f;

	uranusTexture = Utils::loadTexture("uranus.bmp");
	uranusSize = 1.428f;
	uranusRevolutionAroundRadius = 23.2f + sunSize;
	uranusSelfAroundSpeed = 5.2f;
	uranusRevolutionAroundSpeed = 0.329f;

	neptuneTexture = Utils::loadTexture("neptune.bmp");
	neptuneSize = 1.419f;
	neptuneRevolutionAroundRadius = 20.9f + sunSize;
	neptuneSelfAroundSpeed = 5.3f;
	neptuneRevolutionAroundSpeed = 0.278f;

	plutoTexture = Utils::loadTexture("pluto.bmp");
	plutoSize = 1.0f;
	plutoRevolutionAroundRadius = 23.0f + sunSize;
	plutoSelfAroundSpeed = 1.0f;
	plutoRevolutionAroundSpeed = 0.3f;

}

void display(GLFWwindow* window, double currentTime) {

	//change the view
	float currentTime1 = glfwGetTime();
	float deltaTime = float(currentTime1 - lastTime);
	lastTime = currentTime1;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		cameraY += 1.0f * deltaTime * 10;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		cameraY -= 1.0f * deltaTime * 10;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		cameraX -= 1.0f * deltaTime * 10;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		cameraX += 1.0f * deltaTime * 10;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		cameraZ -= 1.0f * deltaTime * 10;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	{
		cameraZ += 1.0f * deltaTime * 10;
	}

	glClear(GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(renderingProgram);

	mvLoc = glGetUniformLocation(renderingProgram, "mv_matrix");
	projLoc = glGetUniformLocation(renderingProgram, "proj_matrix");
	nLoc = glGetUniformLocation(renderingProgram, "norm_matrix");

	vMat = glm::translate(glm::mat4(1.0f), glm::vec3(-cameraX, -cameraY, -cameraZ));
	mvStack.push(vMat);

	currentLightPos = glm::vec3(initialLightLoc.x, initialLightLoc.y, initialLightLoc.z);

	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(pMat));
	installLights(vMat);

	cameraPos = glm::vec3(cameraX, cameraY, cameraZ);
	vMat = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	mMat = glm::translate(glm::mat4(1.0f), glm::vec3(torLocX, torLocY, torLocZ));
	mMat = glm::rotate(mMat, toRadians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	mvStack.push(vMat);


	// ----------------------  sun
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)currentTime, glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(2.0, 2.0, 2.0));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));


	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, sunTexture);

	//glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	//Sky
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(10.0, 10.0, 10.0));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(0.2 * currentTime), glm::vec3(0.1, 1.0, 0.3));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(40, 40, 40));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, skyTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);


	glDrawArrays(GL_POINTS, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();

	//-----------------------  mercury
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(mercuryRevolutionAroundSpeed * currentTime)) * mercuryRevolutionAroundRadius, 0.0f, cos((float)(mercuryRevolutionAroundSpeed * currentTime)) * mercuryRevolutionAroundRadius));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(10.0f *currentTime), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(mercurySize, mercurySize, mercurySize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mercuryTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();

	//-----------------------  venus
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(venusRevolutionAroundSpeed * currentTime)) * venusRevolutionAroundRadius, 0.0f, cos((float)(venusRevolutionAroundSpeed * currentTime)) * venusRevolutionAroundRadius));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(venusSelfAroundSpeed * currentTime), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(venusSize, venusSize, venusSize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, venusTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();


	//-----------------------  earth  
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(earthRevolutionAroundSpeed * currentTime)) * earthRevolutionAroundRadius, 0.0f, cos((float)(earthRevolutionAroundSpeed * currentTime)) * earthRevolutionAroundRadius));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)((earthSelfAroundSpeed * currentTime)), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(earthSize, earthSize, earthSize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, earthTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	//mvStack.pop();

	//-----------------------  moon

	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(moonRevolutionAroundSpeed * currentTime)) * moonRevolutionAroundRadius, 0.0f, cos((float)(moonRevolutionAroundSpeed * currentTime)) * moonRevolutionAroundRadius));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)((moonSelfAroundSpeed * currentTime)), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(moonSize, moonSize, moonSize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, moonTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	//glEnd();

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();
	mvStack.pop();
	//-----------------------  mars  
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(marsRevolutionAroundSpeed * currentTime)) * marsRevolutionAroundRadius, 0.0f, cos((float)(marsRevolutionAroundSpeed * currentTime)) * marsRevolutionAroundRadius));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(marsSelfAroundSpeed * currentTime), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(marsSize, marsSize, marsSize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, marsTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();

	//-----------------------  jupiter
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(jupiterRevolutionAroundSpeed * currentTime)) * jupiterRevolutionAroundRadius, 0.0f, cos((float)(jupiterRevolutionAroundSpeed * currentTime)) * jupiterRevolutionAroundRadius));
	//mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(jupiterSelfAroundSpeed*currentTime), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(jupiterSize, jupiterSize, jupiterSize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, jupiterTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	//mvStack.pop();
	mvStack.pop();

	

	//-----------------------  saturn
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(saturnRevolutionAroundSpeed * currentTime)) * saturnRevolutionAroundRadius, 0.0f, cos((float)(saturnRevolutionAroundSpeed * currentTime)) * saturnRevolutionAroundRadius));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(saturnSelfAroundSpeed * currentTime), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(saturnSize, saturnSize, saturnSize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, saturnTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	//mvStack.pop();

	//-----------------------  Donuts

	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, sin(3.0f), 0.0f));
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(10 * currentTime), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(2.0f, 2.0f, 2.0f));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));

	glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, saturnTexture);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CCW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[6]);
	glDrawElements(GL_TRIANGLES, myTorus.getIndices().size(), GL_UNSIGNED_INT, 0);
	mvStack.pop();
	mvStack.pop();
	//mvStack.pop();

	//-----------------------  uranus
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(uranusRevolutionAroundSpeed * currentTime)) * uranusRevolutionAroundRadius, 0.0f, cos((float)(uranusRevolutionAroundSpeed * currentTime)) * uranusRevolutionAroundRadius));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(uranusSelfAroundSpeed * currentTime), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(uranusSize, uranusSize, uranusSize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, uranusTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();


	//-----------------------  neptune
	mvStack.push(mvStack.top());
	mvStack.top() *= glm::translate(glm::mat4(1.0f), glm::vec3(sin((float)(neptuneRevolutionAroundSpeed * currentTime)) * neptuneRevolutionAroundRadius, 0.0f, cos((float)(neptuneRevolutionAroundSpeed * currentTime)) * neptuneRevolutionAroundRadius));
	mvStack.push(mvStack.top());
	mvStack.top() *= rotate(glm::mat4(1.0f), (float)(neptuneSelfAroundSpeed * currentTime), glm::vec3(0.0, 1.0, 0.0));
	mvStack.top() *= scale(glm::mat4(1.0f), glm::vec3(neptuneSize, neptuneSize, neptuneSize));

	glUniformMatrix4fv(mvLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	mvStack.top() = glm::transpose(glm::inverse(mvStack.top()));
	glUniformMatrix4fv(nLoc, 1, GL_FALSE, glm::value_ptr(mvStack.top()));
	glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, 0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, neptuneTexture);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_DEPTH_TEST);
	glFrontFace(GL_CCW);
	glDepthFunc(GL_LEQUAL);

	glDrawArrays(GL_TRIANGLES, 0, mySphere.getNumIndices());
	mvStack.pop();
	mvStack.pop();

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos; // Reversed since y-coordinates go from bottom to left
	lastX = xpos;
	lastY = ypos;

	GLfloat sensitivity = 0.08; // Change this value to your liking
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front);
}

void window_size_callback(GLFWwindow* win, int newWidth, int newHeight) {
	aspect = (float)newWidth / (float)newHeight;
	glViewport(0, 0, newWidth, newHeight);
	pMat = glm::perspective(1.0472f, aspect, 0.1f, 1000.0f);
}

int main(void) {
	if (!glfwInit()) { exit(EXIT_FAILURE); }
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	GLFWwindow* window = glfwCreateWindow(600, 600, "Solar System", NULL, NULL);
	glfwMakeContextCurrent(window);
	glfwSetCursorPosCallback(window, mouse_callback);
	if (glewInit() != GLEW_OK) { exit(EXIT_FAILURE); }
	glfwSwapInterval(1);

	glfwSetWindowSizeCallback(window, window_size_callback);

	init(window);

	while (!glfwWindowShouldClose(window)) {
		display(window, glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}