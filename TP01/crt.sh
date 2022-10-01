#!/bin/bash
oldPath=$(pwd)
# Parse args
if [ $# -lt 2 ]; then 
    >&2 echo "Not enough arguments, expecting at least 2"
    #exit 2
fi

res=""
if [ $# -ge 3 ]; then 
    if [ $# != 3 ]; then echo "Too many arguments, only taking the first 3 in consideration"; fi
    res="$3";
fi
beg="$1"; dest="$2"

echo $'\n Starting (png) conversion of images from'" \"$1\" to \"$2\" ..." 
if [ "$3" ]; then echo $'\t (specified res: '"\"$3\")"; fi
echo ""

beg="images"
dest="lul"
# Create output dir if does not exists
mkdir -p -v "$dest"

function copyMoveRenameFile {
    name="$1" # quoting since name might contain spaces
    newName=$(echo "$name" | tr -d "['\"]" | tr ' ' '-')
    echo "copying $name to \"$dest/$newName\""
    cp "$name" "$dest/$newName"
    echo "$dest/$newName" #output is new path of the img
}

function cvrt {
    name=$1; nameNoExt="${name%%.*}"
    cmd="convert "
    if [ "$res" ]; then cmd="$cmd resize $res "; fi
    cmd="$cmd""$name $nameNoExt.png"
    echo $'command is\n\t'"\"$cmd\""

}

function is {
      [[ $1 ]] && echo "true" || echo "false"
}

function isImg {
    crtFile="$1"
    is '$(file "$crtFile" -i) =~ image'
}


function renameBeforeExecution {
    mkdir tmp; cd tmp
    cp ../* . -f 2> err
    for f in * ; do mv "$f" $(tr -d "[' ''\"]" <<< "$f") -f 2> err; done 
}


function copyAndConvertAll {
    renameBeforeExecution
    for f in * ; do 
        if [[ $(isImg "$f") ]]; then
            mv "$f" "$oldPath/$dest"
        fi
    done
    cd ..
    rm tmp -r
    cd "$oldPath/$dest"; echo " "; pwd; ls; echo " "

    echo "converting"
    if [ "$res" ]; then mogrify -format png -resize "$res" *
    else mogrify -format png * ; fi
    echo "done"
    cd $oldPath #go back to path where script was called
}

cd "$beg"

copyAndConvertAll 

cd "$oldPath"
