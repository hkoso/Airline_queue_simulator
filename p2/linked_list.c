#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"



/*
 * Function:  add_newNode
 * --------------------
 *  add and sort a node at the end of the linked list
 *
 *  input: 
 *  head: head of the node
 * 	id: new id
 * 	cabin: customer's class
 *  arrive_time: arriving time
 *  service_time: service time
 * 
 *
 *  returns: head of the node
 */
Node * add_newNode(Node* head, int id, int class, int arrive_time, int service_time){
    Node *node = malloc(sizeof(struct Node));
    node->id = id;
	node->class = class;
    node->arrive_time = arrive_time;
    node->service_time = service_time;
    node->next = NULL;

    if(head == NULL) {
        return node;
    }
	else if(head->arrive_time > node->arrive_time) {
		node->next = head;
		return node;
	}
	else {
		Node *cur = head;
		while (cur->next != NULL && cur->next->arrive_time <= node->arrive_time) {
			cur = cur->next;
		}

        if(cur->next == NULL){
		    cur->next = node;
        }
        else {
            Node *temp = cur->next;
            cur->next = node;
            node->next = temp;
        }

        return head;
	}
}

/*
 * Function:  add_back
 * --------------------
 *  add a node at the end of the linked list
 *
 *  input: 
 *  head: head of the node
 * 	id: new id
 * 	cabin: customer's class
 *  arrive_time: arriving time
 *  service_time: service time
 * 
 *
 *  returns: head of the node
 */
Node * add_back(Node* head, int id, int class, int arrive_time, int service_time){
    Node *node = malloc(sizeof(struct Node));
    node->id = id;
	node->class = class;
    node->arrive_time = arrive_time;
    node->service_time = service_time;
    node->next = NULL;

    if(head == NULL) {
        return node;
    }
	else {
		Node *cur = head;
		while (cur->next != NULL) {
			cur = cur->next;
		}

		cur->next = node;

        return head;
	}
}


/*
 * Function:  peekNode
 * --------------------
 *  return the head in the list
 *
 *  input: 
 *  head: head of the list
 * 
 *
 *  returns: head of the node
 */
Node * peekNode(Node* head){
    return head;
}


/*
 * Function:  peekNode
 * --------------------
 *  return the head in the list
 *
 *  input: 
 *  head: head of the list
 * 
 *
 *  returns: new head of the node
 */
Node * removeFront(Node* head){
    Node *result = head->next;
    head->next = NULL;
    return result;
}


Node * search(Node* head, int class, int arrival_time){
	Node* cur = head;
	while(cur->next != NULL && cur->next->class != class && cur->next->arrive_time != arrival_time){
		cur = cur->next;
	}

	return cur;
}



/*
 * Function:  printList
 * --------------------
 *  print the id, arrive time, service time in every node of the list
 *
 *  input: 
 *  head: head of the list
 *
 *  returns: number of nodes in the list
 */
int printList(Node *head){
	int num = 0;
	Node *cur = head;
	while(cur != NULL) {
		printf("%d ", cur->id);
		printf("%d ", cur->arrive_time);
		printf("%d\n", cur->service_time);
		cur = cur->next;
		num++;
	}

	return num;
}



/*
 * Function:  get_size
 * --------------------
 *  Get number of nodes in the list
 *
 *  input: 
 *  head: head of the list
 * 
 *
 *  returns: number of nodes in the list
 */
int get_size(Node *head){
    int num = 0;
	Node *cur = head;
	while(cur != NULL) {
		cur = cur->next;
		num++;
	}

	return num;
}
