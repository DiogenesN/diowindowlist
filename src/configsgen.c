/* creates initial config directory and file */

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>

void create_configs() {
	const char *HOME = getenv("HOME");

	if (HOME == NULL) {
		fprintf(stderr, "Unable to determine the user's home directory.\n");
		return;
	}

	char dirConfigBuff[777];
	char fileConfigBuff[777];
	const char *dirConfig	= "/.config/diowindowlist";
	const char *fileConfig	= "/diowindowlist.conf";

	snprintf(dirConfigBuff, sizeof(dirConfigBuff), "%s%s", HOME, dirConfig);
	snprintf(fileConfigBuff, sizeof(fileConfigBuff), "%s%s%s", HOME, dirConfig, fileConfig);

	DIR *confDir = opendir(dirConfigBuff);

	// cheks if the file already exists
	if (confDir) {
		// directory exists do nothing
		closedir(confDir);
		return;
	}
	else {
		// directory not found
		printf("created initial configs\n");
		mkdir(dirConfigBuff, 0755);
		closedir(confDir);
	}

	FILE *config = fopen(fileConfigBuff, "w+");
	
	fprintf(config, "%s\n", "width=100");
	fprintf(config, "%s\n", "height=100");
	fprintf(config, "%s\n", "posx=700");
	fprintf(config, "%s\n", "posy=700");

	fclose(config);
}
