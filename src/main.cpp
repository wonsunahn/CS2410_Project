#include "Simulator.h"
#include <fstream>
#include <iostream>
#include <bits/getopt_core.h>

void print_usage_info()
{
	printf("USAGE: sim [OPTIONS]\n");
	printf("Runs a CPU simulation given a CPU configuration file and a program file.\n\n");
	printf("  -h           this help screen.\n");
	printf("  -d           debug output (shows pipeline on each cycle).\n");
	printf("  -c file      [Required] uses file as configuration file.\n");
	printf("  -p file      [Required] uses file as program file.\n");
	printf("  -j file      [Optional] uses file as JSON output file.\n");
}

int main(int argc, char *argv[])
{
	char *program_file_name = NULL;
	char *config_file_name = NULL;
	char *json_file_name = NULL;
	char c;
	while ((c = getopt(argc, argv, "hdc:p:j:")) != -1)
	{
		switch (c)
		{
		case 'h':
			print_usage_info();
			return 0;
		case 'd':
			debug = true;
			break;
		case 'p':
			program_file_name = optarg;
			break;
		case 'c':
			config_file_name = optarg;
			break;
		case 'j':
			json_file_name = optarg;
			break;
		case '?':
			if (optopt == 'j' || optopt == 'c' || optopt == 'p')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr,
						"Unknown option character `\\x%x'.\n",
						optopt);
			return 1;
		default:
			abort();
		}
	}

	if (config_file_name == NULL || program_file_name == NULL)
	{
		print_usage_info();
		exit(1);
	}
	std::ifstream program(program_file_name);
	if (!program.is_open())
	{
		std::cerr << "Failed to open " << program_file_name << " for reading." << std::endl;
		return 1;
	}
	if (!parse_config(config_file_name))
	{
		fprintf(stderr, "Error while parsing config file %s.\n", config_file_name);
		exit(1);
	}

	Simulator *sim = new Simulator(&program, config);

	// Run the simulator until completion
	
	/* TODO: Replace the hardcoded state with the actual simulation loop that executes instructions until the program is complete.
	 *       The loop should look something like this:
	 *       while (simulation not complete) {
	 *           // Simulate fetch, decode, execute, and commit stages, updating the simulator state accordingly.
	 *       }
	 */

	// For testing purposes, I've hardcoded the state of the CPU after executing prog-noloop.dat.
	// Data memory values stored by the program
	sim->dataMemory[0] = 111;
	sim->dataMemory[8] = 14;
	sim->dataMemory[16] = 5;
	sim->dataMemory[24] = 10;
	sim->dataMemory[100] = 2;
	sim->dataMemory[108] = 27;
	sim->dataMemory[116] = 3;
	sim->dataMemory[124] = 128;
	sim->dataMemory[200] = 12;
	// Architectural registers mapped to physical registers
	sim->registerMapTable[ArchitecturalRegister{ArchitecturalRegister::X, 1}] = 7;
	sim->registerFile[7].value = 16;
	sim->registerMapTable[ArchitecturalRegister{ArchitecturalRegister::X, 2}] = 8;
	sim->registerFile[8].value = 116;
	sim->registerMapTable[ArchitecturalRegister{ArchitecturalRegister::F, 0}] = 6;
	sim->registerFile[6].value = 128;
	sim->registerMapTable[ArchitecturalRegister{ArchitecturalRegister::F, 2}] = 2;
	sim->registerFile[2].value = 12;
	sim->registerMapTable[ArchitecturalRegister{ArchitecturalRegister::F, 4}] = 5;
	sim->registerFile[5].value = 8;
	// Physical registers that were used but are not currently mapped to any architectural register
	sim->registerFile[0].value = 24;
	sim->registerFile[1].value = 124;
	sim->registerFile[3].value = 10;
	sim->registerFile[4].value = 120;

	// Print out simulation results and statistics
	std::cout << "Simulation complete!" << std::endl;
	sim->printStats();
	if (debug)
		sim->dump(2, DEBUG_DCACHE, DEBUG_REGISTERS);
	if (json_file_name != NULL)
	{
		std::ofstream json_output(json_file_name);
		if (!json_output.is_open())
		{
			std::cerr << "Failed to open " << json_file_name << " for writing JSON." << std::endl;
			return 1;
		}
		sim->serializeJSON(&json_output);
		json_output.close();
	}
	delete sim;

	return 0;
}
