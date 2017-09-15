#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "lxi.h"

int main(int argc, char *argv) {
	int i, s;
	char buffer[0x400];
	const char * const cmds[] = {
		"*IDN?",
		":WAV:FORM ASC",
		":WAV:FORM?",
		":WAV:DATA?"
	};

	LXI_Device dev;
	if (LXI_DeviceCreate(&dev, "10.0.0.11", 5025) < 0) {
		goto quit;
	}

	for (i = 0; i < sizeof(cmds)/sizeof(cmds[0]); ++i) {
		printf("> %s\n", cmds[i]);
		s = LXI_Evaluate(&dev, cmds[i], strlen(cmds[i]), buffer, sizeof(buffer));
		if (s < 0) {
			goto destroy_device;
		}
		printf("%.*s", s, buffer);

		while (s == sizeof(buffer)) {
			s = LXI_Receive(&dev, buffer, sizeof(buffer));
			if (s < 0) {
				goto destroy_device;
			}
			printf("%.*s", s, buffer);
		}

		printf("\n");
	}

destroy_device:
	LXI_DeviceDestroy(&dev);
quit:
	return 0;
}
