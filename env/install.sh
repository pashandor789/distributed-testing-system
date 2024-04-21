set -e

# download tools
apt-get update && \
    apt-get install -y \
    g++ \
    git \
    autoconf \
    automake \
    libtool \
    curl \
    make \
    wget \
    libssl-dev \
    build-essential \
    unzip || exit 1

mkdir downloads
cd downloads

download_dir=$(pwd)

# begin of cmake download

wget https://github.com/Kitware/CMake/releases/download/v3.28.1/cmake-3.28.1.tar.gz && \
    tar -zxvf cmake-3.28.1.tar.gz && \
    cd cmake-3.28.1 && \
    ./bootstrap && \
    make -j $(nproc) && \
    make install || exit 1

# end of cmake download

cd $download_dir

# begin of mongocxx download

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
    make install || exit 1

# end of mongocxx download

cd $download_dir

# begin of grpc download

apt-get update && \
    apt-get install -y  \
    pkg-config \
    libgflags-dev libgtest-dev \
    clang libc++-dev || exit 1

git clone --recurse-submodules -b v1.60.0 https://github.com/grpc/grpc && \
    cd grpc && \
    mkdir -p cmake/build && \
    cd cmake/build && \
    cmake -DgRPC_INSTALL=ON -DgRPC_BUILD_TESTS=OFF -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_STANDARD=17 ../.. && \
    cmake --build . -- -j $(nproc) && \
    make install || exit 1

# end of grpc download

cd $download_dir

# begin of argparse download

git clone https://github.com/p-ranav/argparse.git && \
    cd argparse && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install || exit 1

# end of argparse download

cd $download_dir

# begin of nlohmann/json download

git clone https://github.com/nlohmann/json.git && \
    cd json && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install || exit 1

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
    make install || exit 1

# end of crow download

cd $download_dir

# begin of ampqcpp download

git clone https://github.com/CopernicaMarketingSoftware/AMQP-CPP.git && \
    cd AMQP-CPP && \
    make -j $(nproc) && \
    make install || exit 1

# end of ampqcpp download

cd $download_dir

# begin of catch2 download

git clone https://github.com/catchorg/Catch2.git && \
    cd Catch2 && \
    mkdir build && \
    cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -- -j $(nproc) && \
    make install || exit 1

# end of catch2 download

cd $download_dir

apt-get install -y \
    libsnappy-dev \
    libpqxx-dev \
    docker \
    docker-compose \
    libmongocrypt-dev \
    python3-pip || exit 1

pip install \
    grpcio-tools \
    pytest || exit 1

# begin of curlpp download

apt-get install libcurl4-openssl-dev

git clone https://github.com/jpbarrette/curlpp.git && \
    cd curlpp && \
    mkdir build && \
    cd build && \
    cmake .. && \
    cmake --build . -- -j $(nproc) && \
    make install || exit 1

# end of curlpp download

cd $download_dir

# begin of glog download

git clone -b v0.6.0 https://github.com/google/glog.git && \
    cd glog && \
    cmake -S . -B build -G "Unix Makefiles" && \
    cmake --build build && \
    cmake --build build --target install || exit 1

# end of glog download

cd $download_dir

# begin of protobuf download

git clone -b v3.17.3 --recurse-submodules https://github.com/protocolbuffers/protobuf.git && \
   cd protobuf && \
   mkdir build && \
   cd build && \
   cmake ../cmake && \
   make -j $(nproc) && \
   make install || exit 1

# end of protobuf downloadя

cd $download_dir
cd ..

rm -r $download_dir
