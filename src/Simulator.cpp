#include "Simulator.h"
#include <fstream>
#include <iostream>
#include <map>

Simulator::Simulator(std::ifstream* program, struct config c)
{
}

void Simulator::printStats()
{
	std::cout << "Total CCs: " << "0" << std::endl;
	std::cout << "RS Stalls: " << "0" << std::endl;
	std::cout << "ROB Stalls: " << "0" << std::endl;
};

void Simulator::serializeJSON(std::ofstream* output)
{
	nlohmann::json j;
	j["total_cycles"] = 0;
	j["rs_stalls"] = 0;
	j["rob_stalls"] = 0;
	for (int i = 0; i < MAX_MEM_SIZE; i++)
	{
		j["data_memory"][i] = dataMemory[i];
	}
	for (int i = 0; i < NUM_PHYS_REG; i++)
	{
		j["register_file"][i] = registerFile[i].value;
	}
	*output << j.dump(4) << std::endl;
}