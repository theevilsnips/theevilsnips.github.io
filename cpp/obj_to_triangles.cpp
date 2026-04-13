#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>

struct Vertex {
    double x, y, z;
};

struct Face {
    int v1, v2, v3;
};

std::vector<Vertex> vertices;
std::vector<Face> faces;

bool parseOBJFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
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

    file.close();
    return true;
}

void outputTriangles() {
    std::cout << "[";
    
    for (size_t i = 0; i < faces.size(); i++) {
        const Face& f = faces[i];
        const Vertex& v1 = vertices[f.v1];
        const Vertex& v2 = vertices[f.v2];
        const Vertex& v3 = vertices[f.v3];

        if (i > 0) {
            std::cout << ",";
        }

        std::cout  << v1.x << "," << v1.y << "," << v1.z << ","
                       << v2.x << "," << v2.y << "," << v2.z << ","
                       << v3.x << "," << v3.y << "," << v3.z;
    }

    std::cout << "]" << std::endl;
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

    outputTriangles();

    return 0;
}
