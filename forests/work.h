#ifndef WORK_H
#define WORK_H

struct command { 
    char oprtype;
    char* words[3];
};
typedef struct command command;

void operation_handle(Tree* treePointer, command* currant_command);

#endif /* WORK_H */