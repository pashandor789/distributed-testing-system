# docker image for tests

FROM ubuntu:22.04

WORKDIR init

COPY install.sh .

RUN ./install.sh

RUN echo '1234'

WORKDIR init
