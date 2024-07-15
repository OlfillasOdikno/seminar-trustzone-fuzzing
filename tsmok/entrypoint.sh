#!/usr/bin/env bash
export AFL_STATSD_TAGS_FLAVOR=dogstatsd
export AFL_STATSD=1
export AFL_STATSD_HOST=statsd_exporter_tsmok
export AFL_STATSD_PORT=9125
export AFL_TMPDIR=/ramdisk

afl-fuzz -U -m none -i images/examples/optee/ta-fuzz-samples/ -o /shared -M fuzzer01 \
    -- /opt/pypy3/bin/pypy -m tsmok.examples.optee.ta_arm64_fuzz_app \
    /work/target.elf @@
