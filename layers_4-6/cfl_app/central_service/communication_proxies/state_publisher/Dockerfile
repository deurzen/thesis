FROM ubuntu:18.04
ARG TARGETARCH

# the proxy microservices will be housed in /fl_app
WORKDIR /fl_app

# copy over the microservice, the QoS file, and the RTI license
COPY ./USER_QOS_PROFILES.xml \
    ./rti_license.dat \
    ./obj/$TARGETARCH/state_publisher \
    ./

ENV PATH /fl_app:$PATH
ENTRYPOINT ["state_publisher"]
