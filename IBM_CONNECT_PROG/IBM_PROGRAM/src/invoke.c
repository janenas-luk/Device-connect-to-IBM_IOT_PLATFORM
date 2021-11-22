#include <libubox/blobmsg_json.h>
#include <libubus.h>
#include <syslog.h>
#include "DeviceConnect.h"


int rc = 0;
enum {
	TOTAL_MEMORY,
	FREE_MEMORY,
	SHARED_MEMORY,
	BUFFERED_MEMORY,
	__MEMORY_MAX,
};

enum {
	MEMORY_DATA,
	__INFO_MAX,
};

static const struct blobmsg_policy memory_policy[__MEMORY_MAX] = {
	[TOTAL_MEMORY] = { .name = "total", .type = BLOBMSG_TYPE_INT64 },
	[FREE_MEMORY] = { .name = "free", .type = BLOBMSG_TYPE_INT64 },
	[SHARED_MEMORY] = { .name = "shared", .type = BLOBMSG_TYPE_INT64 },
	[BUFFERED_MEMORY] = { .name = "buffered", .type = BLOBMSG_TYPE_INT64 },
};

static const struct blobmsg_policy info_policy[__INFO_MAX] = {
	[MEMORY_DATA] = { .name = "memory", .type = BLOBMSG_TYPE_TABLE },
};

static void board_cb(struct ubus_request *req, int type, struct blob_attr *msg) 
{
	struct MemoryD *buf = (struct MemoryD *)req->priv;
	struct blob_attr *tb[__INFO_MAX];
	struct blob_attr *memory[__MEMORY_MAX];

	blobmsg_parse(info_policy, __INFO_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[MEMORY_DATA]) 
	{
		syslog(LOG_ERR, "No memory data received\n");
		rc=-1;
		return;
	}

	blobmsg_parse(memory_policy, __MEMORY_MAX, memory,
			blobmsg_data(tb[MEMORY_DATA]), blobmsg_data_len(tb[MEMORY_DATA]));

	buf->total_memory= blobmsg_get_u64(memory[TOTAL_MEMORY]);
	buf->free_memory= blobmsg_get_u64(memory[FREE_MEMORY]);
	buf->shared_memory = blobmsg_get_u64(memory[SHARED_MEMORY]);
	buf->buffered_memory = blobmsg_get_u64(memory[BUFFERED_MEMORY]);
	//
}

int getmemoryinfo(struct ubus_context **ctx, struct MemoryD *memory)
{
	uint32_t id;
	int rc = 0;
	if (ubus_lookup_id(*ctx, "system", &id) || ubus_invoke(*ctx, id, "info", NULL, board_cb, memory, 3000)) {
		syslog(LOG_ERR, "cannot request memory info from procd\n");
		rc=-1;
	}

	return rc;
}


int ConnectUbus(struct ubus_context **ctx)
{
	int rc = 0;
	*ctx = ubus_connect(NULL);
	if (!*ctx) {
		syslog(LOG_ERR, "Failed to connect to ubus\n");
		rc = -1;
	}

	return rc;
}



