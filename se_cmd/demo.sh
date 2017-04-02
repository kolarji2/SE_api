#!/bin/sh
#./SE_api -i PeriodicRVE.geo -o foam.fe -c test.cmd
#evolver -f foam.cmd foam.fe

#./SE_api -g random -n 9 -p foam.gnu
#gnuplot> splot "foam.gnu" with lines


./SE_api -i PeriodicRVE.geo -o foam.fe -c test.cmd,distribution.cmd
evolver -f foam.cmd foam.fe
#./SE_api -g random -n 9 -p foam.gnu
#gnuplot> splot "foam.gnu" with lines
