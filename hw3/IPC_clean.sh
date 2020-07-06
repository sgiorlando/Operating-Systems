#!/bin/bash

ME=`whoami`

IPCS_S=`ipcs -s | egrep "[0-9]+" | grep $ME | cut -f2 -d" "`
echo semid\(s\): $IPCS_S

IPCS_M=`ipcs -m | egrep "[0-9]+" | grep $ME | cut -f2 -d" "`
echo shmid\(s\): $IPCS_M

for id in $IPCS_M
do
ipcrm -m $id;
done

for id in $IPCS_S
do
ipcrm -s $id;
done
