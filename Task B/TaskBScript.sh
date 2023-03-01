#!/bin/bash

if [[ -f "Outputs/CLREDDataRate.txt" ]];
then
	rm "Outputs/CLREDDataRate.txt"
fi

for rate in {100..500..10}
do
	./waf --run "scratch/CLRED --queueDiscType=CLRED --appDataRate="$rate"Mbps"
done
