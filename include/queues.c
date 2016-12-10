// SYSC 4001 - Assignment 3
// December 16
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#include <stdlib.h>
#include "queues.h"

// Queue initializition
node* init_queue(){
    node *temp = (node*) malloc(sizeof(node));
    temp->data.PID = 0;
    temp->next = NULL;
    
    return temp;
}

// Dequeue
PCB dequeue(node *head){

    // Get PCB structure from head node
    PCB temp_PCB = head->data;

    // Check for empty queue
    if (head->next != NULL){
        // Get PCB struct from frist non-head node
        temp_PCB = head->next->data;

        // Free first non-head node
        node *new_front = head->next->next;
        free(head->next);
        head->next = new_front;
    }
    return temp_PCB;
}


// Queue
void queue(node *head, PCB process_data){
    node *temp = head;

    // Navigate to last node in linked list
    while (temp->next != NULL){
        temp = temp->next;
    }

    // Appened 
    temp->next = (node*) malloc(sizeof(node));

    // Populate new node 
    temp->next->next = NULL;
    temp->next->data = process_data;
}
