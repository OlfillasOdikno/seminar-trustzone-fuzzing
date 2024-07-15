#ifndef TA_BENCHMARK_H
#define TA_BENCHMARK_H


/*
 * 648d27f5-c55c-444c-80f6-1dcaa44eaa84
 */
#define TA_BENCHMARK_UUID \
	{ 0x648d27f5, 0xc55c, 0x444c, \
		{ 0x80, 0xf6, 0x1d, 0xca, 0xa4, 0x4e, 0xaa, 0x84} }

/* The function IDs implemented in this TA */
#define TA_BENCHMARK_CMD_OOB_WRITE			0
#define TA_BENCHMARK_CMD_DOUBLE_FREE		1
#define TA_BENCHMARK_CMD_NULL_DEREF			2
#define TA_BENCHMARK_CMD_TYPE_CONFUSION		3
#define TA_BENCHMARK_CMD_ALWAYS_CRASH_EZ	4
#define TA_BENCHMARK_CMD_ALWAYS_CRASH_HARD	31337

#endif /*TA_BENCHMARK_H*/
