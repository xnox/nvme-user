#include <linux/nvme.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define min(x, y) x > y ? y : x;

int main(int argc, char **argv)
{
	struct nvme_admin_cmd cmd;
	static const char *perrstr;
	int err, fd, fw_fd, fw_size, xfer_size;
	struct stat sb;
	void *fw_buf;
	int offset = 0;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s <device> <fw_image_file>\n", argv[0]);
		return 1;
	}

	perrstr = argv[1];
	fd = open(argv[1], O_RDWR);
	if (fd < 0)
		goto perror;

	perrstr = argv[2];
	fw_fd = open(argv[2], O_RDWR);
	if (fw_fd < 0)
		goto perror;

	perrstr = "Get file size\n";
	if (stat(argv[2], &sb) != 0)
		goto perror;
	fw_size = sb.st_size;

	if (fw_size & 0x3) {
		fprintf(stderr, "%s: Invalid size for f/w image\n", argv[2]);
		return 1;
	} 

	perrstr = "Allocate firmware buffer\n";
	if (posix_memalign(&fw_buf, getpagesize(), fw_size))
		goto perror;

	perrstr = "Read firmware file\n";
	if (read(fw_fd, fw_buf, fw_size) != ((ssize_t)(fw_size)))
		goto perror;

	perrstr = "Download Firmware";
	while (fw_size > 0) {
		xfer_size = min(getpagesize(), fw_size);

		memset(&cmd, 0, sizeof(cmd));
		cmd.opcode = nvme_admin_download_fw;
		cmd.addr = (__u64)fw_buf;
		cmd.data_len = xfer_size;
		cmd.cdw10 = (xfer_size >> 2) - 1;
		cmd.cdw11 = offset >> 2; 

		err = ioctl(fd, NVME_IOCTL_ADMIN_CMD, &cmd);
		if (err < 0)
		        goto perror;
		else if (err != 0) {
		        fprintf(stderr,
				"%s: offset:%#x transfer size:%#x error:%x\n",
				perrstr, offset, xfer_size, err);
			break;
		}
		fw_buf += xfer_size;
		fw_size -= xfer_size;
		offset += xfer_size;
	}
	if (!err)	
		printf("Firmware Download Successful\n");
	return err;
 perror:
	perror(perrstr);
	return 1;
}
