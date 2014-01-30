/*
 * nvme_irqcoal.c
 * Copyright (c) 2013, Intel Corporation
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *  * Neither the name of Intel Corporation nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

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
	struct nvme_admin_cmd cmd;

	if ((argc != 2) && (argc != 4)) {
		fprintf(stderr, "Usage: %s <device> [<threshold> <time>]\n",
				argv[0]);
		return 1;
	}

	perrstr = argv[1];
	err = open(argv[1], O_RDWR);
	if (err < 0)
		goto perror;
	fd = err;

	memset(&cmd, 0, sizeof(cmd));

	if (argc == 4) {
		unsigned thresh = strtoul(argv[2], NULL, 10);
		unsigned time = strtoul(argv[3], NULL, 10);
		if ((thresh > 255) || (time > 255)) {
			fprintf(stderr, "Threshold and time must both be "
					"less than 255\n");
			return 1;
		}

		cmd.opcode = nvme_admin_set_features;
		cmd.cdw10 = 8;
		cmd.cdw11 = thresh | (time << 8);
		perrstr = "Set Features";
		err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
		if (err != 0)
			goto perror;
	}

	cmd.opcode = nvme_admin_get_features;
	cmd.cdw10 = 8;
	perrstr = "Get Features";
	err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
	if (err != 0)
		goto perror;
	printf("Threshold set to %d, Time set to %d\n", cmd.result & 0xff,
							cmd.result >> 8);

	return 0;

 perror:
	if (err < 0)
		perror(perrstr);
	else
		fprintf(stderr, "%s returned NVMe status code %x\n", perrstr,
				err);
	return 1;
}
