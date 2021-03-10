FROM archlinux:latest

RUN /bin/bash -c "pacman -Syu --noconfirm make cmake gcc python-pip m4 bison flex && \
		pip3 install conan && \
		conan profile new default --detect && \
		conan profile update settings.compiler.libcxx=libstdc++11 default"

COPY . /work/

RUN /bin/bash -c "cd /work && mkdir build_docker && cd build_docker && conan install .. --build missing && cmake -DUSE_DOCKER_BUILD=ON .. && make && cp -fr bin/* /usr/bin/."


