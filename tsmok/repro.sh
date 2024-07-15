#!/usr/bin/env bash

sudo docker run \
        --init \
        --ulimit core=-1 \
        --rm -it \
        --user $(id -u) \
        --entrypoint /opt/pypy3/bin/pypy \
        --mount type=bind,source=/tmp/,target=/tmp/ tsmok \
        -m tsmok.examples.optee.ta_arm64_fuzz_app /work/target.elf -v $1
