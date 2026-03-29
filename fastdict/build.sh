#!/bin/bash

set -e

python3 -m venv venv
venv/bin/pip install pybind11

mkdir -p build
cd build

cmake .. -DPython3_EXECUTABLE=../venv/bin/python
make -j

cp fast_dict*.so ..
