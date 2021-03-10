FROM debian:unstable

RUN apt-get update -y && apt-get install -y make cmake g++ build-essential libncurses-dev python3-pip m4 bison flex && \
		pip3 install conan && \
		conan profile new default --detect && \
		conan profile update settings.compiler.libcxx=libstdc++11 default

COPY . /work/

RUN /bin/bash -c "cd /work && mkdir build_docker && cd build_docker && conan install .. --build missing && cmake -DUSE_DOCKER_BUILD=ON .. && make && cp -fr bin/* /usr/bin/."


