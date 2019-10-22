# COSC418

## To build in src
cd ./omnetProject/src
opp_makemake -f --deep -r
make

## To run in src
`./omnetProject -n ./ned -f ./ned/omnetpp.ini`

## To run in simulations

## Generating .ini files
cd ./omnetProject/python/generators

Edit the generator files as required.

python3 expX_generator.py
