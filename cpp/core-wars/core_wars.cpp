#include <iostream>
#include <chrono>
#include <random>
#include <unordered_map>

int core_size;
int max_cycles;
int max_processes;
int max_warrior_length;
int min_seperation;
struct Core{
    char* memory;
    bool* colors;
    Core(int core_size=8000){
        memory = new char[core_size];
        colors = new bool[core_size];
    }
    void put(int address, char instruction, int color){
        memory[address] = instruction;
        colors[address] = color;
    }
    ~Core(){
        delete[] memory;
        delete[] colors;
    }
};
struct Warrior{
    bool isAlive;
    int pc; // program counter
    int color;
    Warrior(int startAddress, int warriorColor)
        : isAlive(true), pc(startAddress), color(warriorColor) {}
    
    void executer(Core &core){
        // Example execution logic
        if(isAlive){
            // Execute instruction at pc
            char instruction = core.memory[pc];
            // (Instruction execution logic goes here)
            pc = (pc + 1) % core_size; // Move to next instruction
        }
    }
};

int main (int argc, char** argv) {
    // obtain a seed from the system clock:
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator (seed);  // mt19937 is a standard mersenne_twister_engine
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        std::unordered_map<std::string, int> arg_map = {
            {"--core-size", 0},
            {"--max-cycles", 1},
            {"--max-processes", 2},
            {"--max-warrior-length", 3},
            {"--min-seperation", 4}
        };
        
        if (arg_map.find(arg) != arg_map.end() && i + 1 < argc) {
            int value = std::stoi(argv[i + 1]);
            switch (arg_map[arg]) {
                case 0:
                    core_size = value;
                    break;
                case 1:
                    max_cycles = value;
                    break;
                case 2:
                    max_processes = value;
                    break;
                case 3:
                    max_warrior_length = value;
                    break;
                case 4:
                    min_seperation = value;
                    break;
            }
            i++; // Skip the next argument as it's already processed
        }
    }
    
    return 0;
}