# CARLsim-Noxim-Parser
Scripts to port CARLsim4 logs into configuration files for Noxim.


1. Run edit_carllog.pm to remove the paths in the carlsim.log file (creates carl.log).
2. Edit the log_parser.py with information regarding the index of the first neuron in every group in the design. 
3. Run log_parser.py to extract traffic, weight and connection information from the carl.log file. 
