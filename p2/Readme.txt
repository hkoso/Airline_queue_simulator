How to compile and run your code:

1. Open the cmd or bash shell, and go to the directory that has the source code.
2. Find the p2.tar.gz file and use "tar -zxvf p2.tar.gz" to extract the p2 directory
3. Use cd go to the p2 directory and type make to compile the code.
4. type ./ACS to run the program.
5. Input the filename when the "ACS " prompt appears


Other:
1. Due to the fact that time measurement is based on relative machine time, +-0.01 second variation would occur from call to call.
Therefore, to minimize calculation error digits are truncate to 0.1s.
2. According to the Assignment2 QA, if customer file has arrive time or service time <= 0, error info will be displayed.
3. If the number of customer in the file does not match, error info will be displayed.
4. Extremely large imput file might cause stack smashing,
as the array processing the file data cannot have enough space to hold all input from file.
if such a thing happens, try to increase the initializing size for the customer[4000] array in the beginning of the process_file() function.


Reference:
https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm
https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm
linked_list file from Assignment1
tutorial4
