#!/bin/bash

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

echo "-------------- Building --------------"
mkdir --parents ${DIR}/build/linux
cd ${DIR}/build/linux
cmake -DCMAKE_BUILD_TYPE=Debug ../..
make -j

echo "---------- Starting program ----------"
gdb ./program

echo "----------- Opening image ------------"
# Machine specific
cd ${DIR}/output
feh 2.png
