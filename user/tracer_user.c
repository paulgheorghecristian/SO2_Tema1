#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include "../include/tracer.h"

#define DEVICE_PATH	"/dev/tracer"

int main(int argc, char **argv)
{
	int fd;
	int pid;
	
	fd = open(DEVICE_PATH, O_RDONLY);
	if (fd < 0) {
		perror("open");
		exit(EXIT_FAILURE);
	}
	
	scanf("%d", &pid);
	switch(argv[1][0]){
		case 'a':
			ioctl(fd, TRACER_ADD_PROCESS ,pid); 	
		break;
		case 'd':
			ioctl(fd, TRACER_REMOVE_PROCESS, pid);
		break;
	}

	char buffer[256];
	FILE *f;

	f = fopen("/proc/tracer", "rt");

	/* skip header line */
	fgets(buffer, 256, f);

	printf("%s\n", buffer);

	int a,b,c,d,e,x,g,h,i,j;

	for (i = 0; i < 5; i++) {
		fscanf(f, "%d %d %d %d %d %d %d %d %d %d",
			   &a, &b, &c, &d, &e, &x, &g, &h, &i, &j);
		printf("%d %d %d %d %d %d %d %d %d %d\n", a,b,c,d,e,x,g,h,i,j);
	}

	return 0;
}
