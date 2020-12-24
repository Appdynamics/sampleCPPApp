FROM gcc:4.9
RUN apt-get update && apt-get install -y gdb vim libboost1.55-all-dev cmake wget curl libcurl4-openssl-dev
RUN wget http://downloads.cpp-netlib.org/0.11.2/cpp-netlib-0.11.2-final.tar.gz && echo "09118c61e8267fcbf9a213697f242535 cpp-netlib-0.11.2-final.tar.gz" > md5sum.txt && md5sum -c md5sum.txt && tar -zxf cpp-netlib-0.11.2-final.tar.gz && mkdir /cpp-netlib-build
WORKDIR /cpp-netlib-build
RUN cmake /cpp-netlib-0.11.2-final/. && make && make install
COPY . /usr/src/dockertest1
WORKDIR /usr/src/dockertest1
RUN tar xvf appdynamics-sdk-native-64bit-linux-20.9.0.36.0.tar -C /opt
# RUN tar xvf appdynamics-cpp-sdk-linux-x64-debug-0.0.0-0-new.tbz2 -C /opt
ENV LD_LIBRARY_PATH=/opt/appdynamics-cpp-sdk/lib/
ENV BOOST_ROOT=/cpp-netlib-0.11.2-final/boost
EXPOSE 8000
RUN g++ -std=c++11 -g -o main main.cpp -L /opt//appdynamics-cpp-sdk/lib -lappdynamics -lpthread -ldl -lrt -I$BOOST_ROOT -I. -L$BOOST_ROOT/stage/lib -lboost_system -lcurl
CMD ["./main", "0.0.0.0", "8000"]
