
#ifndef _NVME_FEATURES
#define _NVME_FEATURES

char* nvme_feature_to_string(int feature)
{
	switch(feature)
	{
	case NVME_FEAT_ARBITRATION:
		return "Arbitration";
	case NVME_FEAT_POWER_MGMT:
		return "Power Management";
	case NVME_FEAT_LBA_RANGE:
		return "LBA Range";
	case NVME_FEAT_TEMP_THRESH:
		return "Temperature Threshold";
	case NVME_FEAT_ERR_RECOVERY:
		return "Error Recovery";
	case NVME_FEAT_VOLATILE_WC:
		return "Volatile Write Cache";
	case NVME_FEAT_NUM_QUEUES:
		return "Number of Queues";
	case NVME_FEAT_IRQ_COALESCE:
		return "IRQ Coalescing";
	case NVME_FEAT_IRQ_CONFIG:
		return "IRQ Configuration";
	case NVME_FEAT_WRITE_ATOMIC:
		return "Write Atomicity";
	case NVME_FEAT_ASYNC_EVENT:
		return "Async Event";
	case NVME_FEAT_SW_PROGRESS:
		return "Software Progress";
	default:
		return "Unknown";
	}
}

#endif

