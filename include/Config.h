#ifndef CONFIG_H
#define CONFIG_H

#include <glib.h>
#include <glib/gprintf.h>

typedef struct
{
	int NF; // Fetch Width
	int NI; // Instruction Queue Size
	int NW; // Number of Instructions Executable
	int NR; // Reorder Buffer Size
	int NB; // Number of Common Data Buses (CDB)
} Config;

int parse_config(const char *config_file_name);

extern Config *config;
extern bool verbose;
extern bool debug;

#endif /* #define CONFIG_H */
