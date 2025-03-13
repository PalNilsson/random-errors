# random-errors
Example of how to draw random error codes from a distribution with historical panda errors.
The randomization of the error codes (in mc_dict) was turned into a generator (in error_code_generator) which will be easier to use in e.g. SimGrid.

## How to build and run
* mkdir build
* cd build
* cmake ..
* make
* ./mc_dict --input \<input file\> --queue \<queue name\> --n \<number of errors\>
* ./error_code_generator

