 file -i * | cut -d ';' -f 1 | grep image | cut -d ':' -f 1 | tr '\n' ','
 #last call to be able to treat it like csv
