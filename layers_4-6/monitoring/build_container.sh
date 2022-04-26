#!/usr/bin/env bash
set -Eeo pipefail

docker run --rm --privileged multiarch/qemu-user-static --reset -p yes
docker buildx build --push --platform linux/amd64,linux/arm64/v8 -t deurzen/fedapp-monitor:latest -f ./Dockerfile .
