#include <tee_internal_api.h>
#include <tee_internal_api_extensions.h>
#include <string.h>

#include <benchmark_ta.h>
#include <ditto_ta.h>
#include "func_extended.h"

/*
 * Called when the instance of the TA is created. This is the first call in
 * the TA.
 */
TEE_Result TA_CreateEntryPoint(void)
{
	DMSG("has been called");

	return TEE_SUCCESS;
}

/*
 * Called when the instance of the TA is destroyed if the TA has not
 * crashed or panicked. This is the last call in the TA.
 */
void TA_DestroyEntryPoint(void)
{
	DMSG("has been called");
}

/*
 * Called when a new session is opened to the TA. *sess_ctx can be updated
 * with a value to be able to identify this session in subsequent calls to the
 * TA. In this function you will normally do the global initialization for the
 * TA.
 */
TEE_Result TA_OpenSessionEntryPoint(uint32_t param_types,
									TEE_Param __maybe_unused params[4],
									void __maybe_unused **sess_ctx)
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE);

	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	/* Unused parameters */
	(void)&params;
	(void)&sess_ctx;

	IMSG("Opened Test!\n");

	/* If return value != TEE_SUCCESS the session will not be created. */
	return TEE_SUCCESS;
}

/*
 * Called when a session is closed, sess_ctx hold the value that was
 * assigned by TA_OpenSessionEntryPoint().
 */
void TA_CloseSessionEntryPoint(void __maybe_unused *sess_ctx)
{
	(void)&sess_ctx; /* Unused parameter */
	IMSG("Goodbye!\n");
}

static TEE_Result test_oob_write(uint32_t param_types,
								 TEE_Param params[4])
{
	volatile char buf[16];
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_VALUE_INPUT,
											   TEE_PARAM_TYPE_VALUE_INPUT,
											   TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE);
	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	memset(buf, 0, sizeof(buf));

	IMSG("index: %u from NW", params[0].value.a);
	IMSG("value: %u from NW", (char)params[1].value.a);
	if (params[0].value.a < 100)
	{
		buf[params[0].value.a] = (char)params[1].value.a;
		IMSG("written");
	}

	return TEE_SUCCESS;
}

static TEE_Result test_type_confusion(uint32_t param_types,
									  TEE_Param params[4])
{
	DMSG("has been called");

	if (params[0].memref.size < 4)
	{
		return TEE_ERROR_BAD_PARAMETERS;
	}

	IMSG("buf: %p val: %08x", params[0].memref.buffer, params[1].value.a);
	((int32_t *)params[0].memref.buffer)[0] = params[1].value.a;

	return TEE_SUCCESS;
}

static TEE_Result test_double_free(uint32_t param_types, TEE_Param params[4])
{
	char *data;
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE);
	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	DMSG("malloc");

	data = TEE_Malloc(16, 0);

	DMSG("First free");

	TEE_Free(data);

	DMSG("Double free");

	TEE_Free(data);

	return TEE_SUCCESS;
}

static TEE_Result test_null_deref(uint32_t param_types, TEE_Param params[4])
{
	uint32_t exp_param_types = TEE_PARAM_TYPES(TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE,
											   TEE_PARAM_TYPE_NONE);
	DMSG("has been called");

	if (param_types != exp_param_types)
		return TEE_ERROR_BAD_PARAMETERS;

	DMSG("doing null-deref %08x\n", *(char *)0);
	return TEE_SUCCESS;
}

static TEE_Result always_crash(uint32_t param_types, TEE_Param params[4])
{

	DMSG("has been called");

	DMSG("doing always_crash");
	*(char *)0xFFFFFFFF = 0;
	DMSG("done always_crash");

	return TEE_SUCCESS;
}

static TEE_Result always_crash_hard(uint32_t param_types, TEE_Param params[4])
{

	DMSG("has been called");

	DMSG("doing always_crash_hard");
	*(char *)0xFFFFFFFF = 0;
	DMSG("done always_crash_hard");

	return TEE_SUCCESS;
}

/*
 * Called when a TA is invoked. sess_ctx hold that value that was
 * assigned by TA_OpenSessionEntryPoint(). The rest of the paramters
 * comes from normal world.
 */
TEE_Result __TA_InvokeCommandEntryPoint(void __maybe_unused *sess_ctx,
										uint32_t cmd_id,
										uint32_t param_types, TEE_Param params[4])
{
	TEE_Result ret;
	(void)&sess_ctx; /* Unused parameter */

	switch (cmd_id)
	{
	case TA_BENCHMARK_CMD_OOB_WRITE:
		ret = test_oob_write(param_types, params);
		break;
	case TA_BENCHMARK_CMD_DOUBLE_FREE:
		ret = test_double_free(param_types, params);
		break;
	case TA_BENCHMARK_CMD_NULL_DEREF:
		ret = test_null_deref(param_types, params);
		break;
	case TA_BENCHMARK_CMD_TYPE_CONFUSION:
		ret = test_type_confusion(param_types, params);
		break;
	case TA_BENCHMARK_CMD_ALWAYS_CRASH_EZ:
		ret = always_crash(param_types, params);
		break;
	case TA_BENCHMARK_CMD_ALWAYS_CRASH_HARD:
		ret = always_crash_hard(param_types, params);
		break;
	default:
		ret = TEE_ERROR_BAD_PARAMETERS;
		break;
	}
	DMSG("Invoke end");

	return ret;
}

TEE_Result TA_InvokeCommandEntryPoint(void __maybe_unused *sess,
									  uint32_t cmd_id, uint32_t param_types,
									  TEE_Param params[4])
{
	TEE_Result res;

	switch (cmd_id)
	{
	case CMD_FETCH_TA_ADDRS:
		return func_fetch_ta_addrs(param_types, params);
	case CMD_SET_DITTO_PRINTF:
		return func_set_ditto_printf(param_types, params);
	case CMD_COPY_FROM_TA:
		return func_copy_from_ta(param_types, params);
	case CMD_COPY_TO_TA:
		return func_copy_to_ta(param_types, params);
	case CMD_PROXY_SYSCALL:
		return func_proxy_syscall(param_types, params);

	default:
		asm volatile("sub sp,sp,0x100");
		res = __TA_InvokeCommandEntryPoint(sess, cmd_id, param_types, params);
		asm volatile("add sp,sp,0x100");

		return res;
	}
}
