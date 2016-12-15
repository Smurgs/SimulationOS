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

// Enqueue - priority queue
void enqueue_priority(node *head, PCB process_data){
    
    // If empty, queue same way as regular queue
    if (head->next == NULL){
        enqueue_regular(head, process_data);
        return;
    }
    
    // Create new node
    node *new_node = (node*) malloc(sizeof(node));
    new_node->data = process_data;
    
    // Insert new node based on priority
    node *temp = head;
    while(temp->next != NULL){
        if (process_data.priority < temp->next->data.priority){
            new_node->next = temp->next;
            temp->next = new_node;
            return;
            
        }else if (temp->next->next == NULL){
            new_node->next = NULL;
            temp->next->next = new_node;
            return;
        }
        
        temp = temp->next;
    }
}
