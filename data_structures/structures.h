// SYSC 4001 - Assignemnt 3
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John 

#ifndef KERNEL_STRUCTURES
#define KERNEL_STRUCTURES


// PCB definition
typedef struct PCB {
    char PID;
    char state;
    char type;
    char priority;
} PCB;

// Node definition
typedef struct node {
    PCB data;
    struct node *next;
} node;

#endif
