This repository contains software packages related to the article: "On the Effects of Transaction Data Access Patterns on Performance in Lock-based Concurrency Control". It includes two folders: 

1) "model_solver" contains the source code of the solver implementation of the analytical model presented in the article. The solver is written in Matlab. The script TDAC_scenario_1_public.m targets the cases with a single data set. It allows to reproduce the results presented in the article for Scenario 1. The script TDAC_scenario_2_public.m targets the cases with multiple data sets. It allows to reproduce the results presented in the article for Scenario 2. To reproduce the results for a given case, please, set the configuration parameters at the beginning of the script. The source code of the solver implementation has been succesfully compiled with Matlab R2021a. 

2) "simulator" contains the source code of the simulator used in the article for evaluating the model accuracy. The somulator is written in C language. It can simulate both the cases with single data set and the cases with mutiple data sets, thus allowing to reproduce the results presented in the article for both Scenario 1 and Scenario 2. To reproduce the results for a given case, please, set the configuration parameters in the file input_parameters.h. The bash script build_and_run.sh compiles and run the simulation. The source code has been succesfully compiled with gcc version 9.3.0
