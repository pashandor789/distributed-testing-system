FROM ubuntu:22.04

RUN apt update && \
    apt install -y \
    zip \
    g++ \
    cmake \
    python3

RUN mkdir check

COPY ./executor/executor /check/executor
