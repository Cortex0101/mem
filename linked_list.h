//
// Created by ldeir on 04-10-2022.
//

#ifndef MEM_LINKED_LIST_H
#define MEM_LINKED_LIST_H

#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

// Node of a doubly linked list.
struct Node {
    int size;
    char alloc;
    void* ptr;
    struct Node * next; // Pointer to next node in DLL.
    struct Node * prev; // Pointer to previous node in DLL.
};

struct LinkedList {
    struct Node * head;
    struct Node * tail;
};

typedef struct Node Node;
typedef struct LinkedList LinkedList;

LinkedList* linkedList;

unsigned is_empty(void); // Check if a linked list is empty or no.
void insert_first(int, char, void* ptr); // Add a node in the beginning.
void insert_last(int size, char alloc, void* ptr); // Add a node in the end.
void insert_after(Node *, int size, char alloc, void* ptr); // Add a node after a given node.
void insert_before(Node *, int size, char alloc, void* ptr); // Add a node before a given node.
Node * find_node(int size, char alloc, void* ptr); // Check if a given key exist or no.
void show(Node *); // Higher order function to be used in traverse: show node's key.
void traverse_forward(Node *, void (*callback)(Node *)); // Forward Traverse the linked list starting from a given Node.
void traverse_backward(Node *, void (*callback)(Node *)); // Backward Traverse the linked list starting from a given Node.
void remove_succesor(Node *); // Remove the successor of a Node.
void remove_node_with_ptr(void* ptr); // Remove Node from the list.
void remove_head(); // Remove a node from the beginning.
void remove_tail(); // Remove a node from the end.

unsigned is_empty(void) {
    return (linkedList -> head == NULL) && (linkedList -> tail == NULL);
}

void insert_first(int size, char alloc, void* ptr) {
    // Dynamic allocate node.
    Node * nd;
    nd = malloc(sizeof(Node));
    // put in the data.
    nd -> size = size;
    nd -> alloc = alloc;
    nd -> ptr = ptr;
    // Make next of new node as head.
    nd -> next = linkedList -> head;
    // Make previous of new node as NULL.
    nd -> prev = NULL;
    // If list is empty make the tail point to the new node.
    if (is_empty())
        linkedList -> tail = nd;
    else
        // change prev of head node to new node.
        linkedList -> head -> prev = nd;
    // move the head to point to the new node.
    linkedList -> head = nd;
}

void insert_last(int size, char alloc, void* ptr) {
    // Dynamic allocate node.
    Node * nd;
    nd = malloc(sizeof(Node));
    // put in the data.
    nd -> size = size;
    nd -> alloc = alloc;
    nd -> ptr = ptr;
    // This new node is going to be the last node, so next of it is NULL.
    nd -> next = NULL;
    // If the Linked List is empty, then the new node is head and tail.
    if (is_empty()) {
        nd -> prev = NULL;
        linkedList -> head = nd;
        linkedList -> tail = nd;
    } else {
        // Make previous of new node as tail.
        nd -> prev = linkedList -> tail;
        // change next of tail node to new node.
        linkedList -> tail -> next = nd;
        // move the prev to point to the new node.
        linkedList -> tail = nd;
    }


}

void insert_after(Node * nd, int size, char alloc, void* ptr) {
    // Dynamic allocate node.
    Node * temp_nd;
    temp_nd = malloc(sizeof(Node));
    // put in the data.
    temp_nd -> size = size;
    temp_nd -> alloc = alloc;
    temp_nd -> ptr = ptr;
    // Make next of new node as next of nd.
    temp_nd -> next = nd -> next;
    // Change previous of new nd's next node to the new node.
    if (temp_nd -> next != NULL)
        nd -> next -> prev = temp_nd;

    // Make the next of nd as new node.
    nd -> next = temp_nd;
    // Make nd as previous of new node.
    temp_nd -> prev = nd;
    if (temp_nd -> next == NULL)
        // If the next of new node is NULL, it will be  the new tail node.
        linkedList -> tail = temp_nd;
}

void insert_before(Node * nd, int size, char alloc, void* ptr) {
    // Dynamic allocate node.
    Node * temp_nd;
    temp_nd = malloc(sizeof(Node));
    // put in the data.
    temp_nd -> size = size;
    temp_nd -> alloc = alloc;
    temp_nd -> ptr = ptr;
    // Make prev of new node as prev of nd.
    temp_nd -> prev = nd -> prev;
    // Change next of nd's previous node to the new node.
    if (temp_nd -> prev != NULL)
        nd -> prev -> next = temp_nd;
    // Make the prev of nd as new node.
    nd -> prev = temp_nd;
    // Make nd as next of new node.
    temp_nd -> next = nd;

    if (temp_nd -> prev == NULL)
        // If the previous of new node is NULL, it will be  the new head node.
        linkedList -> head = temp_nd;
}

Node * find_first_node_with_capacity(size_t capacity) {
    Node* node = linkedList->head;
    while (node && (node -> size < capacity || node -> alloc == '1') ) {
        node = node -> next;
    }
    return node;
}

void traverse_forward(Node * nd, void(*callback)(Node *)) {
    while (nd) {
        (*callback)(nd);
        nd = nd -> next;
    }
}

void traverse_backward(Node * nd, void(*callback)(Node *)) {
    while (nd) {
        (*callback)(nd);
        nd = nd -> prev;
    }
}

void show(Node * nd) {
    printf("%d -- %c -- %d", nd -> size, nd -> alloc, nd->ptr);
}

void remove_succesor(Node * nd) {
    Node * temp_nd;
    temp_nd = nd -> next;
    nd -> next = temp_nd -> next;
    temp_nd -> next -> prev = nd;
    free(temp_nd);
}

void remove_node(Node * nd) {
    bool addedSizeToLeft = false;
    bool addedSizeToTheRight = false;
    if (nd -> prev != NULL) {
        if (nd->prev->alloc == '0') {
            nd->prev->size += nd->size;
            nd -> prev -> next = nd -> next;
            if (nd->next != NULL) {
                nd->next->prev = nd->prev;
            }
            addedSizeToLeft = true;
        }
    } else {
        nd->next->prev = NULL;
    }
    if (nd -> next != NULL) {
        if (!addedSizeToLeft && nd->next->alloc == '0') {
            nd->next->size += nd->size;
            nd->next->ptr -= nd->size;
            nd -> next -> prev = nd -> prev;
            if (nd->prev != NULL) {
                nd->prev->next = nd->next;
            }
            addedSizeToTheRight = true;
        }
    } else {
        nd->prev->next = NULL;
    }

    if (addedSizeToLeft | addedSizeToTheRight) {
        nd->next = NULL;
        nd->prev = NULL;
        nd->alloc = NULL;
        nd->size = NULL;
        nd->ptr = NULL;
        free(nd);
    }
    else {
        nd->alloc = '0';
    }
}

void remove_node_with_ptr(void* ptr) {
    Node* node = linkedList->head;
    while (node && (node->ptr != ptr)) {
        node = node -> next;
    }
    remove_node(node);
}

Node* find_adjecent_unallocated_nodes() {
    Node* last;
    Node* node = linkedList->head;
    while (node != NULL) {
        last = node;
        node = node->next;

        if (node != NULL && last != NULL) {
            if (node->alloc == '0' && last->alloc == '0') {
                return last;
            }
        }
    }
    return NULL;
}

void remove_head() {
    Node * nd;
    nd = linkedList -> head;
    linkedList -> head = nd -> next;
    free(nd);
    if (linkedList -> head == NULL)
        linkedList -> tail = NULL;
    else
        linkedList -> head -> prev = NULL;
}

void remove_tail() {
    Node * nd;
    if (linkedList -> head == linkedList -> tail)
        remove_head(linkedList);
    else {
        nd = linkedList -> head;
        while(nd ->next != linkedList -> tail)
            nd = nd -> next;
        nd -> next = NULL;
        free(linkedList -> tail);
        linkedList -> tail = nd;
    }
}

#endif //MEM_LINKED_LIST_H