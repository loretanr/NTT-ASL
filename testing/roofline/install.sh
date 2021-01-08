#!/usr/bin/env bash
# install if gfortran is not available
# sudo apt-get install gfortran
pushd www.cs.virginia.edu/stream/FTP/
make
./stream_c.exe
popd