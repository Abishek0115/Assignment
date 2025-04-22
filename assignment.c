/* 
Name: Abishek S
Roll No: ME24B1053
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// Some size limits
#define NAME_LEN 20
#define Q_MAX 10
#define S_MAX 10
#define LOG_MAX 5
// For parts in system
typedef struct {
    char name[NAME_LEN];  // part name
} Part;
// For normal requests
typedef struct {
    Part parts[Q_MAX];  // parts in queue
    int start, end;     // positions
    int num;            // how many
} PartQueue;
// For urgent stuff
typedef struct {
    Part parts[S_MAX];  // emergency parts
    int top;            // top position
} UrgentStack;
// To remember usage
typedef struct {
    char action[NAME_LEN]; // what happened
    int time;              // when
} Usage;
// For leaky parts
typedef struct LeakyNode {
    Part part;           // the part
    struct LeakyNode* nxt; // next one
} LeakyNode;
// For fixed parts
typedef struct FixedNode {
    Part part;           // the part
    struct FixedNode* nxt; // next
    struct FixedNode* prv; // previous
} FixedNode;
// For important parts
typedef struct PrioNode {
    Part part;           // the part
    struct PrioNode* nxt; // next
} PrioNode;
// All system stuff
PartQueue pqueue;
UrgentStack estack;
Usage ulog[LOG_MAX];
int log_num = 0;
LeakyNode* leaky = NULL;
FixedNode* fixed = NULL;
PrioNode* prio = NULL;
// Setup queue
void setupQueue(PartQueue* q) {
    q->start = 0;
    q->end = -1;
    q->num = 0;
}
// Check empty queue
int qEmpty(PartQueue* q) {
    return q->num == 0;
}
// Check full queue
int qFull(PartQueue* q) {
    return q->num == Q_MAX;
}
// Add to queue
void addToQ(PartQueue* q, Part p) {
    if (qFull(q)) {
        printf("Queue full!\n");
        return;
    }
    q->end = (q->end + 1) % Q_MAX;
    q->parts[q->end] = p;
    q->num++;
    printf("Added %s\n", p.name);
}
// Remove from queue
Part removeFromQ(PartQueue* q) {
    Part empty = {""};
    if (qEmpty(q)) {
        printf("Queue empty!\n");
        return empty;
    }
    Part p = q->parts[q->start];
    q->start = (q->start + 1) % Q_MAX;
    q->num--;
    printf("Removed %s\n", p.name);
    return p;
}
// Setup stack
void setupStack(UrgentStack* s) {
    s->top = -1;
}
// Check empty stack
int sEmpty(UrgentStack* s) {
    return s->top == -1;
}
// Check full stack
int sFull(UrgentStack* s) {
    return s->top == S_MAX - 1;
}
// Push to stack
void pushToS(UrgentStack* s, Part p) {
    if (sFull(s)) {
        printf("Stack full!\n");
        return;
    }
    s->parts[++s->top] = p;
    printf("Pushed %s\n", p.name);
}
// Pop from stack
Part popFromS(UrgentStack* s) {
    Part empty = {""};
    if (sEmpty(s)) {
        printf("Stack empty!\n");
        return empty;
    }
    Part p = s->parts[s->top--];
    printf("Popped %s\n", p.name);
    return p;
}
// Log usage
void logUse(char* act) {
    if (log_num >= LOG_MAX) {
        printf("Log full, removing %s\n", ulog[0].action);
        for (int i = 0; i < LOG_MAX - 1; i++) {
            ulog[i] = ulog[i + 1];
        }
        strcpy(ulog[LOG_MAX - 1].action, act);
        ulog[LOG_MAX - 1].time = log_num;
    } else {
        strcpy(ulog[log_num].action, act);
        ulog[log_num].time = log_num;
    }
    log_num++;
    printf("Logged %s\n", act);
}
// Show usage
void showLog() {
    printf("\nUsage log:\n");
    int first = (log_num > LOG_MAX) ? (log_num % LOG_MAX) : 0;
    int total = (log_num > LOG_MAX) ? LOG_MAX : log_num;
    for (int i = 0; i < total; i++) {
        int pos = (first + i) % LOG_MAX;
        printf("%d: %s at %d\n", i, ulog[pos].action, ulog[pos].time);
    }
}
// Add leaky part
void addLeaky(Part p) {
    LeakyNode* new = malloc(sizeof(LeakyNode));
    if (!new) {
        printf("No memory!\n");
        return;
    }
    new->part = p;
    new->nxt = leaky;
    leaky = new;
    printf("Added leaky %s\n", p.name);
}
// Remove leaky part
void removeLeaky(char* name) {
    LeakyNode* curr = leaky;
    LeakyNode* prev = NULL;
    if (!curr) {
        printf("No leaky parts!\n");
        return;
    }
    if (strcmp(curr->part.name, name) == 0) {
        leaky = curr->nxt;
        printf("Removed %s\n", name);
        free(curr);
        return;
    }
    while (curr && strcmp(curr->part.name, name) != 0) {
        prev = curr;
        curr = curr->nxt;
    }
    if (!curr) {
        printf("%s not found\n", name);
        return;
    }
    prev->nxt = curr->nxt;
    printf("Removed %s\n", name);
    free(curr);
}
// Show leaky parts
void showLeaky() {
    printf("\nLeaky parts:\n");
    LeakyNode* curr = leaky;
    if (!curr) {
        printf("None!\n");
        return;
    }
    while (curr) {
        printf("Leak: %s\n", curr->part.name);
        curr = curr->nxt;
    }
}
// Add fixed part
void addFixed(Part p) {
    FixedNode* new = malloc(sizeof(FixedNode));
    if (!new) {
        printf("No memory!\n");
        return;
    }
    new->part = p;
    new->nxt = fixed;
    new->prv = NULL;
    if (fixed) {
        fixed->prv = new;
    }
    fixed = new;
    printf("Added fixed %s\n", p.name);
}
// Show fixed forward
void showFixedFwd() {
    printf("\nFixed parts (front to back):\n");
    FixedNode* curr = fixed;
    if (!curr) {
        printf("None!\n");
        return;
    }
    while (curr) {
        printf("Fixed: %s\n", curr->part.name);
        curr = curr->nxt;
    }
}
// Show fixed backward
void showFixedBwd() {
    printf("\nFixed parts (back to front):\n");
    FixedNode* curr = fixed;
    if (!curr) {
        printf("None!\n");
        return;
    }
    while (curr->nxt) {
        curr = curr->nxt;
    }
    while (curr) {
        printf("Fixed: %s\n", curr->part.name);
        curr = curr->prv;
    }
}
// Add important part
void addPrio(Part p) {
    PrioNode* new = malloc(sizeof(PrioNode));
    if (!new) {
        printf("No memory!\n");
        return;
    }
    new->part = p;
    if (!prio) {
        prio = new;
        new->nxt = prio;
    } else {
        PrioNode* temp = prio;
        while (temp->nxt != prio) {
            temp = temp->nxt;
        }
        temp->nxt = new;
        new->nxt = prio;
    }
    printf("Added priority %s\n", p.name);
}
// Show important parts
void showPrio(int times) {
    printf("\nPriority parts (%d cycles):\n", times);
    if (!prio) {
        printf("None!\n");
        return;
    }
    PrioNode* curr = prio;
    int cnt = 0;
    do {
        printf("Priority %d: %s\n", cnt, curr->part.name);
        curr = curr->nxt;
        cnt++;
    } while (curr != prio && cnt < times * 100);
}
// Test queue and stack
void testQueueStack() {
    printf("\nTesting queue and stack...\n");
    setupQueue(&pqueue);
    setupStack(&estack);
    Part parts[6] = {{"Pump"}, {"Pipe"}, {"Valve"}, {"Sensor"}, {"Sprinkler"}, {"Filter"}};
    for (int i = 0; i < 6; i++) {
        addToQ(&pqueue, parts[i]);
    }
    while (!qEmpty(&pqueue)) {
        Part p = removeFromQ(&pqueue);
        pushToS(&estack, p);
    }
    while (!sEmpty(&estack)) {
        popFromS(&estack);
    }
}
// Test logging
void testLogging() {
    printf("\nTesting logging...\n");
    logUse("Morning");
    logUse("Noon");
    logUse("Evening");
    logUse("Night");
    logUse("Test1");
    logUse("Test2");
    logUse("Test3");
    showLog();
}
// Test parts
void testParts() {
    printf("\nTesting parts...\n");
    Part pipe = {"Pipe"};
    Part sprinkler = {"Sprinkler"};
    addLeaky(pipe);
    addLeaky(sprinkler);
    showLeaky();
    removeLeaky("Pipe");
    addFixed(pipe);
    showLeaky();
    showFixedFwd();
    showFixedBwd();
    Part pump = {"Pump"};
    Part valve = {"Valve"};
    addPrio(pump);
    addPrio(valve);
    showPrio(2);
}
int main() {
    printf("Starting system...\n");
    testQueueStack();
    testLogging();
    testParts();
    printf("\nDone!\n");
    return 0;
}
