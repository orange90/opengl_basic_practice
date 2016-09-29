#include <stdlib.h>;
#include<GL\glui.h>;
#include "Vector3.h";
#include "Vector2.h";
#include "TriangleFaceData.h";
#define POINT_MODE 0
#define LINE_MODE 1
#define FLAT_SHADING 2
#define SMOOTH_SHADING 3

using namespace std;

class MFileParser
{
public:
	MFileParser();
	~MFileParser();
	void setmode(int mode);
	bool parserFile(char* path);
	void display();
private:
	int m_render_mode;
	std::vector< unsigned int > vertexIndices, uvIndices, normalIndices;
	std::vector<Vector3> *m_vertices;
	std::vector<TriangleFaceData> *m_faces;
	std::vector<Vector2> temp_uvs;
	std::vector<Vector3> temp_normals;
};