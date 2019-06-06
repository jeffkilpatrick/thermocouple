# docker build -t thermocouple-build . && docker run thermocouple-build
FROM ubuntu:18.04
MAINTAINER Jeff Kilpatrick <jeff@bouncingsheep.org>

# install system-wide deps
RUN apt-get -yqq update
RUN apt-get -yqq install g++ cmake libncurses5-dev

# copy our sources
ADD cmake    /opt/thermocouple/cmake
ADD src      /opt/thermocouple/src
ADD test     /opt/thermocouple/test
ADD vendor   /opt/thermocouple/vendor
ADD build.sh /opt/thermocouple/
WORKDIR /opt/thermocouple

# build and test the app
CMD [ "./build.sh", "test", "release" ]