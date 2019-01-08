#!/bin/bash

dirpath=$(python -c "import os,sys; print os.path.realpath(sys.argv[1])" $1)
pushd $dirpath > /dev/null 
find . -type f  -not -path "*/build/*" \( -name \*.h -o -name \*.hpp -o -name \*.inl -o -name \*.mm -o -name \*.m -o -name \*.cpp \) -print | xargs clang-format -i
popd  > /dev/null 
