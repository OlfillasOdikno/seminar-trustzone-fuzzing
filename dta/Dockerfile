FROM ubuntu:noble-20240407.1
ENV DEBIAN_FRONTEND=noninteractive
RUN \
  --mount=type=cache,target=/var/cache/apt,sharing=locked \
  --mount=type=cache,target=/var/lib/apt,sharing=locked \
  --mount=type=bind,source=./repro-sources-list.sh,target=/usr/local/bin/repro-sources-list.sh \
  repro-sources-list.sh && \
  apt-get update && \
  apt-get install -y \
  adb \
  acpica-tools \
  autoconf \
  automake \
  bc \
  bison \
  build-essential \
  ccache \
  cpio \
  cscope \
  curl \
  device-tree-compiler \
  e2tools \
  expect \
  fastboot \
  flex \
  ftp-upload \
  gdisk \
  git \
  libattr1-dev \
  libcap-ng-dev \
  libfdt-dev \
  libftdi-dev \
  libglib2.0-dev \
  libgmp3-dev \
  libhidapi-dev \
  libmpc-dev \
  libncurses5-dev \
  libpixman-1-dev \
  libslirp-dev \
  libssl-dev \
  libtool \
  libusb-1.0-0-dev \
  make \
  mtools \
  netcat-openbsd \
  ninja-build \
  python3-cryptography \
  python3-pip \
  python3-pyelftools \
  python3-serial \
  python-is-python3 \
  rsync \
  repo \
  swig \
  sed \
  unzip \
  uuid-dev \
  wget \
  xdg-utils \
  xterm \
  xz-utils \
  zlib1g-dev \
  && \
  : "Clean up for improving reproducibility (optional)" && \
  rm -rf /var/log/* /var/cache/ldconfig/aux-cache

WORKDIR /work

RUN mkdir dta && cd dta && git init && \
    git remote add origin https://github.com/juhyun167/dta.git && \
    git fetch origin 7f593ed6743ccb41336570065b6ddea609b2c782 && \
    git reset --hard FETCH_HEAD
RUN mkdir optee && cd optee && repo init -u https://github.com/OP-TEE/manifest.git -m qemu_v8.xml -b 4.2.0 && repo sync -j10
WORKDIR /work/optee/build
RUN make -j2 toolchains

RUN cp -r /work/dta/build/br-ext/package/aflplusplus_ext /work/optee/build/br-ext/package/aflplusplus_ext 
RUN sed -i '/BR2_PACKAGE_KEYUTILS ?= y/a BR2_PACKAGE_AFLPLUSPLUS_EXT ?= y' /work/optee/build/common.mk && \
    echo 'source "$BR2_EXTERNAL_OPTEE_PATH/package/aflplusplus_ext/Config.in"' >> /work/optee/build/br-ext/Config.in
ENV FORCE_UNSAFE_CONFIGURE=1

ENV CCACHE_DIR=/ccache
RUN --mount=type=cache,target=/ccache/ ccache make -j$(nproc) 
RUN --mount=type=cache,target=/ccache/ rm -rf ../qemu/build && ccache make -j$(nproc) QEMU_VIRTFS_ENABLE=y
COPY benchmark_dta /work/optee/optee_examples/benchmark_dta
RUN --mount=type=cache,target=/ccache/ ccache make -j$(nproc) QEMU_VIRTFS_ENABLE=y QEMU_VIRTFS_AUTOMOUNT=y
RUN --mount=type=cache,target=/ccache/ ccache -s

COPY qemu.patch /work/qemu.patch

RUN git apply < /work/qemu.patch

COPY entrypoint.sh /entrypoint.sh
COPY test.exp /work/test.exp

CMD ["/entrypoint.sh"]