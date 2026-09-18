/*
EECS 348 Assignment 2
Description: C program that implements a priority queue for emails using a max heap.
Outputs: Display of email information and queue status
Collaborators: I used ChatGPT for some of the commenting, improving the input handling and the parsing in my revision, as well as bug fixing
Other sources for the code: Copilot
Author: Copilot (prompted and revised by Zach Lowe)
Creation Date: 09/17/2026
Revision Date: 09/17/2026
Revisions: More consistent comments. Email now stores priority and date at insert time rather than recomputing
per comparison (faster). Safer input handling (bounded copies, presence checks, parse validation). Insertion now increments
size before heapifying. Sender-priority function takes a const pointer.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMAILS 1000
#define MAX_SUBJECT 256

// Store all information associated with one email.
typedef struct {
    char sender[30];
    char subject[MAX_SUBJECT];

    int month;
    int day;
    int year;
    int priorityLevel; // Store sender priority after the email is read.
    int dateValue;     // Store date as YYYYMMDD for easier comparison.
} Email;

// Store the emails in an array-based max heap.
typedef struct {
    Email data[MAX_EMAILS];
    int size;
} MaxHeap;


// -------------------- Priority Functions --------------------

// Convert the sender category into its required priority value.
int senderPriority(const char *sender)
{
    if(strcmp(sender, "Boss") == 0)
        return 5;

    if(strcmp(sender, "Subordinate") == 0)
        return 4;

    if(strcmp(sender, "Peer") == 0)
        return 3;

    if(strcmp(sender, "ImportantPerson") == 0)
        return 2;

    return 1; // Any other sender is treated as OtherPerson.
}

// Convert the date into YYYYMMDD so newer dates can be compared as integers.
int makeDateValue(int month, int day, int year)
{
    return year * 10000 + month * 100 + day;
}

// Return 1 if email a has higher priority than email b.
int higherPriority(Email a, Email b)
{
    // Compare the sender priorities that were calculated when each email was added.
    if(a.priorityLevel > b.priorityLevel)
        return 1;

    if(a.priorityLevel < b.priorityLevel)
        return 0;

    // If sender priority is tied, the newer date has higher priority.
    if(a.dateValue > b.dateValue)
        return 1;

    return 0;
}


// -------------------- Heap Operations --------------------

// Swap two emails in the heap.
void swap(Email *a, Email *b)
{
    Email temp = *a;
    *a = *b;
    *b = temp;
}

// Move a newly inserted email upward until the max-heap property is restored.
void heapifyUp(MaxHeap *heap, int index)
{
    while(index > 0)
    {
        // Calculate the index of the current email's parent.
        int parent = (index - 1) / 2;

        // If the child has higher priority, swap it with its parent.
        if(higherPriority(heap->data[index], heap->data[parent]))
        {
            swap(&heap->data[index], &heap->data[parent]);
            index = parent;
        }
        else
        {
            // Stop when the max-heap property is already satisfied.
            break;
        }
    }
}

// Move the root downward until the max-heap property is restored.
void heapifyDown(MaxHeap *heap, int index)
{
    while(1)
    {
        // Calculate the indices of the current email's children.
        int left = 2 * index + 1;
        int right = 2 * index + 2;
        int largest = index;

        // Check whether the left child has higher priority.
        if(left < heap->size &&
           higherPriority(heap->data[left], heap->data[largest]))
        {
            largest = left;
        }

        // Check whether the right child has higher priority.
        if(right < heap->size &&
           higherPriority(heap->data[right], heap->data[largest]))
        {
            largest = right;
        }

        // Swap with the higher-priority child if necessary.
        if(largest != index)
        {
            swap(&heap->data[index], &heap->data[largest]);
            index = largest;
        }
        else
        {
            // Stop when the max-heap property is restored.
            break;
        }
    }
}

// Add an email to the end of the heap and move it into the correct position.
void insertEmail(MaxHeap *heap, Email email)
{
    // Do not insert another email if the fixed heap is full.
    if(heap->size >= MAX_EMAILS)
        return;

    // Place the new email at the next open position.
    heap->data[heap->size] = email;

    // Increase the heap size before performing heapifyUp.
    heap->size++;

    // Move the new email upward if it has higher priority than its parent.
    heapifyUp(heap, heap->size - 1);
}

// Remove and return the highest-priority email from the heap.
Email removeMax(MaxHeap *heap)
{
    // Create an empty email to return when the heap has no emails.
    Email empty = {"", "", 0, 0, 0, 0, 0};

    // Return the empty email if there is nothing to remove.
    if(heap->size == 0)
        return empty;

    // Save the root email because it has the highest priority.
    Email maxEmail = heap->data[0];

    // Reduce the size because the root email is being removed.
    heap->size--;

    // Move the last email to the root and restore the heap property.
    if(heap->size > 0)
    {
        heap->data[0] = heap->data[heap->size];
        heapifyDown(heap, 0);
    }

    // Return the email that was removed from the root.
    return maxEmail;
}

// Return the highest-priority email without removing it from the heap.
Email peekMax(MaxHeap *heap)
{
    // Create an empty email to return when the heap has no emails.
    Email empty = {"", "", 0, 0, 0, 0, 0};

    // Return the empty email if there is nothing to display.
    if(heap->size == 0)
        return empty;

    // Return the root without changing the heap.
    return heap->data[0];
}


// -------------------- Display --------------------

// Print an email or an empty-queue message.
void printEmail(Email e)
{
    // An empty sender indicates that there is no email to display.
    if(strlen(e.sender) == 0)
    {
        printf("There are no emails to read.\n");
        return;
    }

    // Display the selected email's information.
    printf("Next email:\n");
    printf("\tSender: %s\n", e.sender);
    printf("\tSubject: %s\n", e.subject);
    printf("\tDate: %02d-%02d-%04d\n",
           e.month,
           e.day,
           e.year);
}


// -------------------- Parsing --------------------

// Parse an EMAIL command and add the resulting email to the heap.
void processEmailLine(MaxHeap *heap, char *line)
{
    // Create an email and initialize it so all fields have known values.
    Email e = {"", "", 0, 0, 0, 0, 0};

    // Use strtok to separate the sender, subject, and date fields.
    char *token;

    // Remove the newline so it is not included in the final field.
    line[strcspn(line, "\n")] = '\0';

    // Read the sender after the "EMAIL " command.
    token = strtok(line + 6, ",");
    if(token == NULL)
        return;

    // Copy the sender into the email structure.
    strncpy(e.sender, token, sizeof(e.sender) - 1);
    e.sender[sizeof(e.sender) - 1] = '\0';

    // Read the subject between the first and second commas.
    token = strtok(NULL, ",");
    if(token == NULL)
        return;

    // Copy the subject into the email structure without exceeding its buffer.
    strncpy(e.subject, token, sizeof(e.subject) - 1);
    e.subject[sizeof(e.subject) - 1] = '\0';

    // Read the date from the remaining field.
    token = strtok(NULL, ",");
    if(token == NULL)
        return;

    // Convert the date string into month, day, and year values.
    if(sscanf(token, "%d-%d-%d", &e.month, &e.day, &e.year) != 3)
        return;

    // Calculate the sender priority once instead of during every comparison.
    e.priorityLevel = senderPriority(e.sender);

    // Calculate the date value once for use during heap comparisons.
    e.dateValue = makeDateValue(e.month, e.day, e.year);

    // Add the completed email to the priority queue.
    insertEmail(heap, e);
}


// -------------------- Main --------------------

int main()
{
    // Create the max heap and start it with zero emails.
    MaxHeap heap;
    heap.size = 0;

    // Create a buffer large enough to hold one input line.
    char line[512];

    // Continue reading commands until the input reaches EOF.
    while(fgets(line, sizeof(line), stdin))
    {
        // Remove the newline character from the input command.
        line[strcspn(line, "\n")] = '\0';

        // Process an EMAIL command by parsing and inserting the email.
        if(strncmp(line, "EMAIL ", 6) == 0)
        {
            processEmailLine(&heap, line);
        }
        // NEXT displays the highest-priority email without removing it.
        else if(strcmp(line, "NEXT") == 0)
        {
            printEmail(peekMax(&heap));
        }
        // READ removes the highest-priority email from the queue.
        else if(strcmp(line, "READ") == 0)
        {
            removeMax(&heap);
        }
        // COUNT displays the number of emails currently in the queue.
        else if(strcmp(line, "COUNT") == 0)
        {
            printf("There are %d emails to read.\n",
                   heap.size);
        }
    }

    // End the program successfully after all input has been processed.
    return 0;
}
