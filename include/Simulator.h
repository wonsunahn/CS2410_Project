
#include <climits>
#include <cstdint>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <sys/types.h>
#include <vector>

#ifndef RV32SIM_SIMULATOR_H
#	define RV32SIM_SIMULATOR_H

#	define MAX_MEM_SIZE 1024
#	define NUM_PHYS_REG 32

struct config
{
	int NF; // Issue Width
	int NI; // Instruction Queue Size
	int NW; // Number of Instructions Executable
	int NR; // Reorder Buffer Size
	int NB; // Number of Common Data Buses (CDB)
};

enum debugArg
{
	DEBUG_DCACHE,
	DEBUG_REGISTERS
};

struct virtualRegister
{
	enum
	{
		R,
		F
	} type;
	int32_t num;
};

struct registerFileEntry
{
	double value;
};

class Simulator
{
public:
	/**
	 * @brief Simulator Configuration
	 */
	struct config configuration;

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
	registerFileEntry registerFile[NUM_PHYS_REG];
	std::map<virtualRegister, int> registerMapTable;

	// *PUBLIC --------------------
	/**
	 * @brief Constructor for Simulator that uses custom configuration
	 * @param program Pointer to ifstream object containing the program
	 * @param c Configuration struct
	 */
	Simulator(std::ifstream* program, struct config c);

	/**
	 * @brief Debugging function
	 * @param argc Number of arguments excluding argc
	 * @param ... Variable arguments - see enum debugArg
	 * 
	 * @example this->debug(1, DEBUG_ALL);
	 */
	void dump(int argc...);
	void printStats();
};

#endif // RV32SIM_SIMULATOR_H
