FROM python:3.9.9

# the monitoring microservice will be housed in /monitoring
WORKDIR /monitoring

# copy over the FL model microservice
COPY ./metrics.py \
    ./monitor.py \
    ./requirements.txt \
    ./

# install FL model microservice dependencies
RUN pip3 install --no-cache-dir -r ./requirements.txt

# add the microservices to the PATH
ENV PATH /monitoring:$PATH

ENTRYPOINT ["monitor.py"]
