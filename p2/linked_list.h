#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct Node Node;

struct Node{
    int id;
    int class;
    int arrive_time;
    int service_time;
    Node * next;
};


Node * add_newNode(Node* head, int id, int class, int arrive_time, int service_time);
Node * add_back(Node* head, int id, int class, int arrive_time, int service_time);
Node * peekNode(Node* head);
Node * removeFront(Node* head);
Node * search(Node* head, int class, int arrival_time);
int printList(struct Node *node);
int get_size(Node *node);



#endif