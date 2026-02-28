#include "../include/Simulator.h"
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

struct config getConfig(char *configFile)
{
	FILE *f = fopen(configFile, "r");
	struct config c = {4, 16, 4, 16, 4};
	fscanf(f, "%d %d %d %d %d", &c.NF, &c.NI, &c.NW, &c.NR, &c.NB);
	fclose(f);
	return c;
}

int main(int argc, char *argv[])
{
	bool debug = false;
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

	struct config config = getConfig(config_file_name);
	Simulator *sim = new Simulator(&program, config);
	sim->printStats();
	if (debug)
		sim->dump(2, DEBUG_DCACHE, DEBUG_REGISTERS);
	if (json_file_name != NULL)
	{
		std::ofstream json_output(json_file_name);
		if (!json_output.is_open())
		{
			std::cerr << "Failed to open " << json_file_name << " for writing." << std::endl;
			return 1;
		}
		sim->serializeJSON(&json_output);
		json_output.close();
	}
	delete sim;

	return 0;
}
