FROM ubuntu:22.04
LABEL authors="pudge1000-7"

# download docker-engine begin

RUN apt-get update && \
    apt-get install -y ca-certificates curl gnupg && \
    install -m 0755 -d /etc/apt/keyrings && \
    curl -fsSL https://download.docker.com/linux/ubuntu/gpg |  gpg --dearmor -o /etc/apt/keyrings/docker.gpg && \
    chmod a+r /etc/apt/keyrings/docker.gpg && \
    echo \
      "deb [arch=$(dpkg --print-architecture) signed-by=/etc/apt/keyrings/docker.gpg] https://download.docker.com/linux/ubuntu \
      $(. /etc/os-release && echo "$VERSION_CODENAME") stable" | \
      tee /etc/apt/sources.list.d/docker.list > /dev/null && \
    apt-get update && \
    apt-get install -y docker-ce docker-ce-cli

# download docker-engine end

ENV LD_LIBRARY_PATH="/usr/local/lib"

RUN mkdir "/testing_processor"
RUN mkdir "/configs"

COPY ./shared-libs /usr/local/lib
COPY ./build/testing_processor/testing_processor /testing_processor
COPY ./configs /configs

CMD ["/testing_processor/testing_processor", "--config-path", "/configs/testing_processor.cfg"]
