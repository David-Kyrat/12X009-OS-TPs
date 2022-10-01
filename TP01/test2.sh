#!/bin/bash
DATA=~/BA3



if [ $# -lt 2 ]; then 
    echo "Not enough arguments, expecting at least 2"
    exit "-1"
fi

if [ $# -gt 3 ]; then echo "Too many arguments, only taking the first 3 in consideration"; fi

beg=$2
dest=$3

if [ -d $DATA ]; then
    #Efface les anciens backups
    echo "Lul"
else
    #Crée le repetoire
    mkdir "Backup"
fi

echo "startDir $1"
echo "destDir $2"
echo "res $3"
