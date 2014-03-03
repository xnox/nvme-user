#include <linux/nvme.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	static const char *perrstr;
	int err, fd;
	char data[4096];
	struct nvme_user_io io;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <device>\n", argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	io.opcode = nvme_cmd_write;
	io.flags = 0;
	io.control = 0;
	io.metadata = (unsigned long)0;
	io.addr = (unsigned long)data;
	io.slba = 0;
	io.nblocks = 7;
	io.dsmgmt = 0;
	io.reftag = 0;
	io.apptag = 0;
	io.appmask = 0;

	err = ioctl(fd, NVME_IOCTL_SUBMIT_IO, &io);
	if (err < 0)
		goto perror;
	if (err)
		fprintf(stderr, "nvme write status:%x\n", err);

	io.opcode = nvme_cmd_read;
	err = ioctl(fd, NVME_IOCTL_SUBMIT_IO, &io);
	if (err < 0)
		goto perror;
	if (err)
		fprintf(stderr, "nvme read status:%x\n", err);

	return 0;

 perror:
	perror(perrstr);
	return 1;
}
