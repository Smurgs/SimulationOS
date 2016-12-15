// SYSC 4001 - Final Project
// December 2016
// Stefan Murga
// Nwakpadolu Soluzochukwu John

#include <stdlib.h>
#include "queues.h"
#include "Scheduler.hpp"
#include "PriorityQueue.hpp"

PriorityQueue::PriorityQueue(void){
    queue = new(node);
    queue->data.PID = 0;
    queue->next = NULL;
}

void PriorityQueue::schedule(PCB newPCB) {
    // If empty, queue same way as regular queue
    if (queue->next == NULL){
        node *temp = queue;
        
        // Navigate to last node in linked list
        while (temp->next != NULL){
            temp = temp->next;
        }
        
        // Appened
        temp->next = new(node);
        
        // Populate new node
        temp->next->next = NULL;
        temp->next->data = newPCB;
        return;
    }
    
    // Create new node
    node *new_node = new(node);
    new_node->data = newPCB;
    
    // Insert new node based on priority
    node *temp = queue;
    while(temp->next != NULL){
        if (newPCB.priority < temp->next->data.priority){
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

bool PriorityQueue::getNext(PCB &nextPCB) {
    // Get PCB structure from head node
    PCB temp_PCB = queue->data;
    
    // Check for empty queue
    if (queue->next != NULL){
        // Get PCB struct from frist non-head node
        temp_PCB = queue->next->data;
        
        // Free first non-head node
        node *new_front = queue->next->next;
        free(queue->next);
        queue->next = new_front;
    }
    
    nextPCB = temp_PCB;
    
    if (nextPCB.PID == 0) {
        return false;
    }
    return true;
}
