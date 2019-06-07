# docker build -t thermocouple-build . && docker run --rm thermocouple-build
FROM kilpatjr/thermocouple-build-base
MAINTAINER Jeff Kilpatrick <jeff@bouncingsheep.org>

# copy our sources
ADD cmake    /opt/thermocouple/cmake
ADD src      /opt/thermocouple/src
ADD test     /opt/thermocouple/test
ADD vendor   /opt/thermocouple/vendor
ADD build.sh /opt/thermocouple/
WORKDIR /opt/thermocouple

# build and test the app
CMD [ "./build.sh", "test", "release" ]