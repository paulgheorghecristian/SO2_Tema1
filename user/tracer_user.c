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
			ioctl(fd, TRACER_ADD_PROCESS, (void*)&pid);		
		break;
		case 'd':
			ioctl(fd, TRACER_REMOVE_PROCESS, (void*)&pid);
		break;
	}

	return 0;
}
