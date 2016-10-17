#!/bin/bash

# Create Swarm cluster and launch Qserv integration tests

set -e

DIR=$(cd "$(dirname "$0")"; pwd -P)
echo "Setup Swarm cluster and launch Qserv integration tests"

"$DIR"/swarm-destroy.sh
"$DIR"/swarm-create.sh
"$DIR"/run.sh
"$DIR"/run-multinode-tests.sh
