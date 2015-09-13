#ifndef _Texture_
#define _Texture_

#include <glm\glm.hpp>
#include <string>

using namespace std;

class Texture
{
public:
	Texture();
	Texture(string fname);
	~Texture(void);

	string getFilename() {return filename;}
	unsigned int getID() {return ID;}
	bool load_BMP();
	bool load_tex();

private:
	string filename;
	unsigned int ID;
};

#endif
