# docker build -t thermocouple-build . && docker run thermocouple-build
FROM ubuntu:18.04
MAINTAINER Jeff Kilpatrick <jeff@bouncingsheep.org>

# install system-wide deps
RUN apt-get -yqq update
RUN apt-get -yqq install cmake curl file g++ libncurses5-dev libusb-dev

WORKDIR /opt/phidgets21
RUN curl -O https://www.phidgets.com/downloads/phidget21/libraries/linux/libphidget/libphidget_2.1.9.20190409.tar.gz
RUN tar -xzf libphidget_2.1.9.20190409.tar.gz
WORKDIR /opt/phidgets21/libphidget-2.1.9.20190409
RUN ./configure
RUN make install

# copy our sources
ADD cmake    /opt/thermocouple/cmake
ADD src      /opt/thermocouple/src
ADD test     /opt/thermocouple/test
ADD vendor   /opt/thermocouple/vendor
ADD build.sh /opt/thermocouple/
WORKDIR /opt/thermocouple

# build and test the app
CMD [ "./build.sh", "test", "release" ]