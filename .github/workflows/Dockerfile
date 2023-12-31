# docker image for tests

FROM ubuntu:22.04

# download tools
RUN apt-get update && \
    apt-get install -y \
    curl \
    cmake \
    g++ \
    git

RUN mkdir downloads
WORKDIR downloads

# begin of mongocxx download

RUN apt-get install -y \
    libssl-dev

RUN curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.9.0/mongo-cxx-driver-r3.9.0.tar.gz && \
    tar -xzf mongo-cxx-driver-r3.9.0.tar.gz && \
    cd mongo-cxx-driver-r3.9.0/build && \
    cmake ..                                            \
        -DCMAKE_BUILD_TYPE=Release                      \
        -DBSONCXX_POLY_USE_STD=ON                       \
        -DMONGOCXX_OVERRIDE_DEFAULT_INSTALL_PREFIX=OFF  \
        -DBUILD_VERSION=3.9.0			    \
        -DCMAKE_CXX_STANDARD=17 && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of mongocxx download

# begin of grpc download

RUN apt-get update && \
    apt-get install -y  \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    libgflags-dev libgtest-dev \
    clang libc++-dev

RUN git clone --recurse-submodules -b v1.60.0 https://github.com/grpc/grpc && \
    cd grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 ../.. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of grpc download

# begin of argparse download

RUN git clone https://github.com/p-ranav/argparse.git && \
    cd argparse && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of argparse download

# begin of nlohmann/json download

RUN git clone https://github.com/nlohmann/json.git && \
    cd json && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of nlohmann/json download

# begin of crow download

RUN apt-get install -y \
    libboost-all-dev \
    libasio-dev \
    libev-dev

RUN git clone --recurse-submodules https://github.com/CrowCpp/Crow.git && \
    cd Crow && \
    mkdir build && \
    cd build && \
    cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of crow download

# begin of ampqcpp download

RUN git clone https://github.com/CopernicaMarketingSoftware/AMQP-CPP.git && \
    cd AMQP-CPP && \
    mkdir build && \
    cd build && \
    cmake .. -DAMQP-CPP_LINUX_TCP=ON -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of ampqcpp download

# begin of protobuf download

RUN git clone -b v3.17.3 --recurse-submodules https://github.com/protocolbuffers/protobuf.git && \
    cd protobuf && \
    ./autogen.sh && \
    ./configure && \
    make -j $(nproc) && \
    make install && \
    ldconfig

# end of protobuf download

# begin of catch2 download

RUN git clone https://github.com/catchorg/Catch2.git && \
    cd Catch2 && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of catch2 download

RUN apt-get install -y \
    libsnappy-dev \
    libpqxx-dev

WORKDIR /