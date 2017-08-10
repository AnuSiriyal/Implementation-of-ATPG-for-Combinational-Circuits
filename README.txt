Authors: Swetha Babu, Anu Siriyal,Jinung Jeong

GROUP #6

Compiling the program:
gcc -o <objectname> Group-6-FaultSimulator-ParallelFaultSimulation-FaultSimulator-DeductiveFaultSimulation.c

Please follow the below steps to execute the program:

Step 1: View the options provided by the project using the command HELP

Step 2: At any point of the execution, QUIT will exit from execution

Step 3: For reading a circuit using the command: READ <circuitname>

Step 4: For performing levelization use the command: LEV(done after READ always)

Step 5: To view the circuit at any point use the command: PC (after LEV always)

Step 6: To perform logic simulation use the command: LS (always after LEV) followed by the test pattern to be entered when asked to

Step 7: For creating collapsed fault list: FL (used after LS)

Step 8: For performing Parallel Fault Simulation use the command: PFS (only after FL) followed by the test pattern to be entered when asked to
NOTE: It works only for first set of 31 faults

Step 9: For performing Deductive Fault Simulation use the command: DFS (only after FL) followed by the test pattern to be entered when asked to
NOTE: It works only for circuits with 2 input logic gates at the maximum
 Jinung Jeong submitted his code at 11:59PM 