#!/bin/bash

cpu_usage(){

#Nome do processo
PROCESSO=testes

# Intervalo em segundos da verificacao
INTERVALO=1

until ps -e | grep $PROCESSO > /dev/null
do
      sleep $INTERVALO
done

python3 monitoramentoCPU.py

}

	while : ; do
	cpu_usage
	sleep 1
	done

