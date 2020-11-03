#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo "-------------- Building --------------"
mkdir --parents ${DIR}/build/linux
cd ${DIR}/build/linux
cmake ../..
make -j

echo "---------- Starting program ----------"
./program

echo "----------- Opening image ------------"
# Machine specific
cd ${DIR}/output
feh 1.png 2.png 3.png 4.png
