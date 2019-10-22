# COSC418

## To build in src
`cd ./omnetProject/src
opp_makemake -f --deep -r
make`

## To run in src
`./omnetProject -n ./ned -f ./ned/omnetpp.ini`

## To run simulations
`cd ./omnetProject/python/runners
python3 expX_runner.py`

Alternatively,

`cd ./omnetProject/simulations
../src/omnetProject -n .:../src/ned -f X.ini -u Cmdenv`

(.ini file must be in the simulations directory)

## Generating .ini files
`cd ./omnetProject/python/generators`

Edit the generator files as required.

`python3 expX_generator.py`

## Authors
* **Dylan White*
* **Gus Ellerm*
