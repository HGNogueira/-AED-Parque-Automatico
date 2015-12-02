#include"htable.h"
#include"defs.h"
#include"LinkedList.h"
#include<string.h>
#include<stdlib.h>

typedef struct _object{
    int t;
    char *id;
} Object;

struct _hashtable{
    int m;
    int p;
    LinkedList **table;
};

int hash(int m, int p, char *key){
    int h = 0, c;
    while( (c = (int) *key++) )
        h = h*p + c;
    h = h % m;
    return h;
}

HashTable *HTinit(int m, int p){
    int i;
    HashTable *ht;

    ht = (HashTable*) malloc(sizeof(HashTable));
    ht->m = m;
    ht->p = p;
    ht->table = (LinkedList**) malloc(sizeof(LinkedList*) * m);

    for(i = 0; i < m; i++)
        ht->table[i] = initLinkedList();
    return ht;
}

int HTinsert(HashTable *ht, int t, char *key){
    Object *object;
    int h = hash(ht->m, ht->p, key);

    object = (Object *) malloc(sizeof(Object));
    object->id = (char *) malloc(sizeof(char) * (strlen(key) + 1));
    strcpy(object->id, key);

    object->t = t;

    ht->table[h] = insertUnsortedLinkedList(ht->table[h], object);
        
    return h;
}

int HTget(HashTable *ht, char *key){
    Object *object;
    LinkedList *auxList;
    int h = hash(ht->m, ht->p, key);

    auxList = ht->table[h];

    while(auxList != NULL){
        object = (Object *) getItemLinkedList(auxList);
        if(strcmp(object->id, key) == 0)
            return object->t;
        object = (Object *) getItemLinkedList(auxList);
        auxList = getNextNodeLinkedList(auxList);
    }

    return -1;
}

void ObjectDestroy(Item t){
    Object *object = (Object *) t;
    free(object->id);
    free(object);
    return;
}

void HTdestroy(HashTable *ht){
    int i = 0;
    for(i = 0; i < ht->m; i++)
        freeLinkedList(ht->table[i], ObjectDestroy);

    free(ht->table);
    free(ht);
    return;
}
