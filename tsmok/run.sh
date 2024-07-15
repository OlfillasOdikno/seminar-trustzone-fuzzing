#!/usr/bin/env bash

echo '/tmp/core.%e.%p' | sudo tee /proc/sys/kernel/core_pattern
sudo docker run \
        --init \
        --ulimit core=-1 \
        --rm -it \
        --user $(id -u) \
        --mount type=bind,source=/tmp/,target=/tmp/ tsmok
