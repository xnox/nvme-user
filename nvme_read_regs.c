/**
 * Prints the values of the nvme register map. Use the address from Region 0 of
 * the nvme device as seen from 'lspci -vv -s <your dev b:d.f>'.
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <inttypes.h>
#include <sys/mman.h>
#include <unistd.h>

int main(int argc, char **argv)
{
	unsigned long bar;
	int fd;
	void *regs;

	if (argc != 2) {
		fprintf(stderr, "%s 0x<bar>\n", argv[0]);
		return 1;
	}
	if (sscanf(argv[1], "0x%lx\n", &bar) != 1) {
		fprintf(stderr, "%s: invalid param:%s\n", argv[0], argv[1]);
		return 1;
	}
	if (bar & 0x7) {
		fprintf(stderr, "%s: bad offset:%s\n", argv[0], argv[1]);
		return 1;
	}

	fd = open("/dev/mem", O_RDONLY | O_SYNC);
	if (fd < 0) {
		fprintf(stderr, "failed to open mem\n");
		return 1;
	}

	regs = mmap(0, getpagesize(), PROT_READ, MAP_SHARED, fd, bar);
	if (!regs) {
		fprintf(stderr, "failed to map device bar\n");
		return 1;
	}

	printf("cap     : %"PRIx64"\n", (*(uint64_t *)regs));
	printf("version : %x\n", (*(uint32_t *)(regs + 0x8)));
	printf("intms   : %x\n", (*(uint32_t *)(regs + 0xc)));
	printf("intmc   : %x\n", (*(uint32_t *)(regs + 0x10)));
	printf("cc      : %x\n", (*(uint32_t *)(regs + 0x14)));
	printf("csts    : %x\n", (*(uint32_t *)(regs + 0x1c)));
	printf("aqa     : %x\n", (*(uint32_t *)(regs + 0x24)));
	printf("asq     : %"PRIx64"\n", (*(uint64_t *)(regs + 0x28)));
	printf("acq     : %"PRIx64"\n", (*(uint64_t *)(regs + 0x30)));

	return 0;
}
