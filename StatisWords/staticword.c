#include "dict.h"

// 默认词典
#define DICTIONARY "./words"

/**
 * Copypasta from COS 217.
 * Returns a hash code in [0, BUCKET_COUNT - 1] for word.
 */
int hash(char *word)
{
    assert(word != NULL);
    enum {HASH_MULTIPLIER = 65599};
    int i;
    unsigned int uiHash = 0U;
    
    for(i = 0; word[i] != '\0'; i++)
    {
        uiHash = uiHash * (unsigned int)HASH_MULTIPLIER+(unsigned int)word[i];

    }
    return (int)(uiHash % (unsigned int)BUCKET_COUNT);

}

// 加载词典
bool load_dict(const char* dictionary)
{
    FILE *dict = fopen(dictionary,"r");
    if(dict == NULL)
        return false;
    
    while(!feof(dict))
    {
        node *hnode = malloc(sizeof(struct node));
        if(hnode == NULL)
        {
            fclose(dict);
            return false;
        }     

        // 从字典里挨个读取单词
        int count = fscanf(dict,"%s",hnode->word);
        if(count == 1)
        {
            SIZE = SIZE + 1;
        }

        // 将单词存入Hash表里 
        int index = hash(hnode->word);
        
        // 单链表形式存入
        if(table[index] == NULL)
        {
            table[index] = hnode;
            hnode->next = NULL;
        }
        else
        {
            hnode->next = table[index];
            table[index] = hnode;
        }
    }
    fclose(dict);
    return true;
}

// 返回after - before 之家的秒数
double calctime(const struct rusage *before,const struct rusage *after)
{
    if(before == NULL || after == NULL)
    {
        return 0.0;
    }
    else
    {
        return ((((after->ru_utime.tv_sec * 1000000 + after->ru_utime.tv_usec) - (before->ru_utime.tv_sec * 1000000 + before->ru_utime.tv_usec)) + ((after->ru_stime.tv_sec * 1000000 + before->ru_stime.tv_usec) - (before->ru_stime.tv_sec * 1000000 + before->ru_stime.tv_usec))) / 1000000.0); 
    }

}

void toLowerCase(char *word)
{
    int i;
    for(i = 0; word[i]; i++)
    {
        word[i] = tolower(word[i]);
    }
}

bool check_in(const char *word)
{
    int len = strlen(word);
    char key[len];
    strcpy(key,word);
    
    toLowerCase(key);
    
    node *ptr = table[hash(key)];
    // 存在
    while(ptr != NULL)
    {
        if(strcmp(ptr->word,key) == 0)
            return true;
        else
            ptr = ptr->next;
    }

    return false;
}

// 释放内存
void unload_dict()
{
    int i;
    for(i = 0; i < BUCKET_COUNT;i++)
    {
        if(table[i] != NULL)
        {
            node *ptr = table[i];
            while(ptr != NULL)
            {
                node *prev = ptr;
                ptr = ptr->next;
                free(prev);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        printf("Usage:StatisWord [dictionary] text\n");
        return 0;
    }
    
    struct rusage before,after;
    double time_load = 0.0,time_check = 0.0,time_unload = 0.0;

    // 判断是否使用了词典
    char *dictionary = (argc == 3) ? argv[1] : DICTIONARY;

    // 加载词典
    getrusage(RUSAGE_SELF,&before);
    bool load = load_dict(dictionary);
    getrusage(RUSAGE_SELF,&after);
    
    if(!load)
    {
        printf("load %s error!\n",dictionary);
    }
    // 计算加载字典需要的时间
    time_load = calctime(&before,&after);
    printf("Time in load:%.2f\n",time_load);
    
    // 打开文本
    char *text = (argc == 3) ? (argv[2]) : argv[1];
    FILE *fp = fopen(text,"r");
    if(fp == NULL)
    {
        printf("Open %s Error!\n",text);
        unload_dict();
        return 1;     
    }

    printf("\nMISSSPELLED WORDS IS:\n\n");

    char word[LENGTH];
    int n = 0, miss = 0,words = 0;
    
    int c;
    for(c = fgetc(fp); c != EOF; c = fgetc(fp))
    {
        if(isalpha(c))
        {
            word[n] = c;
            n++;
            if(n > LENGTH)
            {
                while((c = fgetc(fp)) != EOF && isalpha(c));
                n = 0;
            }
         
        }
        else if(isdigit(c))
        {
            while((c = fgetc(fp)) != EOF && isalnum(c));
            n = 0;
        }
        else if(n > 0)
        {
            word[n] = '\0';
            // 处理完每一个单词，单词个数加1
            words++;
            // 检测单词拼写
            getrusage(RUSAGE_SELF,&before);
            bool check = check_in(word);
            getrusage(RUSAGE_SELF,&after);
            
            time_check += calctime(&before,&after);

            if(!check)
            {
                printf("%s\n",word);
                miss++;
            }
            // 下一个单词
            n = 0;
        }        
    }
    
    if(ferror(fp))
    {
        fclose(fp);
        printf("Read %s Error!\n",text);
        unload_dict();
        return 1;
    } 
    fclose(fp);
    
    printf("Words In Dictionary: %d\n",SIZE);
    printf("Words In Text: %d\n",words);
    
    getrusage(RUSAGE_SELF,&before);
    unload_dict();
    getrusage(RUSAGE_SELF,&after);

    time_unload = calctime(&before,&after);
    
    printf("Time in Check: %.2f\n",time_check);
    printf("Time in unload: %.2f\n",time_unload);
    printf("All Time: %.2f\n",time_load + time_check + time_unload);
    return 0;
}


