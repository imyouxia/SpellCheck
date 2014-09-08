#ifndef DICT_H
#define DICT_H


#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <string.h>
#include <assert.h>

// 每个word的最大长度
#define LENGTH 45
// hash表的大小
#define BUCKET_COUNT 65521
//#define HASH_MULTIPLIER 65599;

#define false 0
#define true 1
typedef int bool;

// 统计词典里单词数目
unsigned int SIZE = 0;
// defines a node of the table 
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
} node;

// 将hash返回值相同的值单链表串起来
node *table[BUCKET_COUNT] = {NULL};

int hash(char *word);
bool load_dict(const char* dictionary);
double calctime(const struct rusage * before,const struct rusage *after);
bool check_in(const char *word);
void unload_dict();
void toLowerCase(char *word);

#endif

