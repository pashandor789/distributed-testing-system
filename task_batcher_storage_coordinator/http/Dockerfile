FROM ubuntu:22.04
LABEL authors="pudge1000-7"

ENV LD_LIBRARY_PATH="/usr/local/lib"

RUN mkdir "/http_tabasco"
RUN mkdir "/configs"

COPY ./shared-libs /usr/local/lib
COPY ./build/task_batcher_storage_coordinator/http/http_tabasco /http_tabasco
COPY ./configs /configs

CMD ["/http_tabasco/http_tabasco", "--config-path", "/configs/tabasco.cfg", "--server-port", "8080"]
