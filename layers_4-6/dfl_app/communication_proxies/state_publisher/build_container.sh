#!/usr/bin/env bash
set -Eeo pipefail

# https://community.rti.com/kb/creating-docker-image-rti-connext-dds
docker run -it --rm -v `pwd`:/app -w="/app" dds-build make amd64
docker run -it --rm -v `pwd`:/app -w="/app" dds-build make arm64

docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
docker buildx build --push --platform linux/amd64,linux/arm64/v8 -t deurzen/dfl-statepub:latest -f ./Dockerfile .
