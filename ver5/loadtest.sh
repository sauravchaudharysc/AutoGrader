#!/bin/bash


SERVER_IP=$(nslookup "$1" | awk '/^Address: / { print $2; exit }')
if [ -n "$SERVER_IP" ]; then
    echo "Starting simulation with $1"
else
    echo "Unable to resolve the IP address of $1"
    exit 1
fi

SERVER_PORT=$2

SOURCE_FILE="./sampleFilesForTesting/program_runs.cpp"

mkdir -p simulation_results

CONCURRENT_CLIENTS=$3


for ((i=1; i<=$CONCURRENT_CLIENTS; i++)); do
    (
        ./submit "$SERVER_IP" "$SERVER_PORT" "$SOURCE_FILE" > "simulation_results/$i.txt"
    ) &
done

for job in `jobs -p`
do
    wait $job
done

echo Done
