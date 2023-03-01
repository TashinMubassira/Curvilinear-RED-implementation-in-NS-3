#!/bin/bash

# if [[ -f "Outputs/nWifiData.txt" ]];
# then
# 	rm "Outputs/nWifiData.txt"
# fi

# for nodes in {20..80..5}
# do
# 	./waf --run "scratch/wireless --nWifi="$nodes""
# done


# if [[ -f "Outputs/speedData.txt" ]];
# then
# 	rm "Outputs/speedData.txt"
# fi

# for speed in {5..25}
# do
# 	./waf --run "scratch/wireless --speed="$speed""
# done

if [[ -f "Outputs/WirelessDataRate.txt" ]];
then
	rm "Outputs/WirelessDataRate.txt"
fi


	./waf --run "scratch/wireless --interval=1"
	./waf --run "scratch/wireless --interval=1.1"
	./waf --run "scratch/wireless --interval=1.2"
	./waf --run "scratch/wireless --interval=1.3"
	./waf --run "scratch/wireless --interval=1.4"
	./waf --run "scratch/wireless --interval=1.5"
	./waf --run "scratch/wireless --interval=1.6"
	./waf --run "scratch/wireless --interval=1.7"
	./waf --run "scratch/wireless --interval=1.8"
	./waf --run "scratch/wireless --interval=1.9"
	./waf --run "scratch/wireless --interval=2"
	./waf --run "scratch/wireless --interval=2.1"
	./waf --run "scratch/wireless --interval=2.2"
	./waf --run "scratch/wireless --interval=2.3"
	./waf --run "scratch/wireless --interval=2.4"
	./waf --run "scratch/wireless --interval=2.5"




