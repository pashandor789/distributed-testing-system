FROM ubuntu:22.04

RUN apt update && \
    apt install -y \
    zip \
    g++ \
    cmake

RUN mkdir check

COPY ./build/executor/executor /check/executor
