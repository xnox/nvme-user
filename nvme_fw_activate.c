#include <linux/nvme.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	static const char *perrstr;
	struct nvme_admin_cmd cmd;
	int err, fd, slot = 0, action = 1;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s [<fw-slot>] [<activate-action>]\n",
			argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	if (argc > 2) {
		perrstr = argv[2];
		if (sscanf(argv[2], "%i", &slot) != 1) {
			fprintf(stderr, "Invalid parameter:%s\n", perrstr);
			return 1;
		}
		if (slot > 7) {
			fprintf(stderr, "Invalid slot:%s\n", perrstr);
			return 1;
		}
	}
	if (argc > 3) {
		perrstr = argv[3];
		if (sscanf(argv[3], "%i", &action) != 1) {
			fprintf(stderr, "Invalid parameter:%s\n", perrstr);
			return 1;
		}
		if (action > 3) {
			fprintf(stderr, "Invalid activate action:%s\n", perrstr);
			return 1;
		}
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = nvme_admin_activate_fw;
	cmd.cdw10 = (action << 3) | slot;

	perrstr = "NVMe Firmware Activate";
	printf("%s: slot:%x action:%x\n", perrstr, slot, action);
	err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
	if (err < -1)
		goto perror;
	else if (err != 0)
		fprintf(stderr, "NVME Admin command error:%#x\n", err);
	else
		printf("%s: result:%x\n", perrstr, cmd.result);
	return err;
 perror:
	perror(perrstr);
	return 1;
}
