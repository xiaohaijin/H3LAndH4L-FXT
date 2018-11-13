#!/bin/bash

workdir="/star/u/pengliu/gpfs01/Lambda/R10AuAu200GeV"
rootfiledir="${workdir}/output0"
cshfiledir="${workdir}/input/part0/runinfo/csh"
sessfiledir="${workdir}/input/part0"

id="5F8D5562C2A252FA09C33F791E55CC00"

n=0
i=0
failedID=""

while [ $i -lt 10492 ]; do
	
	if [ ! -e ${rootfiledir}/${id}_$i.xi.picodst.root ]; then
		echo "${id}_$i.xi.picodst.root doesn't exist.............."

		failedID="${failedID}${i},"

		let "n+=1"
	fi
    
	let "i+=1"
done

echo "failed job ID:"
echo "${failedID%?}"

if [ $n -gt 0 ]; then
	
	star-submit -r  ${failedID%?}  ${sessfiledir}/${id}.session.xml

fi

echo "Total number of files missing = $n"
