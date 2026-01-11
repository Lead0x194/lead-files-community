#!/bin/bash
printf  "%-32s  %8s %8s %8s %8s\n" Map X Y Width Height
echo "---------------------------------------------------------------------"
for map in $(cat index|sort|awk '{print $2}')
do
	while read line
	do
		data=(${line})
		case ${data[0]} in
			BasePosition ) 
			               x=$(bc <<< "${data[1]}/100")
			               y=$(bc <<< "${data[2]}/100")
			;;
			MapSize ) 
			               w=$(bc <<< "${data[1]}*256")
			               h=$(bc <<< "${data[2]}*256")
			;;
		esac
	done<<_EOD_
	$(egrep "(BasePosition|MapSize)" ${map}/Setting.txt)
_EOD_
	printf  "%-32s: %8d %8d %8d %8d\n" ${map} ${x} ${y} ${w} ${h}
	if [[ ${1} != "" && ${2} != "" ]]; then
		searchmap=1
		if [[ ${1} -ge ${x} && ${1} -le $(bc <<< "${x}+${w}") && ${2} -ge ${y} && ${2} -le $(bc <<< "${y}+${h}") ]]; then
			foundmap="${map}"
		fi
	fi
done

if [[ ${searchmap} == 1 ]]; then
	if [[ ${foundmap} != "" ]]; then
		echo -e "\n\n# Coordinates are in the map ${foundmap}\n"
	else
		echo -e "\n\n# Coordinates are not on any map\n"
	fi
fi
