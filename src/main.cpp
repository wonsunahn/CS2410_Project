#include "../include/Simulator.h"
#include <fstream>
#include <iostream>

struct config getConfig(char* configFile)
{
	FILE* f = fopen(configFile, "r");
	struct config c = {4, 16, 4, 16, 4};
	fscanf(f, "%d %d %d %d %d", &c.NF, &c.NI, &c.NW, &c.NR, &c.NB);
	fclose(f);
	return c;
}

int main(int argc, char* argv[])
{
	if(argc < 2)
	{
		std::cout << "Usage:\t./sim <prog.dat>\n";
		std::cout << "\t./sim <prog.dat> <config.cfg>\n";
		return 1;
	}
	std::ifstream program(argv[1]);
	if(!program.is_open())
	{
		std::cerr << "Failed to open " << argv[1] << " for writing." << std::endl;
		return 1;
	}

	struct config c = getConfig(argv[2]);
	Simulator* sim = new Simulator(&program, c);
	sim->printStats();
	sim->dump(2, DEBUG_DCACHE, DEBUG_REGISTERS);
	delete sim;

	return 0;
}
