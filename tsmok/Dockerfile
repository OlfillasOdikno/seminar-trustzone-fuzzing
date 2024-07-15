FROM aflplusplus/aflplusplus


WORKDIR /work

RUN pip uninstall --yes unicornafl unicorn

RUN cd /tmp/ && wget https://downloads.python.org/pypy/pypy3.10-v7.3.16-linux64.tar.bz2 && \
    tar xf pypy3.10-v7.3.16-linux64.tar.bz2 && mv pypy3.10-v7.3.16-linux64 /opt/pypy3

RUN /opt/pypy3/bin/pypy -m ensurepip

RUN git clone https://github.com/dmitryya/unicornafl.git -b tee-dev
RUN cd unicornafl/bindings/python && /opt/pypy3/bin/pypy setup.py install

RUN git clone https://github.com/google/tsmok.git && cd tsmok && git checkout f22945be2cdaab939d1b30cac237de4cfd35d8c5

WORKDIR /work/tsmok

COPY ta_fuzz.py /work/tsmok/tsmok/optee/fuzzing/ta_fuzz.py
COPY fix.patch /work
RUN git apply < /work/fix.patch

RUN /opt/pypy3/bin/pypy -m pip install -r requirements.txt && /opt/pypy3/bin/pypy setup.py install
RUN /opt/pypy3/bin/pypy -m pip install lark

COPY 648d27f5-c55c-444c-80f6-1dcaa44eaa83.elf /work/target.elf

RUN rm -rf images/examples/optee/ta-fuzz-samples/* && \
    /opt/pypy3/bin/pypy -m tsmok.examples.optee.optee_gen_samples \
    --sample-dir images/examples/optee/ta-fuzz-samples/ --mode ta

COPY entrypoint.sh /entrypoint.sh

ENTRYPOINT [ "/entrypoint.sh" ]