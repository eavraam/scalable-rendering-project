#include <iostream>
#include <cstring>
#include <vector>
#include "PLYWriter.h"


// Reads the mesh from the PLY file, first the header, then the vertex data, 
// and finally the face data. Then it rescales the model so that it fits a
// box of size 1x1x1 centered at the origin

bool PLYWriter::writeMesh(const string& filename, vector<glm::vec3*>& representatives, vector<int>& triangle_representatives)
{
	ofstream fout;
	int nVertices = representatives.size(), nFaces = triangle_representatives.size();

	fout.open(filename.c_str(), ios_base::out | ios_base::binary);
	if (!fout.is_open())
		return false;
	if (!writeHeader(fout, nVertices, nFaces))
	{
		fout.close();
		return false;
	}

	vector<float> plyVertices;
	vector<int> plyTriangles;

	writeVertices(fout, representatives);
	writeFaces(fout, triangle_representatives);
	fout.close();

	return true;
}

// Reads the header of a PLY file.
// It first checks that the file is really a PLY. 
// Then it reads lines until it finds the 'end_header'
// The 'element vertex' and 'element face' lines contain the number 
// of primitives in the file.

bool PLYWriter::writeHeader(ofstream& fout, int& nVertices, int& nFaces)
{
	char line[100];

	fout << "ply" << endl;
	fout << "format binary_little_endian 1.0" << endl;
	fout << "comment VCGLIB generated" << endl;
	fout << "element vertex " << nVertices << endl;
	fout << "property float x" << endl;
	fout << "property float y" << endl;
	fout << "property float z" << endl;
	fout << "element face " << nFaces << endl;
	fout << "property list uchar int vertex_indices" << endl;
	fout << "end_header" << endl;


	return true;
}

// Loads the vertices' coordinates into a vector

void PLYWriter::writeVertices(ofstream& fout, vector<glm::vec3*>& representatives)
{
	int i;
	float value;

	for (i = 0; i < representatives.size(); i++)
	{
		fout.write((char*)&(*representatives[i]).x, sizeof(float));
		fout.write((char*)&(*representatives[i]).y, sizeof(float));
		fout.write((char*)&(*representatives[i]).z, sizeof(float));
		/*plyVertices[3 * i] = value;
		fin.read((char*)&value, sizeof(float));
		plyVertices[3 * i + 1] = value;
		fin.read((char*)&value, sizeof(float));
		plyVertices[3 * i + 2] = value;*/
	}
}

// Same thing for the faces. Those with more than three sides
// are subdivided into triangles.

void PLYWriter::writeFaces(ofstream& fout, vector<int>& triangle_representatives)
{
	int i, tri[3];
	unsigned char nVrtxPerFace = 3;

	for (i = 0; i < triangle_representatives.size(); i+=3)
	{
		fout.write((char*)&nVrtxPerFace, sizeof(unsigned char));
		fout.write((char*)&triangle_representatives[i], sizeof(int));
		fout.write((char*)&triangle_representatives[i+1], sizeof(int));
		fout.write((char*)&triangle_representatives[i+2], sizeof(int));
	}
}