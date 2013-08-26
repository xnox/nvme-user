#include <linux/nvme.h>
#include "nvme_features.h"
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "nvme_identify.h"

int main(int argc, char **argv)
{
	static const char *perrstr;
	struct nvme_admin_cmd cmd;
	int err, fd, nsid, fmt, i;
	struct nvme_id_ns ns;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <device> [<lbaf>]\n",
			argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	perrstr = "Getting namespace ID";
	nsid = ioctl(fd, NVME_IOCTL_ID);
	if (nsid < 0 )
		goto perror;

	perrstr = "Identifying namespace";
	err = identify(fd, nsid, &ns, 0);
	if (err < 0)
		goto perror;

	if (argc == 2) {
		printf("LBA formats:\n");
		for (i = 0; i <= ns.nlbaf; i++) {
			printf("lbaf %2d : ms:%-2d ds:%-2d rp:%#x %s\n", i,
				ns.lbaf[i].ms, ns.lbaf[i].ds, ns.lbaf[i].rp,
				i == (ns.flbas & 0xf) ? "(in use)" : "");
		}

		printf("Enter format index: ");
		if (scanf("%d", &fmt) != 1) {
			printf("Invalid input for format\n");
			return -1;
		}
	} else if (argc == 3) {
		fmt = atoi(argv[2]);
	}
	if (fmt < 0 || fmt > ns.nlbaf) {
		printf("Invalid format:%d\n", fmt);
		return -1;
	}
	printf("Entered %d, formatting namespace\n", fmt);

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = nvme_admin_format_nvm;
	cmd.nsid = nsid;
	cmd.cdw10 = fmt;

	perrstr = "Format NVM";
	err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
	if (err < 0)
		goto perror;
	else if (err != 0)
		fprintf(stderr, "NVME Admin command error:%d\n", err);
	else
		printf("Success formatting namespace:%d\n", nsid);
	return err;
 perror:
	perror(perrstr);
	return 1;
}

