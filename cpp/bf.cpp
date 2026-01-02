//Brainf*ck
#include <iostream>
#include <fstream>
#include <vector>
int main(int argc, char** argv) {
    std::string code;
    std::ifstream file;
    if (argc < 2) {
        std::cin >> code;
    } else {
        file.open(argv[1]);
        if (!file.is_open()) {
        std::cerr << "Error opening file: " << argv[1] << '\n';
        return 1;
    }
    }
    
    const char* filename = argv[1];
    // Read the entire file content into the code string
    if (file.is_open()) {
      std::string line;
        while (std::getline(file, line)) {
            code += line;
            code += '\n'; // Preserve newlines
        }
    }
    unsigned char tape[30000] = {0};
    unsigned char* ptr = tape;
    std::vector<size_t> loop_stack;
    for (size_t i = 0; i < code.size(); ++i) {
        switch (code[i]) {
            case '>':
                ++ptr;
                break;
            case '<':
                --ptr;
                break;
            case '+':
                ++(*ptr);
                break;
            case '-':
                --(*ptr);
                break;
            case '.':
                std::cout << *ptr;
                break;
            case ',':
                *ptr = std::cin.get();
                break;
            case '[':
                if (*ptr == 0) {
                    int bracket_count = 1;
                    while (bracket_count > 0 && i < code.size()) {
                        ++i;
                        if (code[i] == '[') ++bracket_count;
                        else if (code[i] == ']') --bracket_count;
                    };
                    
                } else {
                    loop_stack.push_back(i);
                }
                break;
            case ']':
                if (*ptr != 0) {
                    i = loop_stack.back();
                } else {
                    loop_stack.pop_back();
                }
                break;
        }
    }
    

    
    file.close();
    return 0;
}