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

#include <cmath>
#include <vector>
#include <glm\glm.hpp>
class Torus
{
private:
	int numVertices;
	int numIndices;
	int prec;
	float inner;
	float outer;
	std::vector<int> indices;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> texCoords;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec3> sTangents;
	std::vector<glm::vec3> tTangents;
	void init();
	float toRadians(float degrees);

public:
	Torus();
	Torus(float inner, float outer, int prec);
	int getNumVertices();
	int getNumIndices();
	std::vector<int> getIndices();
	std::vector<glm::vec3> getVertices();
	std::vector<glm::vec2> getTexCoords();
	std::vector<glm::vec3> getNormals();
	std::vector<glm::vec3> getStangents();
	std::vector<glm::vec3> getTtangents();
};