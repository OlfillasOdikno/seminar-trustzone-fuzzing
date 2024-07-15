#include <err.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <tee_client_api.h>
#include <benchmark_ta.h>

#include "ditto.h"
#include "setup.h"

#define SENDTO_TA 0
#define SENDTO_DITTO 1

#define INVOKE_COMMAND(session, cmd_id, operation, error_origin)           \
	((dest == SENDTO_TA) ? TEEC_InvokeCommand((session), (cmd_id),         \
											  (operation), (error_origin)) \
						 : ditto_invoke_command((session), (cmd_id),       \
												(operation), (error_origin)))
int dest = SENDTO_TA;

int param_types[] = {TEEC_VALUE_INPUT,
					 TEEC_VALUE_OUTPUT,
					 TEEC_VALUE_INOUT,
					 TEEC_MEMREF_TEMP_INPUT,
					 TEEC_MEMREF_TEMP_OUTPUT,
					 TEEC_MEMREF_TEMP_INOUT,
					 TEEC_MEMREF_WHOLE,
					 TEEC_MEMREF_PARTIAL_INPUT,
					 TEEC_MEMREF_PARTIAL_OUTPUT,
					 TEEC_MEMREF_PARTIAL_INOUT,
					 TEEC_NONE,
					 TEEC_NONE,
					 TEEC_NONE,
					 TEEC_NONE,
					 TEEC_NONE,
					 TEEC_NONE};

/* TEE and TA context structure */
struct test_ctx
{
	TEEC_Context ctx;
	TEEC_Session sess;
} ctx;

void fill_buf(void *buf, size_t size)
{
	int read = 0;
	char *pbuf = buf;
	while (read < size)
	{
		int s = fread(pbuf, 1, size - read, stdin);
		if (s <= 0)
		{
			printf("ERROR wanted %d, got %d\n", size, read);
			exit(1);
		}
		read += s;
		pbuf += s;
	}
	return;
}

void harness(TEEC_Session *sess)
{
	TEEC_Operation op;
	TEEC_Result res;
	uint32_t err_origin;

	TEEC_Parameter params[TEEC_CONFIG_PAYLOAD_REF_COUNT];
	int s;

	uint32_t cmd;
	fill_buf(&cmd, sizeof(cmd));
	printf("cmd: %d\n", cmd);

	uint16_t types;
	s = fread(&types, 1, sizeof(types), stdin);
	if (s != sizeof(types))
	{
		printf("invalid size read: %d instead of %d\n", s, sizeof(types));
		return;
	}
	memset(&op, 0, sizeof(op));

	int param0 = param_types[((types >> 0) & 0b1111)];
	int param1 = param_types[((types >> 4) & 0b1111)];
	int param2 = param_types[((types >> 8) & 0b1111)];
	int param3 = param_types[((types >> 12) & 0b1111)];

	printf("0: %d\n", param0);
	printf("1: %d\n", param1);
	printf("2: %d\n", param2);
	printf("3: %d\n", param3);

	op.paramTypes = TEEC_PARAM_TYPES(param0, param1,
									 param2, param3);

	if (param0 == TEEC_VALUE_INPUT || param0 == TEEC_VALUE_INOUT)
	{
		fill_buf(&op.params[0].value.a, sizeof(op.params[0].value.a));
		fill_buf(&op.params[0].value.b, sizeof(op.params[0].value.b));
		printf("param0.value.a: %d\n", op.params[0].value.a);
		printf("param0.value.b: %d\n", op.params[0].value.b);
	}
	else if (param0 == TEEC_MEMREF_TEMP_INPUT || param0 == TEEC_MEMREF_TEMP_INOUT)
	{
		uint16_t size;
		fill_buf(&size, sizeof(size));
		char *buf = malloc(size);
		if (!buf)
		{
			return;
		}
		fill_buf(buf, size);
		op.params[0].tmpref.buffer = buf;
		op.params[0].tmpref.size = size;
	}

	if (param1 == TEEC_VALUE_INPUT || param1 == TEEC_VALUE_INOUT)
	{
		fill_buf(&op.params[1].value.a, sizeof(op.params[1].value.a));
		fill_buf(&op.params[1].value.b, sizeof(op.params[1].value.b));
	}
	else if (param1 == TEEC_MEMREF_TEMP_INPUT || param1 == TEEC_MEMREF_TEMP_INOUT)
	{
		uint16_t size;
		fill_buf(&size, sizeof(size));
		char *buf = malloc(size);
		if (!buf)
		{
			return;
		}
		fill_buf(buf, size);
		op.params[1].tmpref.buffer = buf;
		op.params[1].tmpref.size = size;
	}

	if (param2 == TEEC_VALUE_INPUT || param2 == TEEC_VALUE_INOUT)
	{
		fill_buf(&op.params[2].value.a, sizeof(op.params[2].value.a));
		fill_buf(&op.params[2].value.b, sizeof(op.params[2].value.b));
	}
	else if (param2 == TEEC_MEMREF_TEMP_INPUT || param2 == TEEC_MEMREF_TEMP_INOUT)
	{
		uint16_t size;
		fill_buf(&size, sizeof(size));
		char *buf = malloc(size);
		if (!buf)
		{
			return;
		}
		fill_buf(buf, size);
		op.params[2].tmpref.buffer = buf;
		op.params[2].tmpref.size = size;
	}

	if (param3 == TEEC_VALUE_INPUT || param3 == TEEC_VALUE_INOUT)
	{
		fill_buf(&op.params[3].value.a, sizeof(op.params[3].value.a));
		fill_buf(&op.params[3].value.b, sizeof(op.params[3].value.b));
	}
	else if (param3 == TEEC_MEMREF_TEMP_INPUT || param3 == TEEC_MEMREF_TEMP_INOUT)
	{
		uint16_t size;
		fill_buf(&size, sizeof(size));
		char *buf = malloc(size);
		if (!buf)
		{
			return;
		}
		fill_buf(buf, size);
		op.params[3].tmpref.buffer = buf;
		op.params[3].tmpref.size = size;
	}

	res = INVOKE_COMMAND(sess, cmd, &op,
						 &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "INVOKE_COMMAND failed with code 0x%x origin 0x%x",
			 res, err_origin);
}

int main(int argc, char **argv)
{
	TEEC_Result res;
	TEEC_UUID uuid = TA_BENCHMARK_UUID;
	uint32_t err_origin;
	if (argc > 1 && !strcmp(argv[1], "-d"))
		dest = SENDTO_DITTO;

	/* Initialize a context connecting us to the TEE */
	res = TEEC_InitializeContext(NULL, &ctx.ctx);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_InitializeContext failed with code 0x%x", res);

	/*
	 * Open a session to the "hello world" TA, the TA will print "hello
	 * world!" in the log when the session is created.
	 */
	res = TEEC_OpenSession(&ctx.ctx, &ctx.sess, &uuid,
						   TEEC_LOGIN_PUBLIC, NULL, NULL, &err_origin);
	if (res != TEEC_SUCCESS)
		errx(1, "TEEC_Opensession failed with code 0x%x origin 0x%x",
			 res, err_origin);

	if (dest == SENDTO_DITTO)
		setup(&ctx.sess);

	harness(&ctx.sess);

	TEEC_CloseSession(&ctx.sess);

	TEEC_FinalizeContext(&ctx.ctx);

	return 0;
}
