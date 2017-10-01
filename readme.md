# SimulationOS

----
## ABSTRACT

This report discusses the structure of the operating system simulation, and its results, which was developed as a final project for SYSC 4001 at Carleton University. The software was written in C++ and uses boost libraries. The simulation is modelled in accordance with PDEVS formalism. The project was carried out in a team of two.

The developed software simulates essential components of an operating system. In the simulation there are models for the kernel, memory manager, main memory, disk storage, system call interface, read/write functions, and programs. The models are event based and respond to external messages from other models. The kernel can be directed to use the first-come first-serve (FCFS) or priority queue algorithm to schedule the simulated processes. New processes are allocated an address space on the disk storage. The system makes use of virtual memory and swapping to allow many processes to operate simultaneously with a relatively low main memory size. Processes in the simulation are able to make read, write, exit, and fork system calls. The system call interface will then call upon the proper components to complete the requests. Together, all these components simulate the workings of a basic operating system.

![alt text](https://github.com/smurgs/SimulationOS/OS_Overview.jpg "Simulation overview")

###Full report included in repo
