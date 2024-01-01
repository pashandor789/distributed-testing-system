FROM ubuntu:22.04
LABEL authors="pudge1000-7"

ENV LD_LIBRARY_PATH="/usr/local/lib"

RUN mkdir "/grpc_tabasco"
RUN mkdir "/configs"

COPY ./shared-libs /usr/local/lib
COPY ./build/task_batcher_storage_coordinator/grpc/grpc_tabasco /grpc_tabasco
COPY ./configs /configs

CMD ["/grpc_tabasco/grpc_tabasco", "--config-path", "/configs/tabasco.cfg", "--server-port", "9090"]
