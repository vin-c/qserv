#!/bin/bash

# Test Docker network is working,
# indeed it might have erratic behavior

set -e
set -x

DIR=$(cd "$(dirname "$0")"; pwd -P)

. "$DIR/env-infrastructure.sh"

SSH_CFG="$DIR/ssh_config"

for qserv_node in $MASTER $WORKERS 
do
	ssh -F "$SSH_CFG" "$qserv_node" "CONTAINER_ID=$(docker ps -l -q) && \
		docker exec -it \${CONTAINER_ID} ping master"
	for i in $(seq 0 $WORKER_LAST_ID)
	do
	    ssh -F "$SSH_CFG" "$qserv_node" "CONTAINER_ID=$(docker ps -l -q) && \
			docker exec -it \${CONTAINER_ID} ping worker-$i"
	done
done
