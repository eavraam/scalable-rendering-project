#ifndef _OCTREE_INCLUDE
#define _OCTREE_INCLUDE

#include <iostream>
#include <vector>
#include <glm/glm.hpp>
#include <unordered_map>
#include <eigen3/Eigen/Dense>

// #define simplification modes
#define REPRESENTATIVE_MEAN 0
#define REPRESENTATIVE_QEM 1

using namespace std;

// #define x: Right/Left y: Top/Bottom z: Front/Back
#define RightTopFront 0
#define LeftTopFront 1
#define LeftBottomFront 2
#define RightBottomFront 3
#define RightTopBack 4
#define LeftTopBack 5
#define LeftBottomBack 6
#define RightBottomBack 7

// Octree class
class Octree {

    // if vertex == NULL, node is internal node.
    // if vertex == (-2.f, -2.f, -2.f), node is empty.
    glm::vec3 *main_vertex;
    vector<glm::vec3*> extra_vertices;

    // Represent the boundary of the cube
    glm::vec3 rightTopFront, leftBottomBack;
    vector<Octree*> children;

    int depth_level;

public:
    // Constructor
    Octree(int depth);

    // Constructor with three arguments
    Octree(glm::vec3& vertex, int depth);

    // Constructor with six arguments
    Octree(glm::vec3 vertex_topLeftFront, glm::vec3 vertex_bottomRightBack, int depth);

    // Function to insert a point in the octree
    void insert(glm::vec3& vertex);

    // Function that returns true if the point
    // (x, y, z) exists in the octree
    bool find(glm::vec3& vertex);

    void getAllVertices(vector<glm::vec3*> &cluster_vertices, unordered_map<glm::vec3*, int> &vertices_representative, int n_representatives);
    void computeRepresentatives(vector<glm::vec3*>& representatives, unordered_map<glm::vec3*, int>& vertices_representative, unordered_map<glm::vec3*, Eigen::Matrix4f*> vertices_quadric, int simplification_mode, int depth);
    void calculateMeanRepresentative(vector<glm::vec3*>& representatives, unordered_map<glm::vec3*, int> &vertices_representative, int depth = -1);
    void calculateQEMRepresentative(vector<glm::vec3*>& representatives, unordered_map<glm::vec3*, int>& vertices_representative, unordered_map<glm::vec3*, Eigen::Matrix4f*> vertices_quadric, int depth = -1);
};

#endif // _OCTREE_INCLUDE
