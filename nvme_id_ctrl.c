#include <linux/nvme.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

#include "nvme_identify.h"

static void show_nvme_id_ctrl(struct nvme_id_ctrl *ctrl)
{
	int i;
	char sn[sizeof(ctrl->sn) + 1];
	char mn[sizeof(ctrl->mn) + 1];
	char fr[sizeof(ctrl->fr) + 1];

	memcpy(sn, ctrl->sn, sizeof(ctrl->sn));
	memcpy(mn, ctrl->mn, sizeof(ctrl->mn));
	memcpy(fr, ctrl->fr, sizeof(ctrl->fr));

	sn[sizeof(ctrl->sn)] = '\0';
	mn[sizeof(ctrl->mn)] = '\0';
	fr[sizeof(ctrl->fr)] = '\0';

	printf("\nNVME Identify Controller:\n\n");
	printf("vid     : %#x\n", ctrl->vid);
	printf("ssvid   : %#x\n", ctrl->ssvid);
	printf("sn      : %s\n", sn);
	printf("mn      : %s\n", mn);
	printf("fr      : %s\n", fr);
	printf("rab     : %d\n", ctrl->rab);
	printf("ieee    : %02x%02x%02x\n", ctrl->ieee[0], ctrl->ieee[1],
							ctrl->ieee[2]);
	printf("mic     : %#x\n", ctrl->mic);
	printf("mdts    : %d\n", ctrl->mdts);
	printf("oacs    : %#x\n", ctrl->oacs);
	printf("acl     : %d\n", ctrl->acl);
	printf("aerl    : %d\n", ctrl->aerl);
	printf("frmw    : %#x\n", ctrl->frmw);
	printf("lpa     : %#x\n", ctrl->lpa);
	printf("elpe    : %d\n", ctrl->elpe);
	printf("npss    : %d\n", ctrl->npss);
	printf("sqes    : %#x\n", ctrl->sqes);
	printf("cqes    : %#x\n", ctrl->cqes);
	printf("nn      : %d\n", ctrl->nn);
	printf("oncs    : %#x\n", ctrl->oncs);
	printf("fuses   : %#x\n", ctrl->fuses);
	printf("fna     : %#x\n", ctrl->fna);
	printf("vwc     : %#x\n", ctrl->vwc);
	printf("awun    : %d\n", ctrl->awun);
	printf("awupf   : %d\n", ctrl->awupf);

	for (i = 0; i <= ctrl->npss; i++) {
		printf("ps %4d : mp:%d enlat:%d exlat:%d rrt:%d rrl:%d rwt:%d rwl:%d\n",
			i, ctrl->psd[i].max_power, ctrl->psd[i].entry_lat,
			ctrl->psd[i].exit_lat, ctrl->psd[i].read_tput,
			ctrl->psd[i].read_lat, ctrl->psd[i].write_tput,
			ctrl->psd[i].write_lat);
	}
	printf("\n");
}

int main(int argc, char **argv)
{
	static const char *perrstr;
	int err, fd;
	struct nvme_id_ctrl ctrl;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <device>\n", argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	perrstr = "Identifying controller";
	err = identify(fd, 0, &ctrl, 1);
	if (err < 0)
		goto perror;
	else if (err != 0)
		fprintf(stderr, "NVME Admin command error:%d\n", err);
	else
		show_nvme_id_ctrl(&ctrl);

	return err;

 perror:
	perror(perrstr);
	return 1;
}
