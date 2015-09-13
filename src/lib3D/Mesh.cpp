#include "lib3d\Mesh.h"

// System includes
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>


Mesh::Mesh(const string& filename)
{
  numVertices = numNormals = numTex = numGroups = numFaces = numTextures = 0;

  objLoader(filename);
  printf("Model loaded!\n\n");

  indexAttributes();
  printf("Mesh Indexed!\n\n");

  createBuffers();
}

Mesh::~Mesh(void)
{
  destroyBuffers();
}

void Mesh::objLoader(const string& filename)
{
  ifstream file;
  string line,
       type;
  bool has_norm = false,
     has_tex = false;
  unsigned int curr_mtl = 0;

  file.open(filename);

  if(!file)
  {
    cerr << "An error occurred when opening the file "<< filename << endl;
    exit(-1);
  }

  while(getline(file, line))
  {
    type = "";
    istringstream stream(line);
    stream >> type;

    if(type == "mtllib")
    {
      printf("Loading materials.\n");

      stream >> type;
      mtlLoad(type);

      printf("Materials loaded!\n\n");
    }
    else if(type == "v")
    {
      glm::vec4 v;
      stream >> v.x >> v.y >> v.z; v.w = 1.0f;
      vertices.push_back(v);
      numVertices++;
      has_norm = has_tex = false;
    }
    else if(type == "vn")
    {
      glm::vec3 n;
      stream >> n.x >> n.y >> n.z;
      normals.push_back(n);
      numNormals++;
      has_norm = true;
    }
    else if(type == "vt")
    {
      glm::vec3 t;
      stream >> t.x >> t.y >> t.z;
      t.x = 1.0f-t.x;					//texture coordinate U needs to be inverted for OpenGL
      texcoords.push_back(t);
      numTex++;
      has_tex = true;
    }
    else if(type == "g")
    {
      if(!groups.empty())			//update previous group
      {
        groups.back().has_normal = has_norm;
        groups.back().has_texture = has_tex;
        groups.back().matIdx = curr_mtl;
      }

      TriangleGroup g;
      groups.push_back(g);
      numGroups++;
    }
    else if(type == "usemtl")
    {
      string name;
      stream >> name;
      for(unsigned int i=0; i<numMaterials; i++)
      {
        if(name == materials[i].name)
        {
          //groups.back().matIdx = i;
          curr_mtl = i;
          break;
        }
      }
    }
    else if(type == "f")
    {
      glm::uvec3 vIdx,
             tIdx,
             nIdx;

      char tmp;
      glm::uvec3 tmp1 = glm::uvec3(1,1,1);

      if(has_norm && has_tex)
      {
        stream >> vIdx.x >> tmp >> tIdx.x >> tmp >> nIdx.x
               >> vIdx.y >> tmp >> tIdx.y >> tmp >> nIdx.y
               >> vIdx.z >> tmp >> tIdx.z >> tmp >> nIdx.z;

        vIdx-=tmp1; tIdx-=tmp1; nIdx-=tmp1;
      }
      else if(!has_norm && has_tex)
      {
        stream >> vIdx.x >> tmp >> tIdx.x
               >> vIdx.y >> tmp >> tIdx.y
               >> vIdx.z >> tmp >> tIdx.z;

        vIdx-=tmp1; tIdx-=tmp1;
      }
      else if(has_norm && !has_tex)
      {
        stream >> vIdx.x >> tmp >> tmp >> nIdx.x
               >> vIdx.y >> tmp >> tmp >> nIdx.y
               >> vIdx.z >> tmp >> tmp >> nIdx.z;

        vIdx-=tmp1; nIdx-=tmp1;
      }
      else if(!has_norm && !has_tex)
      {
        stream >> vIdx.x
               >> vIdx.y
               >> vIdx.z;

        vIdx-=tmp1;
      }

      groups.back().vertIdx.push_back(vIdx.x);
      groups.back().vertIdx.push_back(vIdx.y);
      groups.back().vertIdx.push_back(vIdx.z);
      groups.back().texIdx.push_back(tIdx.x);
      groups.back().texIdx.push_back(tIdx.y);
      groups.back().texIdx.push_back(tIdx.z);
      groups.back().normIdx.push_back(nIdx.x);
      groups.back().normIdx.push_back(nIdx.y);
      groups.back().normIdx.push_back(nIdx.z);
      groups.back().numFaces++;
      numFaces++;
    }
  }

  if(!file.eof())
  {
    cerr << "Error reading the file " << filename << endl;
    exit(-1);
  }
  file.close();


  if(!groups.empty())					//update last group.
  {
    groups.back().has_normal = has_norm;
    groups.back().has_texture = has_tex;
    groups.back().matIdx = curr_mtl;
  }

  //trim vectors
  for(vector<TriangleGroup>::iterator it = groups.begin(); it != groups.end(); it++)
  {
    vector<unsigned int>(it->vertIdx).swap(it->vertIdx);
    vector<unsigned int>(it->texIdx).swap(it->texIdx);
    vector<unsigned int>(it->normIdx).swap(it->normIdx);
  }
  vector<TriangleGroup>(groups).swap(groups);
}

void Mesh::mtlLoad(const string& filename)
{
  ifstream file;
  string line,
       type;

  file.open(filename);

  if(!file)
  {
    cerr << "An error occurred when opening the file "<< filename << endl;
    exit(-1);
  }

  while(getline(file, line))
  {
    type = "";
    istringstream stream(line);
    stream>>type;

    if(type == "newmtl")
    {
      Material m;
      stream >> m.name;
      materials.push_back(m);
      numMaterials++;
    }
    else if(type == "Ka")
    {
      glm::vec3 Ka;
      stream >> Ka.x >> Ka.y >> Ka.z;
      materials.back().ambient = Ka;
    }
    else if(type == "Kd")
    {
      glm::vec3 Kd;
      stream >> Kd.x >> Kd.y >> Kd.z;
      materials.back().diffuse = Kd;
    }
    else if(type == "Ks")
    {
      glm::vec3 Ks;
      stream >> Ks.x >> Ks.y >> Ks.z;
      materials.back().specular = Ks;
    }
    else if(type == "Ke")
    {
      glm::vec3 Ke;
      stream >> Ke.x >> Ke.y >> Ke.z;
      materials.back().emission = Ke;
    }
    else if(type == "Ns")
    {
      float Ns;
      stream >> Ns;
      materials.back().Ns = Ns;
    }
    else if(type == "map_Ka" || type == "map_Kd" || type == "map_Ks"|| type == "map_Ke")
    {
      map<string, Texture>::iterator it;
      unsigned int mapID;
      string tex = "";

      if(type == "map_Ka")
        mapID = MAP_AMBIENT;
      else if(type == "map_Kd")
        mapID = MAP_DIFFUSE;
      else if(type == "map_Ks")
        mapID = MAP_SPECULAR;
      else if(type == "map_Ke")
        mapID = MAP_EMISSION;

      materials.back().has_texture[mapID] = true;
      stream >> tex;

      it = textures.find(tex);
      if(it == textures.end())		//Texture is not in map.
      {
        textures.insert(pair<string, Texture>(tex, Texture(tex)));	//Insert a new Texture object in map.

        //Get file type
        unsigned tmp = tex.find(".");
        string tex_type = tex.substr(tmp+1, string::npos);

        //load texture
        bool check;
        if(tex_type != "bmp")
          check = textures[tex].load_tex();
        else
          check = textures[tex].load_BMP();

        if(check)
        {
          materials.back().textureID[mapID] = textures[tex].getID();
          numTextures++;
        }
        else
        {
          textures.erase(tex);								//in case of error delete from map
        }
      }
      else
        materials.back().textureID[mapID] = it->second.getID();
    }
  }

  if(!file.eof())
  {
    cerr << "Error reading the file " << filename << endl;
    exit(-1);
  }
  file.close();

  //trim vector
  vector<Material>(materials).swap(materials);
}

void Mesh::indexAttributes()
{
  map<string, unsigned int> uniques;
  map<string, unsigned int>::iterator it;
  string key = "";

  for(unsigned int i=0; i<numGroups; i++)
  {
    for(unsigned int j=0; j<groups[i].numFaces; j++)
    {
      for(int k=0; k<3; k++)
      {
        //OBJ face format v/t/n
        unsigned int idx = 3*j+k;
        char tmp[30];
        sprintf(tmp, "%u%u%u", groups[i].vertIdx[idx],  groups[i].texIdx[idx],  groups[i].normIdx[idx]);
        key = tmp;
        
        it = uniques.find(key);
        if(it == uniques.end())	//vertex is not in map
        {
          indexed_vertices.push_back(vertices[groups[i].vertIdx[idx]]);
          indexed_texcoords.push_back(texcoords[groups[i].texIdx[idx]]);
          indexed_normals.push_back(normals[groups[i].normIdx[idx]]);
          unsigned int index = indexed_vertices.size()-1;
          indices.push_back(index);
          uniques[key] = index;
        }
        else	//vertex is in map
        {
          indices.push_back(it->second);
        }
      }
    }
  }

  //trim vector
  vector<unsigned int>(indices).swap(indices);
}

void Mesh::createBuffers()
{
  GLenum ErrorCheckValue = glGetError();
     
  glGenVertexArrays(1, &vaoId);
  glBindVertexArray(vaoId);
 
  //vertex buffer
  glGenBuffers(1, &vboId);
  glBindBuffer(GL_ARRAY_BUFFER, vboId);
  glBufferData(GL_ARRAY_BUFFER, indexed_vertices.size()*sizeof(glm::vec4), &indexed_vertices[0], GL_STATIC_DRAW);
  glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0);
  
  //normal buffer
  glGenBuffers(1, &nboId);
  glBindBuffer(GL_ARRAY_BUFFER, nboId);
  glBufferData(GL_ARRAY_BUFFER, indexed_normals.size()*sizeof(glm::vec3), &indexed_normals[0], GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //texture buffer
  glGenBuffers(1, &tboId);
  glBindBuffer(GL_ARRAY_BUFFER, tboId);
  glBufferData(GL_ARRAY_BUFFER, indexed_texcoords.size()*sizeof(glm::vec3), &indexed_texcoords[0], GL_STATIC_DRAW);
  glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

  //index buffer
  glGenBuffers(1, &iboId);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
 

  ErrorCheckValue = glGetError();
  if (ErrorCheckValue != GL_NO_ERROR)
  {
    fprintf(
      stderr,
      "ERROR: Could not create a buffer: %s \n",
      gluErrorString(ErrorCheckValue)
    );
 
    exit(-1);
  }

  printf("OpenGL buffers created!\n");
}

void Mesh::destroyBuffers()
{
  glDeleteBuffers(1, &vboId);
  glDeleteBuffers(1, &nboId);
  glDeleteBuffers(1, &tboId);
  glDeleteBuffers(1, &iboId);
}