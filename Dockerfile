FROM debian:unstable

RUN apt-get update -y && \
	apt-get install -y cmake build-essential g++ python3-pip libncurses-dev bison flex m4 && \
	pip3 install conan

WORKDIR /work
COPY . /work/

RUN cd /work && \
	mkdir build_docker && \
	cd build_docker && \
	cmake .. && \
	make && \
	cp ./bin/clfMonitor /usr/bin/. && \
	cp ./bin/clfGenerator /usr/bin/. && \
	cp ./bin/scenarioPlayer /usr/bin/. 
