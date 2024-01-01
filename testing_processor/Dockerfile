FROM ubuntu:22.04
LABEL authors="pudge1000-7"

RUN apt-get update && \
    apt-get install -y docker-compose # for /usr/bin/docker

ENV LD_LIBRARY_PATH="/usr/local/lib"

RUN mkdir "/testing_processor"
RUN mkdir "/configs"

COPY ./shared-libs /usr/local/lib
COPY ./build/testing_processor/testing_processor /testing_processor
COPY ./configs /configs

CMD ["/testing_processor/testing_processor", "--config-path", "/configs/testing_processor.cfg"]
