#!/bin/bash

# Input data
data="3,0.957
4,0.230
5,0.255
6,1.137
7,1.298
8,1.258
9,2.164
10,2.084
13,2.702
15,2.565
18,3.608
20,7.895
21,7.689
22,27.450
23,27.650
24,35.840
27,37.891
30,53.858"

# Output file
output_file="throughput_results.csv"

# Calculate throughput and write the result to the output file
echo "Client,Throughput" > "$output_file"
echo "$data" | awk -F',' '{ throughput = $1 / $2; print $1 ", " throughput }' >> "$output_file"

echo "Results written to $output_file"

