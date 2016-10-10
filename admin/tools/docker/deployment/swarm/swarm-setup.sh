#!/bin/sh

# Configure a swarm cluster for Qserv

# @author  Fabrice Jammes, IN2P3

set -e
set -x

DIR=$(cd "$(dirname "$0")"; pwd -P)

. "${DIR}/swarm-env.sh"

# Create a swarm on the openstack machine dedicated to swarm
HOST_IP=$(hostname --ip-address)
docker swarm init --advertise-addr "$HOST_IP"

