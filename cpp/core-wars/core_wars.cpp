#include <iostream>
#include <chrono>
#include <random>
#include <unordered_map>
#include <vector>
#include <fstream>

int core_size=8000;
int max_cycles=100000;
int max_processes=1000000;
int max_warrior_length=10000;
int min_seperation=10;
struct Core{
    char* memory;
    bool* colors;
    
    Core(int core_size=8000) {
        memory = new char[core_size];
        colors = new bool[core_size];
    }
    void put(int address, char instruction, int color){
        memory[address] = instruction;
        colors[address] = color;
    }
    int get(int address){
        return memory[address];
    }
    ~Core(){
        delete[] memory;
        delete[] colors;
    }
};

struct Warrior{
    int start_address;
    int length;
    int color;
    int num_processes=1;
    int current_process=0;
    std::vector<char> instructions;
    std::vector<int> processes;
    Warrior(int start_address, std::vector<char> instructions, int color, Core &core)
        : start_address(start_address), instructions(instructions), length(instructions.size()), color(color) {
            for (int i = 0; i < length; ++i) {
                core.put(start_address + i, instructions[i], color); // Place instructions in core memory
            }
            
        }
    void execute_step(Core &core) {
        current_process = (current_process + 1) % num_processes;
        int pc = processes[current_process];
        char instruction = core.get(pc);
        //Instruction execution logic
        // For now, just move to the next instruction

        processes[current_process] = (pc + 1) % core_size; // Move to next instruction

    }
};

std::vector<Warrior*> warriors;
int main (int argc, char** argv) {
    std::ifstream war_1("1.redcode");
    std::ifstream war_2("2.redcode");
    std::string Warrior1, Warrior2="";
    if (!war_1.is_open() || !war_2.is_open()) {
        std::cerr << "Error opening warrior files." << "\n";
        return 1;
    }
    while (!war_1.eof()) {
        std::string line;
        std::getline(war_1, line);
        Warrior1 += line + "\n";
    }
    while (!war_2.eof()) {
        std::string line;
        std::getline(war_2, line);
        Warrior2 += line + "\n";
    }
    war_1.close();
    war_2.close();
    // obtain a seed from the system clock:
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 generator (seed);  // mt19937 is a standard mersenne_twister_engine
    auto option_parser = [argc, argv]() {
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
        }

        return 0;}
    }();
    auto code_parser = [&](std::string& warrior_code) {
        std::string parsed_code;
        if (warrior_code.size() > max_warrior_length) {
                std::cerr << "Error: Warrior exceeds maximum length." << "\n";
                
                return std::string();
            }
        for (int i=0; i<warrior_code.size(); ++i) {
            
        }
        return parsed_code;
    };
    code_parser(Warrior1);
    code_parser(Warrior2);
    Core core(core_size);
    auto create_warrior = [&](const std::string& warrior_code, int color) {
        std::vector<char> instructions;
        for (char c : warrior_code) {
            instructions.push_back(c);
        }
        std::uniform_int_distribution<int> distribution(0, core_size - instructions.size());
        int start_address;
        bool valid_position;
        do {
            start_address = distribution(generator);
            valid_position = true;
            for (int i = 0; i < instructions.size(); ++i) {
                if (core.colors[start_address + i]) {
                    valid_position = false;
                    break;
                }
            }
        } while (!valid_position);
        Warrior* warrior = new Warrior(start_address, instructions, color, core);
        warriors.push_back(warrior);
    };
    create_warrior(Warrior1, 1);
    create_warrior(Warrior2, 2);
    // Main execution loop
    for (int cycle = 0; cycle < max_cycles; ++cycle) {
        for (Warrior* warrior : warriors) {
            warrior->execute_step(core);
        }
    }
    std::cout << "Draw: Simulation ended after " << max_cycles << " cycles." << "\n";
    return 0;
}