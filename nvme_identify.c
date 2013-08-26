#include <linux/nvme.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include "nvme_identify.h"

int identify(int fd, int namespace, void *ptr, int cns)
{
	struct nvme_admin_cmd cmd;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = nvme_admin_identify;
	cmd.nsid = namespace;
	cmd.addr = (unsigned long)ptr;
	cmd.data_len = 4096;
	cmd.cdw10 = cns;

	return ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
}
