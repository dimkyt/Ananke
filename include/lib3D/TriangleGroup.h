#ifndef _TriangleGroup_
#define _TriangleGroup_

#include <glm\glm.hpp>
#include <vector>

using namespace std;

class TriangleGroup
{
public:
	TriangleGroup();
	~TriangleGroup();

	vector<unsigned int> vertIdx;
	vector<unsigned int> texIdx;
	vector<unsigned int> normIdx;
	unsigned int matIdx;

	bool has_texture,
		 has_normal;

	unsigned int numFaces;
};

#endif
