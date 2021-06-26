# Geant4-TAU-SAT
## Compile
Create a new `build` folder: `mkdir build`\
Change dir to new folder: `cd build`\
Run: \
`cmake -DGeant4_DIR=/cvmfs/geant4.cern.ch/geant4/10.5/x86_64-slc6-gcc63-opt-MT/lib64/Geant4-10.5.0 ../build`\
in `build` folder, where `-DGeant4_DIR` is the path to Geant4 installation folder in current OS\
In the new `build` folder, run: \
`make`
## Run
Run `./Sim` for UI simulation run.\
Run `./Sim *input.txt*` to run batch.

To run using our script:\
`make_data.py` would create a batch of current run parameters.\
`run_data.py` would run the entire batch:
* `run_data.py --run` would run the batch and save Geant4 results to a file
* `run_data.py --parse` would gather all previous run data to a single `csv` file for data analysis.
