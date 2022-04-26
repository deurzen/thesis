FROM python:3.9.9

# the federated model microservice will be housed in /fl_app
WORKDIR /fl_app

# copy over the CIFAR-10 train/test data
COPY ./data ./data

# copy over the federated model microservice
COPY ./data-handler.py \
    ./model.py \
    ./server.py \
    ./requirements.txt \
    ./

# install federated model microservice dependencies
RUN pip3 install --no-cache-dir -r ./requirements.txt

# add the microservices to the PATH
ENV PATH /fl_app:$PATH
EXPOSE 38866
ENTRYPOINT ["server.py"]
