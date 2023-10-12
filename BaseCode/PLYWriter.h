#ifndef PLYWRITER_H
#define PLYWRITER_H


#include <fstream>
#include "TriangleMesh.h"


using namespace std;


// Class used to read PLY files into objects of the TriangleMesh class
// Currently it can only process very specific PLY files in binary format

class PLYWriter
{

public:
	static bool writeMesh(const string& filename, vector<glm::vec3*> &representatives, vector<int> &triangle_representatives);

private:
	static bool writeHeader(ofstream& fout, int& nVertices, int& nFaces);
	static void writeVertices(ofstream& fout, vector<glm::vec3*>& representatives);
	static void writeFaces(ofstream& fout, vector<int>& triangle_representatives);
	static void rescaleModel(vector<float>& plyVertices);
	static void addModelToMesh(const vector<float>& plyVertices, const vector<int>& plyTriangles, TriangleMesh& mesh);

};

#endif // PLYWRITER_H
