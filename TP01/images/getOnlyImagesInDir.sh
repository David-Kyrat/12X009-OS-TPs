#!/bin/bash 
file -i * | cut -d ';' -f 1 | grep image | cut -d ':' -f 1 | sed -z 's/\n/,\n/g'
 #last call to be able to treat it like csv
 while IFS="," read -r im; do echo $(tr -d "[' ''\"]" <<< $im); done < ./test.csv
