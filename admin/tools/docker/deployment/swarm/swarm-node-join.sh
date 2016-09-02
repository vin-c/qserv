# Swarm node side
# Join swarm nodes
for i in $(seq 0 $INSTANCE_LAST_ID)
do
    HOSTNAME="$HOSTNAME_TPL$i"
    echo "Join swarm node on $HOSTNAME"
	ssh "$HOSTNAME" "$CMD"
done

# Wait for all swarm nodes to be in "Healthy" status
# Swarm 'info' interface is weak
PENDING="TRUE"
while [ -n "$PENDING" ]
do
    STATUS=$(docker -H tcp://$SWARM_HOSTNAME:$SWARM_PORT info | grep "Status: " || true)
    if [ -n "$STATUS" ]; then
        PENDING=$(echo "$STATUS" | grep "Pending" || true)
        echo "Waiting for all swarm node to reach 'Healthy' status"
        sleep 1
    fi
done

echo "Docker Swarm cluster ready"
