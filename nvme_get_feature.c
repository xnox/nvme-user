#include <linux/nvme.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "nvme_features.h"

int main(int argc, char **argv)
{
	static const char *perrstr;
	struct nvme_admin_cmd cmd;
	int err, fd, f, cdw11 = 0;

	if (argc < 3) {
		fprintf(stderr, "Usage: %s <device> <feature> [<cdw11>]\n",
			argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	perrstr = argv[2];
	if (sscanf(argv[2], "%d", &f) != 1) {
		fprintf(stderr, "Invalid parameter:%s\n", perrstr);
		return 1;
	}
	if (argc > 3) {
		if (sscanf(argv[3], "%d", &cdw11) != 1) {
			fprintf(stderr, "Invalid parameter:%s\n", perrstr);
			return 1;
		}
	}

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = nvme_admin_get_features;
	cmd.cdw10 = f;
	cmd.cdw11 = cdw11;

	perrstr = "NVMe Get Feature";
	err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
	if (err == -1)
		goto perror;
	else if (err != 0)
		fprintf(stderr, "NVME Admin command error:%#x\n", err);
	else
		printf("get feature:%s, value:%d\n", nvme_feature_to_string(f),
			cmd.result);
	return err;
 perror:
	perror(perrstr);
	return 1;
}
