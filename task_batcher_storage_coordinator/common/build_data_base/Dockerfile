FROM postgres:latest
LABEL authors="pudge1000-7"

COPY ./task_batcher_storage_coordinator/common/build_data_base/create_tables.sql /docker-entrypoint-initdb.d

ENV POSTGRES_DB="postgres"
ENV POSTGRES_USER="postgres"
ENV POSTGRES_PASSWORD="1488"
