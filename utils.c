#include "utils.h"
#include<stdio.h>
#include<stdlib.h>

void destroy_list(node* head) {
    if(head == NULL) {
        return;
    }
    destroy_list(head->next);
    free(head);
}

void insert_at_last(int value, node* head)
{
    node *temp_node;
    temp_node = (node *) malloc(sizeof(node));

    temp_node->number=value;
    temp_node->next=NULL;

    //For the 1st element
    if(head==NULL)  {
        head=temp_node;
    }
    else {
        while(head->next) {
            head = head->next;
        }
        head->next = temp_node;
    }

}


void insert_at_first(int value, node* head)
{
    node *temp_node = (node *) malloc(sizeof(node));

    temp_node->number=value;
    temp_node->next = head;

    head = temp_node;
}

void insert_after(int key, int value, node* head)
{
    node *myNode = head;
    int flag = 0;

    while(myNode!=NULL) {
        if(myNode->number==key) {
            node *newNode = (node *) malloc(sizeof(node));
            newNode->number = value;
            newNode->next = myNode->next;
            myNode->next = newNode;

            printf("%d is inserted after %d\n", value, key);

            flag = 1;


            break;
        }
        else
            myNode = myNode->next;
    }

    if(flag==0)
        printf("Key not found!\n");

}


void delete_item(int value, node* head)
{
    node *myNode = head, *previous=NULL;
    int flag = 0;

    while(myNode!=NULL) {
        if(myNode->number==value) {
            if(previous==NULL)
                head = myNode->next;
            else
                previous->next = myNode->next;

            printf("%d is deleted from list\n", value);

            flag = 1;
            free(myNode); //need to free up the memory to prevent memory leak
            break;
        }

        previous = myNode;
        myNode = myNode->next;
    }

    if(flag==0)
        printf("Key not found!\n");
}


void search_item(int value, node* head)
{
    node *searchNode = head;
    int flag = 0;

    while(searchNode!=NULL) {
        if(searchNode->number==value) {
            printf("%d is present in this list. Memory address is %d\n", value, searchNode);
            flag = 1;
            break;
        }
        else
            searchNode = searchNode->next;
    }

    if(flag==0)
        printf("Item not found\n");

}


void print_list(node* head)
{
    printf("\nYour full linked list is\n");

    node *myList;
    myList = head;

    while(myList!=NULL) {
        printf("%d ", myList->number);

        myList = myList->next;
    }
    puts("");
}
