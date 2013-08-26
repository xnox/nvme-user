#include <linux/nvme.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <locale.h>

#include "nvme_identify.h"

static long double int128_to_double(__u8 *data)
{
	long double result = 0;
	int i;
	for (i = 0; i < 16; i++) {
		result *= 256;
		result += data[15 - i];
	}
	return result;
}

static void print_smart_log(struct nvme_smart_log *smart_log)
{
	/* convert temperature from Kelvin to Celsius */
	unsigned int temperature = ((smart_log->temperature[1] << 8) |
		smart_log->temperature[0]) - 273;

	printf("critical_warning          : %#x\n", smart_log->critical_warning);
	printf("temperature               : %u C\n", temperature);
	printf("available_spare           : %u%%\n", smart_log->avail_spare);
	printf("available_spare_threshold : %u%%\n", smart_log->spare_thresh);
	printf("percentage_used           : %u%%\n", smart_log->percent_used);

	printf("data_units_read           : %'.0Lf\n",
		int128_to_double(smart_log->data_units_read));

	printf("data_units_written        : %'.0Lf\n",
		int128_to_double(smart_log->data_units_written));

	printf("host_read_commands        : %'.0Lf\n",
		int128_to_double(smart_log->host_reads));

	printf("host_write_commands       : %'.0Lf\n",
		int128_to_double(smart_log->host_writes));

	printf("controller_busy_time      : %'.0Lf\n",
		int128_to_double(smart_log->ctrl_busy_time));

	printf("power_cycles              : %'.0Lf\n",
		int128_to_double(smart_log->power_cycles));

	printf("power_on_hours            : %'.0Lf\n",
		int128_to_double(smart_log->power_on_hours));

	printf("unsafe_shutdowns          : %'.0Lf\n",
		int128_to_double(smart_log->unsafe_shutdowns));

	printf("media_errors              : %'.0Lf\n",
		int128_to_double(smart_log->media_errors));

	printf("num_err_log_entries       : %'.0Lf\n",
		int128_to_double(smart_log->num_err_log_entries));
}

int main(int argc, char **argv)
{
	struct nvme_id_ctrl *ctrl;
	struct nvme_smart_log *smart_log;
	struct nvme_admin_cmd cmd;
	static const char *perrstr;
	int err, fd, nsid;

	if (posix_memalign((void **)&ctrl, getpagesize(), sizeof(*ctrl)))
		goto perror;

	if (posix_memalign((void **)&smart_log, getpagesize(),
				sizeof(*smart_log)))
		goto perror;

	setlocale(LC_ALL, "");

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <device>\n", argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	perrstr = "Getting namespace ID";
	nsid = ioctl(fd, NVME_IOCTL_ID);
	if (nsid < 0)
		nsid = 0;

	perrstr = "Identifying controller";
	err = identify(fd, 0, ctrl, 1);
	if (err < 0)
		goto perror;

	memset(&cmd, 0, sizeof(cmd));
	cmd.opcode = nvme_admin_get_log_page;
	cmd.addr = (unsigned long)smart_log;
	cmd.data_len = sizeof(*smart_log);
	cmd.cdw10 = 0x2 | ((sizeof(*smart_log) / 4) << 16);

	if (ctrl->lpa & 1 && nsid != 0) {
		/* supports smart log on individual namespaces */
		cmd.nsid = nsid;

		perrstr = "Get Namespace Smart Log";
		err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
		if (err < 0)
			goto perror;

		else if (err != 0)
			fprintf(stderr, "Error getting ns smart log:%d\n", err);
		else {
			printf("Smart Log for NVME device namespace:%x\n",
				nsid);
			print_smart_log(smart_log);
		}
	} else {
		/* get the smart log for the entire device */
		cmd.nsid = 0xffffffff;

		perrstr = "Get Device Smart Log";
		err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
		if (err < 0)
			goto perror;
		else if (err != 0)
			fprintf(stderr, "Error getting ctrl smart log:%d\n",
				err);
		else {
			printf("Smart Log for NVME device\n");
			print_smart_log(smart_log);
		}
	}

	free(ctrl);
	free(smart_log);
	return err;

 perror:
	perror(perrstr);
	return 1;
}

