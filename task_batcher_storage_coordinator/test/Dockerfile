FROM python:3.9-alpine
LABEL authors="pudge1000-7"

RUN mkdir "test"

WORKDIR "test"

COPY . .
RUN pip install -r requirements.txt

ENTRYPOINT pytest -vs --log-level=INFO
