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
