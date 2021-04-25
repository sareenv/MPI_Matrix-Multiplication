# MPI Matrix Multiplication

The project contains the code for matrix multiplication implemented using C programming language and MPI interface. 
MPI is message passing interface used for developing parallel application. Furthermore, the implementation code features the interprocess communication with the same 
communicator using the MPI blocking calls. The project can be developed further and improved by implementing more superior parllel processing algorithms such as Cannon's 
Algorithm.


## How to Run the Project

In order to run the mp.c file use the following command to compile the source file to the object file which can run parllel

```
mpi -o m mp.c
```
Furthermore the following command will run the code using three different processes sharing same communicator using the blocking mpi calls. 

```
mpirun -n 3  m
```
