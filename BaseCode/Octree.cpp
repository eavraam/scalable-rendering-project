#include "Octree.h"

// Constructor
Octree::Octree(int depth)
{
    depth_level = depth;
    // To declare empty node
    main_vertex = new glm::vec3(-2.f, -2.f, -2.f);
}

// Constructor with three arguments
Octree::Octree(glm::vec3 &vertex_leaf, int depth)
{
    depth_level = depth;
    // To declare leaf node
    main_vertex = &vertex_leaf;
}

// Constructor with six arguments
Octree::Octree(glm::vec3 vertex_leftBottomBack, glm::vec3 vertex_rightTopFront, int depth)
{
    depth_level = depth;
    // This use to construct Octree or internal node
    // with boundaries defined
    if (vertex_rightTopFront.x < vertex_leftBottomBack.x
        || vertex_rightTopFront.y < vertex_leftBottomBack.y
        || vertex_rightTopFront.z < vertex_leftBottomBack.z) {
        cout << "boundary points are not valid" << endl;
        return;
    }

    main_vertex = nullptr;
    rightTopFront = vertex_rightTopFront;
    leftBottomBack = vertex_leftBottomBack;

    // Assigning null to the children
    children.assign(8, nullptr);
    for (int i = RightTopFront;
        i <= RightBottomBack;
        ++i)
        children[i] = new Octree(depth_level - 1);
}

// Function to insert a vertex in the octree
void Octree::insert(glm::vec3 &vertex)
{

    // If the vertex already exists in the octree
    if (find(vertex)) {
        cout << "Vertex already exist in the tree" << endl;
        return;
    }

    // If the depth_level has been reached
    if (depth_level == 0) {
       cout << "Depth level has been reached" << endl;
       if (main_vertex->x == -2.0f) {
           // To declare leaf node
           main_vertex = &vertex;
       } else
           extra_vertices.push_back(&vertex);
       return;
    }

    // If the vertex is out of bounds
    if (vertex.x < leftBottomBack.x
        || vertex.x > rightTopFront.x
        || vertex.y < leftBottomBack.y
        || vertex.y > rightTopFront.y
        || vertex.z < leftBottomBack.z
        || vertex.z > rightTopFront.z) {
        cout << "Vertex is out of bound" << endl;
        return;
    }

    // Binary search to insert the vertex
    float midx = (leftBottomBack.x
        + rightTopFront.x)
        / 2;
    float midy = (leftBottomBack.y
        + rightTopFront.y)
        / 2;
    float midz = (leftBottomBack.z
        + rightTopFront.z)
        / 2;

    int pos = -1;

    // Checking the octant of
    // the vertex
    if (vertex.x < midx) {
        if (vertex.y < midy) {
            if (vertex.z < midz)
                pos = LeftBottomBack;
            else
                pos = LeftBottomFront;
        }
        else {
            if (vertex.z < midz)
                pos = LeftTopBack;
            else
                pos = LeftTopFront;
        }
    }
    else {
        if (vertex.y < midy) {
            if (vertex.z < midz)
                pos = RightBottomBack;
            else
                pos = RightBottomFront;
        }
        else {
            if (vertex.z < midz)
                pos = RightTopBack;
            else
                pos = RightTopFront;
        }
    }

    // If an internal node is encountered
    if (children[pos]->main_vertex == nullptr) {
        if ((children[pos]->depth_level) == 0) {
            //cout << "depth level == 0";
        }
        children[pos]->insert(vertex);
        return;
    }

    // If an empty node is encountered
    else if (children[pos]->main_vertex->x == -2.0f) {
        if ((children[pos]->depth_level) == 0) {
            delete children[pos]->main_vertex;
            children[pos]->main_vertex = &vertex;
        }
        else {
            delete children[pos];
            children[pos] = new Octree(vertex, depth_level - 1);
        }
        return;
    }
    // If a leaf node is encountered
    else {
        if ((children[pos]->depth_level) == 0) {
            children[pos]->extra_vertices.push_back(&vertex);
        }
        else {
            glm::vec3* vertex_;
            vertex_ = children[pos]->main_vertex;
            delete children[pos];
            if (pos == RightTopFront) {
                children[pos] = new Octree(glm::vec3(midx, midy, midz),
                    glm::vec3(rightTopFront.x, rightTopFront.y, rightTopFront.z),
                    depth_level - 1);
            }
            else if (pos == LeftTopFront) {
                children[pos] = new Octree(glm::vec3(leftBottomBack.x, midy, midz),
                    glm::vec3(midx, rightTopFront.y, rightTopFront.z),
                    depth_level - 1);
            }
            else if (pos == LeftBottomFront) {
                children[pos] = new Octree(glm::vec3(leftBottomBack.x, leftBottomBack.y, midz),
                    glm::vec3(midx, midy, rightTopFront.z),
                    depth_level - 1);
            }
            else if (pos == RightBottomFront) {
                children[pos] = new Octree(glm::vec3(midx, leftBottomBack.y, midz),
                    glm::vec3(rightTopFront.x, midy, rightTopFront.z),
                    depth_level - 1);
            }
            else if (pos == RightTopBack) {
                children[pos] = new Octree(glm::vec3(midx, midy, leftBottomBack.z),
                    glm::vec3(rightTopFront.x, rightTopFront.y, midz),
                    depth_level - 1);
            }
            else if (pos == LeftTopBack) {
                children[pos] = new Octree(glm::vec3(leftBottomBack.x, midy, leftBottomBack.z),
                    glm::vec3(midx, rightTopFront.y, midz),
                    depth_level - 1);
            }
            else if (pos == LeftBottomBack) {
                children[pos] = new Octree(glm::vec3(leftBottomBack.x, leftBottomBack.y, leftBottomBack.z),
                    glm::vec3(midx, midy, midz),
                    depth_level - 1);
            }
            else if (pos == RightBottomBack) {
                children[pos] = new Octree(glm::vec3(midx, leftBottomBack.y, leftBottomBack.z),
                    glm::vec3(rightTopFront.x, midy, midz),
                    depth_level - 1);
            }
            children[pos]->insert(*vertex_);
            children[pos]->insert(vertex);
        }
        return;
    }
}

// Function that returns true if the vertex
// (x, y, z) exists in the octree
bool Octree::find(glm::vec3& vertex)
{
    // If vertex is out of bound
    if (vertex.x < leftBottomBack.x
        || vertex.x > rightTopFront.x
        || vertex.y < leftBottomBack.y
        || vertex.y > rightTopFront.y
        || vertex.z < leftBottomBack.z
        || vertex.z > rightTopFront.z)
        return 0;

    // Otherwise perform binary search
    // for each ordinate
    float midx = (leftBottomBack.x
        + rightTopFront.x)
        / 2;
    float midy = (leftBottomBack.y
        + rightTopFront.y)
        / 2;
    float midz = (leftBottomBack.z
        + rightTopFront.z)
        / 2;

    int pos = -1;

    // Deciding the position
    // where to move
    if (vertex.x <= midx) {
        if (vertex.y <= midy) {
            if (vertex.z <= midz)
                pos = LeftBottomBack;
            else
                pos = LeftBottomFront;
        }
        else {
            if (vertex.z <= midz)
                pos = LeftTopBack;
            else
                pos = LeftTopFront;
        }
    }
    else {
        if (vertex.y <= midy) {
            if (vertex.z <= midz)
                pos = RightBottomBack;
            else
                pos = RightBottomFront;
        }
        else {
            if (vertex.z <= midz)
                pos = RightTopBack;
            else
                pos = RightTopFront;
        }
    }

    // If an internal node is encountered
    if (children[pos]->main_vertex == nullptr) {
        return children[pos]->find(vertex);
    }

    // If an empty node is encountered
    else if (children[pos]->main_vertex->x == -2.f) {
        return 0;
    }
    // If a leaf node is encountered
    else {
        // If node is found with
        // the given value
        if (vertex.x == children[pos]->main_vertex->x
            && vertex.y == children[pos]->main_vertex->y
            && vertex.z == children[pos]->main_vertex->z)
            return &vertex == children[pos]->main_vertex;
        for (unsigned int i = 0; i < children[pos]->extra_vertices.size(); i++) {
            if (vertex.x == children[pos]->extra_vertices[i]->x
                && vertex.y == children[pos]->extra_vertices[i]->y
                && vertex.z == children[pos]->extra_vertices[i]->z)
                return &vertex == children[pos]->extra_vertices[i];
        }
    }
    return 0;
}

void Octree::getAllVertices(vector<glm::vec3*> &cluster_vertices, unordered_map<glm::vec3*, int> &vertices_representative, int n_representatives) {
    // If an internal node is encountered
    if (main_vertex == nullptr) {
        for (int pos = 0; pos < 8; pos++)
            children[pos]->getAllVertices(cluster_vertices, vertices_representative, n_representatives);
    }
    // If an empty node is encountered
    else if (main_vertex->x == -2.0f) {
        return;
    }
    // If a leaf node is encountered
    else {
        cluster_vertices.push_back(main_vertex);
        //vertices_representative.insert_or_assign(main_vertex, n_representatives);
        //vertices_representative[main_vertex] = n_representatives;
        for (unsigned int i = 0; i < extra_vertices.size(); i++) {
            cluster_vertices.push_back(extra_vertices[i]);
            //vertices_representative.insert_or_assign(extra_vertices[i], n_representatives);
            //vertices_representative[extra_vertices[i]] = n_representatives;
        }
    }
}

void Octree::calculateMeanRepresentative(vector<glm::vec3*>& representatives, unordered_map<glm::vec3*, int>& vertices_representative, int depth) {
    int n_representatives = representatives.size();
    // If max depth is reached
    if (depth == 1) {
        // If an internal node is encountered
        if (main_vertex == nullptr) {
            vector<glm::vec3*> cluster_vertices;
            getAllVertices(cluster_vertices, vertices_representative, n_representatives);
            glm::vec3* representative = new glm::vec3(0.f, 0.f, 0.f);
            for (unsigned int i = 0; i < cluster_vertices.size(); i++) {
                *representative = *representative + *cluster_vertices[i];
                vertices_representative[cluster_vertices[i]] = n_representatives;
            }
            *representative = *representative / float(cluster_vertices.size());
            representatives.push_back(representative);
            return;
        }
        // If an empty node is encountered
        else if (main_vertex->x == -2.f) {
            return;
        }
        // If a leaf node is encountered
        else {
            glm::vec3* representative = new glm::vec3(*main_vertex);
            //vertices_representative.insert_or_assign(main_vertex, n_representatives);
            vertices_representative[main_vertex] = n_representatives;
            for (unsigned int i = 0; i < extra_vertices.size(); i++) {
                *representative = *representative + *extra_vertices[i];
                //vertices_representative.insert_or_assign(extra_vertices[i], n_representatives);
                vertices_representative[extra_vertices[i]] = n_representatives;
            }
            *representative = *representative / (extra_vertices.size() + 1.f);
            representatives.push_back(representative);
            return;
        }
    }
    // If a leaf node is encountered
    else {
        glm::vec3 *representative = new glm::vec3(*main_vertex);
        //vertices_representative.insert_or_assign(main_vertex, n_representatives);
        vertices_representative[main_vertex] = n_representatives;
        for(unsigned int i=0; i< extra_vertices.size();i++){
            *representative = *representative + *extra_vertices[i];
            //vertices_representative.insert_or_assign(extra_vertices[i], n_representatives);
            vertices_representative[extra_vertices[i]] = n_representatives;
        }
        *representative = *representative / (extra_vertices.size() + 1.f);
        representatives.push_back(representative);
    }
    return;
}

void jacobiSolver(Eigen::Matrix4f quadric_representative, glm::vec3* &representative) {
    /*Eigen::MatrixXf m(4, 3);
    m(0, 0) = quadric_representative(0, 0);
    m(1, 0) = quadric_representative(1, 0);
    m(2, 0) = quadric_representative(2, 0);
    m(3, 0) = quadric_representative(3, 0);
    m(0, 1) = quadric_representative(0, 1);
    m(1, 1) = quadric_representative(1, 1);
    m(2, 1) = quadric_representative(2, 1);
    m(3, 1) = quadric_representative(3, 1);
    m(0, 2) = quadric_representative(0, 2);
    m(1, 2) = quadric_representative(1, 2);
    m(2, 2) = quadric_representative(2, 2);
    m(3, 2) = quadric_representative(3, 2);
    Eigen::JacobiSVD<Eigen::MatrixXf> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Vector3f rhs(0.f, 0.f, 0.f);
    Eigen::Vector3f eigen_representative = svd.solve(rhs);
    *representative = glm::vec3(eigen_representative(0), eigen_representative(1), eigen_representative(2));*/



    Eigen::MatrixXf m(4, 4);
    m = quadric_representative;
    m(0, 3) = 0.f;
    m(1, 3) = 0.f;
    m(2, 3) = 0.f;
    m(3, 3) = 1.f;
    Eigen::JacobiSVD<Eigen::MatrixXf> svd(m, Eigen::ComputeThinU | Eigen::ComputeThinV);
    Eigen::Vector4f rhs(0.f, 0.f, 0.f, 1.f);
    Eigen::Vector4f eigen_representative = svd.solve(rhs);
    *representative = glm::vec3(eigen_representative(0), eigen_representative(1), eigen_representative(2));
    return;
}

void Octree::calculateQEMRepresentative(vector<glm::vec3*>& representatives, unordered_map<glm::vec3*, int>& vertices_representative, unordered_map<glm::vec3*, Eigen::Matrix4f*> vertices_quadric, int depth) {
    int n_representatives = representatives.size();
    Eigen::Matrix4f quadric_representative;
    // If max depth is reached
    if (depth == 1) {
        // If an internal node is encountered
        if (main_vertex == nullptr) {
            vector<glm::vec3*> cluster_vertices;
            getAllVertices(cluster_vertices, vertices_representative, n_representatives);
            glm::vec3* representative = new glm::vec3(0.f, 0.f, 0.f);
            quadric_representative <<
                0.f, 0.f, 0.f, 0.f,
                0.f, 0.f, 0.f, 0.f,
                0.f, 0.f, 0.f, 0.f,
                0.f, 0.f, 0.f, 0.f;
            for (unsigned int i = 0; i < cluster_vertices.size(); i++) {
                //*representative = *representative + *cluster_vertices[i];
                quadric_representative = quadric_representative + *vertices_quadric[cluster_vertices[i]];
                vertices_representative[cluster_vertices[i]] = n_representatives;
            }
            //*representative = *representative / float(cluster_vertices.size());
            jacobiSolver(quadric_representative, representative);
            representatives.push_back(representative);
            return;
        }
        // If an empty node is encountered
        else if (main_vertex->x == -2.f) {
            return;
        }
        // If a leaf node is encountered
        else {
            glm::vec3* representative = new glm::vec3(0.f, 0.f, 0.f);
            quadric_representative = *vertices_quadric[main_vertex];
            //vertices_representative.insert_or_assign(main_vertex, n_representatives);
            vertices_representative[main_vertex] = n_representatives;
            for (unsigned int i = 0; i < extra_vertices.size(); i++) {
                //*representative = *representative + *extra_vertices[i];
                quadric_representative = quadric_representative + *vertices_quadric[extra_vertices[i]];
                //vertices_representative.insert_or_assign(extra_vertices[i], n_representatives);
                vertices_representative[extra_vertices[i]] = n_representatives;
            }
            //*representative = *representative / (extra_vertices.size() + 1.f);
            jacobiSolver(quadric_representative, representative);
            representatives.push_back(representative);
            return;
        }
    }
    // If a leaf node is encountered
    else {
        glm::vec3* representative = new glm::vec3(*main_vertex);
        //vertices_representative.insert_or_assign(main_vertex, n_representatives);
        vertices_representative[main_vertex] = n_representatives;
        for (unsigned int i = 0; i < extra_vertices.size(); i++) {
            *representative = *representative + *extra_vertices[i];
            //vertices_representative.insert_or_assign(extra_vertices[i], n_representatives);
            vertices_representative[extra_vertices[i]] = n_representatives;
        }
        *representative = *representative / (extra_vertices.size() + 1.f);
        representatives.push_back(representative);
    }
    return;
}

void Octree::computeRepresentatives(vector<glm::vec3*> &representatives, unordered_map<glm::vec3*, int> &vertices_representative, unordered_map<glm::vec3*, Eigen::Matrix4f*> vertices_quadric, int simplification_mode, int depth) {
    // If max depth is reached
    if (depth == 1) {
        if(simplification_mode == REPRESENTATIVE_MEAN)
            calculateMeanRepresentative(representatives, vertices_representative, depth);
        else if (simplification_mode == REPRESENTATIVE_QEM)
            calculateQEMRepresentative(representatives, vertices_representative, vertices_quadric, depth);
    }
    // If an internal node is encountered
    else if (main_vertex == nullptr) {
        for(unsigned int pos=0; pos < 8; pos++)
            children[pos]->computeRepresentatives(representatives, vertices_representative, vertices_quadric, simplification_mode, depth - 1);
    }
    // If an empty node is encountered
    else if (main_vertex->x == -2.f) {
        return;
    }
    // If a leaf node is encountered
    else {
        if (simplification_mode == REPRESENTATIVE_MEAN)
            calculateMeanRepresentative(representatives, vertices_representative, depth);
        else if (simplification_mode == REPRESENTATIVE_QEM)
            calculateQEMRepresentative(representatives, vertices_representative, vertices_quadric, depth);
    }
    return;
}