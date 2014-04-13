#include <linux/nvme.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "nvme_identify.h"

static void show_nvme_id_ns(struct nvme_id_ns *ns, int id)
{
	int i;

	printf("\nNVME Identify Namespace %d:\n\n", id);
	printf("nsze    : %lld\n", ns->nsze);
	printf("ncap    : %lld\n", ns->ncap);
	printf("nuse    : %lld\n", ns->nuse);
	printf("nsfeat  : %#x\n", ns->nsfeat);
	printf("nlbaf   : %d\n", ns->nlbaf);
	printf("flbas   : %#x\n", ns->flbas);
	printf("mc      : %#x\n", ns->mc);
	printf("dpc     : %#x\n", ns->dpc);
	printf("dps     : %#x\n", ns->dps);
	printf("_euid64 : %02x%02x%02x%02x%02x%02x%02x%02x\n",
	       ns->rsvd30[90],
	       ns->rsvd30[91],
	       ns->rsvd30[92],
	       ns->rsvd30[93],
	       ns->rsvd30[94],
	       ns->rsvd30[95],
	       ns->rsvd30[96],
	       ns->rsvd30[97]
	    );

	for (i = 0; i <= ns->nlbaf; i++) {
		printf("lbaf %2d : ms:%-2d ds:%-2d rp:%#x %s\n", i,
			ns->lbaf[i].ms, ns->lbaf[i].ds, ns->lbaf[i].rp,
			i == (ns->flbas & 0xf) ? "(in use)" : "");
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	static const char *perrstr;
	int err, fd, nsid;
	struct nvme_id_ns ns;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s <device> <nsid(optional)>\n", argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	if (argc == 2) {
		perrstr = "Getting namespace ID";
		nsid = ioctl(fd, NVME_IOCTL_ID);
		if (nsid == -1)
			goto perror;
	}
	else {
		if (sscanf(argv[2], "%d", &nsid) != 1) {
			fprintf(stderr, "Invalid parameter:%s\n", argv[2]);
			return 1;
		}
	}

	perrstr = "Identifying namespace";
	err = identify(fd, nsid, &ns, 0);
	if (err < 0)
		goto perror;
	else if (err != 0)
		fprintf(stderr, "NVME Admin command error:%d\n", err);
	else
		show_nvme_id_ns(&ns, nsid);

	return 0;

 perror:
	perror(perrstr);
	return 1;
}

