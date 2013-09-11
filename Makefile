CFLAGS := -m64 -O2 -g -pthread -D_GNU_SOURCE -D_REENTRANT -W
LDFLAGS := -m64 -lm
NVME_PROGS = nvme_rw nvme_smart nvme_set_feature nvme_get_feature nvme_id_ns \
	nvme_id_ctrl nvme_format_ns nvme_async nvme_read_regs
ALL_PROGS := $(NVME_PROGS)
default: $(ALL_PROGS)
clean:
	rm -f $(ALL_PROGS) *.o

clobber: clean

nvme_format_ns nvme_smart nvme_id_ns nvme_id_ctrl: nvme_identify.o

.PHONY: default clean clobber
