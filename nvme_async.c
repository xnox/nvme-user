#include <fcntl.h>
#include <stdio.h>
#include <poll.h>

int main(int argc, char **argv)
{
	static const char *perrstr;
	int err;
	unsigned int async[4];
	struct pollfd fds;

	if (argc < 2) {
		fprintf(stderr, "Usage: %s </dev/nvme#> ...\n", argv[0]);
		return 1;
	}

	fds.fd = open(argv[1], O_RDONLY);
	if (fds.fd < 0)
		goto perror;
	fds.events = POLLIN;

	printf("opened:%s fd:%d\n", argv[1], fds.fd);
	perrstr = "poll";
	err = poll(&fds, 1, -1);
	if (err <= 0)
		fprintf(stderr, "error:%d polling", err);
	else {
		printf("polled received:%x\n", fds.revents);
		perrstr = "read";
		err = read(fds.fd, &async, sizeof(async));
		if (err < 0)
			goto perror;
		printf("read from file:%s: %x %x %x %x\n", argv[1], async[0], async[1], async[2], async[3]);
	}

	close(fds.fd);
	return 0;
 perror:
	perror(perrstr);
	return 1;
}
