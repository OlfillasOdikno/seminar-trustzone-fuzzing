#!/usr/bin/env bash
echo 'c' | make run-only QEMU_VIRTFS_AUTOMOUNT=y QEMU_VIRTFS_HOST_DIR=/shared HOSTFWD=",hostfwd=tcp::1024-:22" &
tail -f /dev/null | unbuffer -p -noecho /work/optee/build/soc_term.py 54321 2>&1 | cat > /dev/null &
tail -f /dev/null | unbuffer -p -noecho /work/test.exp 2>&1 | cat > /dev/null
#exec /work/optee/build/soc_term.py 54320
