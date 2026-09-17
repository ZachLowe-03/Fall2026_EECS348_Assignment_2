/*
EECS 348 Assignment 2
Description: C program that implements a priority queue for emails using a max heap.
Outputs: Display of email information and queue status
Collaborators: None
Other sources for the code: Copilot
Author: Copilot (prompted by Zach Lowe)
Creation Date: 09/17/2026
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMAILS 1000
#define MAX_SUBJECT 256

typedef struct {
    char sender[30];
    char subject[MAX_SUBJECT];

    int month;
    int day;
    int year;
} Email;

typedef struct {
    Email data[MAX_EMAILS];
    int size;
} MaxHeap;


/* -------------------- Priority Functions -------------------- */

int senderPriority(char *sender)
{
    if(strcmp(sender, "Boss") == 0)
        return 5;
    if(strcmp(sender, "Subordinate") == 0)
        return 4;
    if(strcmp(sender, "Peer") == 0)
        return 3;
    if(strcmp(sender, "ImportantPerson") == 0)
        return 2;

    return 1; /* OtherPerson */
}

/* return 1 if a has higher priority than b */
int higherPriority(Email a, Email b)
{
    int pa = senderPriority(a.sender);
    int pb = senderPriority(b.sender);

    if(pa > pb)
        return 1;
    if(pa < pb)
        return 0;

    /* Same sender category -> newer date wins */

    if(a.year != b.year)
        return a.year > b.year;

    if(a.month != b.month)
        return a.month > b.month;

    return a.day > b.day;
}


/* -------------------- Heap Operations -------------------- */

void swap(Email *a, Email *b)
{
    Email temp = *a;
    *a = *b;
    *b = temp;
}

void heapifyUp(MaxHeap *heap, int index)
{
    while(index > 0)
    {
        int parent = (index - 1) / 2;

        if(higherPriority(heap->data[index], heap->data[parent]))
        {
            swap(&heap->data[index], &heap->data[parent]);
            index = parent;
        }
        else
        {
            break;
        }
    }
}

void heapifyDown(MaxHeap *heap, int index)
{
    while(1)
    {
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        if(left < heap->size &&
           higherPriority(heap->data[left], heap->data[largest]))
        {
            largest = left;
        }

        if(right < heap->size &&
           higherPriority(heap->data[right], heap->data[largest]))
        {
            largest = right;
        }

        if(largest != index)
        {
            swap(&heap->data[index], &heap->data[largest]);
            index = largest;
        }
        else
        {
            break;
        }
    }
}

void insertEmail(MaxHeap *heap, Email email)
{
    if(heap->size >= MAX_EMAILS)
        return;

    heap->data[heap->size] = email;
    heapifyUp(heap, heap->size);
    heap->size++;
}

Email removeMax(MaxHeap *heap)
{
    Email empty;

    strcpy(empty.sender, "");
    strcpy(empty.subject, "");
    empty.month = empty.day = empty.year = 0;

    if(heap->size == 0)
        return empty;

    Email maxEmail = heap->data[0];

    heap->size--;

    if(heap->size > 0)
    {
        heap->data[0] = heap->data[heap->size];
        heapifyDown(heap, 0);
    }

    return maxEmail;
}

Email peekMax(MaxHeap *heap)
{
    Email empty;

    strcpy(empty.sender, "");
    strcpy(empty.subject, "");
    empty.month = empty.day = empty.year = 0;

    if(heap->size == 0)
        return empty;

    return heap->data[0];
}


/* -------------------- Display -------------------- */

void printEmail(Email e)
{
    if(strlen(e.sender) == 0)
    {
        printf("There are no emails to read.\n");
        return;
    }

    printf("Next email:\n");
    printf("\tSender: %s\n", e.sender);
    printf("\tSubject: %s\n", e.subject);
    printf("\tDate: %02d-%02d-%04d\n",
           e.month,
           e.day,
           e.year);
}


/* -------------------- Parsing -------------------- */

void processEmailLine(MaxHeap *heap, char *line)
{
    Email e;

    char *token;

    line[strcspn(line, "\n")] = '\0';

    token = strtok(line + 6, ",");
    strcpy(e.sender, token);

    token = strtok(NULL, ",");
    strcpy(e.subject, token);

    token = strtok(NULL, ",");

    sscanf(token,
           "%d-%d-%d",
           &e.month,
           &e.day,
           &e.year);

    insertEmail(heap, e);
}


/* -------------------- Main -------------------- */

int main()
{
    MaxHeap heap;
    heap.size = 0;

    char line[512];

    while(fgets(line, sizeof(line), stdin))
    {
        line[strcspn(line, "\n")] = '\0';

        if(strncmp(line, "EMAIL ", 6) == 0)
        {
            processEmailLine(&heap, line);
        }
        else if(strcmp(line, "NEXT") == 0)
        {
            printEmail(peekMax(&heap));
        }
        else if(strcmp(line, "READ") == 0)
        {
            removeMax(&heap);
        }
        else if(strcmp(line, "COUNT") == 0)
        {
            printf("There are %d emails to read.\n",
                   heap.size);
        }
    }

    return 0;
}
