# download tools
apt-get update && \
    apt-get install -y \
    curl \
    cmake \
    g++ \
    git

mkdir downloads
cd downloads

download_dir=$(pwd)

# begin of mongocxx download

apt-get install -y \
    libssl-dev

curl -OL https://github.com/mongodb/mongo-cxx-driver/releases/download/r3.9.0/mongo-cxx-driver-r3.9.0.tar.gz && \
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

cd $download_dir

# begin of grpc download

apt-get update && \
    apt-get install -y  \
    build-essential \
    autoconf \
    libtool \
    pkg-config \
    libgflags-dev libgtest-dev \
    clang libc++-dev

git clone --recurse-submodules -b v1.60.0 https://github.com/grpc/grpc && \
    cd grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 ../.. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of grpc download

cd $download_dir

# begin of argparse download

git clone https://github.com/p-ranav/argparse.git && \
    cd argparse && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of argparse download

cd $download_dir

# begin of nlohmann/json download

git clone https://github.com/nlohmann/json.git && \
    cd json && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of nlohmann/json download

cd $download_dir

# begin of crow download

apt-get install -y \
    libboost-all-dev \
    libasio-dev \
    libev-dev

git clone --recurse-submodules https://github.com/CrowCpp/Crow.git && \
    cd Crow && \
    mkdir build && \
    cd build && \
    cmake .. -DCROW_BUILD_EXAMPLES=OFF -DCROW_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of crow download

cd $download_dir

# begin of ampqcpp download

git clone https://github.com/CopernicaMarketingSoftware/AMQP-CPP.git && \
    cd AMQP-CPP && \
    make -j $(nproc) && \
    make install

# end of ampqcpp download

cd $download_dir

# begin of protobuf download

git clone -b v3.17.3 --recurse-submodules https://github.com/protocolbuffers/protobuf.git && \
    cd protobuf && \
    ./autogen.sh && \
    ./configure && \
    make -j $(nproc) && \
    make install && \
    ldconfig

# end of protobuf download

cd $download_dir

# begin of catch2 download

git clone https://github.com/catchorg/Catch2.git && \
    cd Catch2 && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of catch2 download

cd $download_dir

apt-get install -y \
    libsnappy-dev \
    libpqxx-dev \
    docker \
    docker-compose \
    libmongocrypt-dev \
    python3-pip

pip install \
    grpcio-tools \
    pytest

# begin of curlpp download

apt-get install libcurl4-openssl-dev

git clone https://github.com/jpbarrette/curlpp.git && \
    cd curlpp && \
    mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build . -- -j $(nproc) && \
    make install

# end of curlpp download

cd $download_dir

# begin of glog download

git clone -b v0.6.0 https://github.com/google/glog.git && \
    cd glog && \
    cmake -S . -B build -G "Unix Makefiles" && \
    cmake --build build && \
    cmake --build build --target install

# end of glog download

cd $download_dir
