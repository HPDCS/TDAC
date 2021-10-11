# TDAC

This repository contains:

1) The source code of the solver implementation, written in Matlab, of the analytical model presented in the article. The script TDAC_scenario_1_public.m is suitable for cases with a single data set, and it allows to reproduce the results in the article for Scenario 1. The script TDAC_scenario_2_public.m is suitable for cases with multiple data sets, and it allows to reproduce the results in the article for Scenario 2. The results for all cases can be achieved by setting the configuration parameters at the beginning of each script. The source code of the solver implementation has been succesfully compiled with Matlab R2021a. 

2) The source code of the simulator, written in C, which has been used in the article for evaluating the model accuracy. It is suitable for simulating cases with both single data set and mutiple data sets. It allows to reproduce the results in the article for both Scenario 1 and Scenario 2. The results of all cases can be achieved by setting the configuration parameters in the file input_parameters.h. The bash script build_and_run.sh compiles and run the simulation. The source code has been succesfully compiled with gcc version 9.3.0
