
#include <climits>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sys/types.h>
#include <vector>

#include "Config.h"
#include "json.hpp"
using namespace nlohmann::literals;

#ifndef SIMULATOR_H
#define SIMULATOR_H

#define MAX_MEM_SIZE 1024
#define NUM_PHYS_REG 32

enum DebugArg
{
	DEBUG_DCACHE,
	DEBUG_REGISTERS
};

struct ArchitecturalRegister
{
	enum
	{
		R,
		F
	} type;
	int32_t num;

	bool operator<(const ArchitecturalRegister& o) const
	{
		return (type < o.type) || (type == o.type && num < o.num);
	}
};

struct RegisterFileEntry
{
	double value;
};

class Simulator
{
public:
	/**
	 * @brief Simulator Configuration
	 */
	Config *configuration;

	/**
	 * @brief Data Memory ($D Cache)
	 * On startup, all values are initialized to 0
	 */
	double dataMemory[MAX_MEM_SIZE] = {0};

	/**
	 * @brief Register File
	 * @note This is a map of physical register number -> value
	 * @note For bookkeeping, we also store the ROB number and whether the register is busy
	 */
	RegisterFileEntry registerFile[NUM_PHYS_REG];
	std::map<ArchitecturalRegister, int> registerMapTable;

	// *PUBLIC --------------------
	/**
	 * @brief Constructor for Simulator that uses custom configuration
	 * @param program Pointer to ifstream object containing the program
	 * @param c Configuration struct
	 */
	Simulator(std::ifstream* program, Config *c);

	/**
	 * @brief Debugging function
	 * @param argc Number of arguments excluding argc
	 * @param ... Variable arguments - see enum DebugArg
	 * 
	 * @example this->dump(1, DEBUG_ALL);
	 */
	void dump(int argc...);
	void printStats();

	/**
	 * @brief Serialize the simulator state to a JSON file
	 * @param output Pointer to ofstream object where the serialized data is written
	 */
	void serializeJSON(std::ofstream* output);
};

#endif // SIMULATOR_H
