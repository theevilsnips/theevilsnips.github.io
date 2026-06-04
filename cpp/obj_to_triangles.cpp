#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <algorithm>

struct Vertex {
    double x, y, z;
};

struct Face {
    int v1, v2, v3;
};

std::vector<Vertex> vertices;
std::vector<Face> faces;
std::vector<std::pair<int, int>> edges;
bool parseOBJFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') {
            continue;
        }

        std::istringstream iss(line);
        std::string type;
        iss >> type;

        if (type == "v") {
            // Parse vertex
            Vertex v;
            iss >> v.x >> v.y >> v.z;
            vertices.push_back(v);
        } 
        else if (type == "f") {
            // Parse face - handles triangles and quads
            std::vector<int> faceVertices;
            std::string token;
            
            while (iss >> token) {
                // Extract vertex index (handle v, v/vt, v/vt/vn, v//vn formats)
                int vertexIndex = std::atoi(token.c_str());
                faceVertices.push_back(vertexIndex - 1); // OBJ uses 1-based indexing
            }

            // Convert to triangles (triangulate if necessary)
            for (size_t i = 1; i < faceVertices.size() - 1; i++) {
                Face f;
                f.v1 = faceVertices[0];
                f.v2 = faceVertices[i];
                f.v3 = faceVertices[i + 1];
                faces.push_back(f);
            }
        }
        
    }
    //Process edges based on faces (no duplicate edges)
    
    for (const Face& f : faces) {
        std::pair<int, int> e1 = {std::min(f.v1, f.v2), std::max(f.v1, f.v2)};
        std::pair<int, int> e2 = {std::min(f.v2, f.v3), std::max(f.v2, f.v3)};
        std::pair<int, int> e3 = {std::min(f.v3, f.v1), std::max(f.v3, f.v1)};
        if (std::find(edges.begin(), edges.end(), e1) == edges.end()) {
            edges.push_back(e1);
        }
        if (std::find(edges.begin(), edges.end(), e2) == edges.end()) {
            edges.push_back(e2);
        }
        if (std::find(edges.begin(), edges.end(), e3) == edges.end()) {
            edges.push_back(e3);
        }
    }
    file.close();
    return true;
}

void outputArrays() {
    std::cout << "Points: [";
    for (size_t i = 0; i < vertices.size(); i++) {
        const Vertex& v = vertices[i];
        if (i > 0) {
            std::cout << ",";
        }
        std::cout << v.x << "," << v.y << "," << v.z;
    }
    std::cout << "]\n";
    std::cout << "Edges: [";
    size_t i = 0;
    for (const auto& edge : edges) {
        if (i > 0) std::cout << ",";
        std::cout << edge.first << "," << edge.second;
        i++;
    }
    std::cout << "]\n";
    std::cout << "Faces: [";
    
    for (size_t i = 0; i < faces.size(); i++) {
        const Face& f = faces[i];
        int v1 = f.v1;
        int v2 = f.v2;
        int v3 = f.v3;

        if (i > 0) {
            std::cout << ",";
        }

        std::cout  << v1 << "," << v2 << "," << v3;
    }

    std::cout << "]\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <obj_file>" << std::endl;
        return 1;
    }

    std::string objFile = argv[1];

    if (!parseOBJFile(objFile)) {
        return 1;
    }

    if (vertices.empty() || faces.empty()) {
        std::cerr << "Error: No vertices or faces found in OBJ file" << std::endl;
        return 1;
    }

    outputArrays();

    return 0;
}
