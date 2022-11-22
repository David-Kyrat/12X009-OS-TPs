#!/bin/zsh
git checkout c1c10ebc39fb9ed59bde7435b1a96fe89ba4b3e7
cd ..
cp TP05 -r ../ 
git checkout master
cd TP05 
cp ../../TP05/* . -rf
git commit -a -m "saving rendu-version of greg before doing anything"
git push 
