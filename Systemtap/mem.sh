#!/bin/bash

# como compilar
# chmod +x mem.sh
# sh mem.sh

#---mem
mem_usage () {
#Porcentagem de memoria livre

#Nome do processo
PROCESSO=htcg_omp 

# Intervalo em segundos da verificacao
INTERVALO=1

until ps -e | grep $PROCESSO > /dev/null
do
      sleep $INTERVALO
done


	date +"%T"

	free -m | grep 'Mem' | awk '{print "Livre:" $4/$2*100}'

	#memoria total
	free -m | grep 'Mem' | awk '{print "Total:" $2}'

	echo -e "\n"
	}

	while : ; do
	mem_usage
	sleep 1
	done


