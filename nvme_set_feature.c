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
	int err, fd, f, v;

	if (argc != 4) {
		fprintf(stderr, "Usage: %s <device> <feature> <value>\n",
			argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	perrstr = argv[2];
	if (sscanf(argv[2], "%d", &f) != 1)
		goto bad_param;

	perrstr = argv[3];
	if (sscanf(argv[3], "%d", &v) != 1)
		goto bad_param;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = nvme_admin_set_features;
	cmd.cdw10 = f;
	cmd.cdw11 = v;

	perrstr = "NVMe Set Feature";
	err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
	if (err == -1)
		goto perror;
	else if (err != 0)
		fprintf(stderr, "NVME Admin command error:%d\n", err);
	else
		printf("Set feature:%s value:%d, result:%d\n",
			nvme_feature_to_string(f), v, cmd.result);
	return err;
 perror:
	perror(perrstr);
	return 1;
 bad_param:
	fprintf(stderr, "Invalid parameter:%s\n", perrstr);
	return 1;
}
