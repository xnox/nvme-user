#ifndef NVME_IDENTIFY
#define NVME_IDENTIFY

int identify(int fd, int namespace, void *ptr, int cns);

#endif
