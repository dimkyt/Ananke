#ifndef _Mesh_
#define _Mesh_

#include "lib3d\TriangleGroup.h"
#include "lib3d\Material.h"
#include "lib3d\Texture.h"

// System includes
#include <GL\glew.h>
#include <glm\glm.hpp>
#include <string>
#include <vector>
#include <map>

using namespace std;

class Mesh
{

public:
	vector<glm::vec4> vertices;
	vector<glm::vec3> normals;
	vector<glm::vec3> texcoords;
	vector<TriangleGroup> groups;
	vector<Material> materials;
	map<string, Texture> textures;

	//Used for indexed rendering
	vector<glm::vec4> indexed_vertices;
	vector<glm::vec3> indexed_normals;
	vector<glm::vec3> indexed_texcoords;
 
	vector<unsigned int> indices;

	GLuint vaoId,
		   vboId,
		   nboId,
		   tboId,
		   iboId;

	unsigned int numVertices;
	unsigned int numNormals;
	unsigned int numTex;
	unsigned int numGroups;
	unsigned int numFaces;
	unsigned int numMaterials;
	unsigned int numTextures;


	Mesh(const string& filename);
	~Mesh(void);

	void objLoader(const string& filename);
	void mtlLoad(const string& filename);
	void indexAttributes();						//align attributes for indexed drawing
	void createBuffers();
	void destroyBuffers();
};

#endif
