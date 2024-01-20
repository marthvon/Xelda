#ifndef LINKED_LIST_H
#define LINKED_LIST_H


#define LINKED_LIST_TEMPLATE(TYPE, SUFFIX)                                                                                          \
typedef struct ListNode##SUFFIX {                                                                                                   \
    TYPE val;                                                                                                                      \
    struct ListNode##SUFFIX * prev;                                                                                                 \
    struct ListNode##SUFFIX * next;                                                                                                 \
} ListNode##SUFFIX ;                                                                                                                \
                                                                                                                                    \
typedef struct {                                                                                                                    \
    struct ListNode##SUFFIX* root;                                                                                                  \
    struct ListNode##SUFFIX* tail;                                                                                                  \
} LinkedList##SUFFIX ;                                                                                                              \
                                                                                                                                    \
ListNode##SUFFIX * create_list_node##SUFFIX (ListNode##SUFFIX* node, TYPE val);                                                     \
ListNode##SUFFIX * create_list_node##SUFFIX##_n (ListNode##SUFFIX* node, TYPE val, ListNode##SUFFIX* next);                         \
ListNode##SUFFIX * create_list_node##SUFFIX##_p (ListNode##SUFFIX* node, ListNode##SUFFIX* prev, TYPE val);                         \
ListNode##SUFFIX * create_list_node##SUFFIX##_pn (ListNode##SUFFIX* node, ListNode##SUFFIX* prev, TYPE val, ListNode##SUFFIX* next);\
                                                                                                                                    \
LinkedList##SUFFIX* init_linked_list##SUFFIX (LinkedList##SUFFIX * list);                                                           \
void free_linked_list##SUFFIX (LinkedList##SUFFIX * list);                                                                          \
void free_ptr_val_linked_list##SUFFIX (LinkedList##SUFFIX * list);                                                                          \
void push_back##SUFFIX (LinkedList##SUFFIX * list, TYPE val);                                                                       \
void push_front##SUFFIX (LinkedList##SUFFIX * list, TYPE val);                                                                      \
TYPE pop_back##SUFFIX (LinkedList##SUFFIX * list);                                                                                  \
TYPE pop_front##SUFFIX (LinkedList##SUFFIX * list);                                                                                 \
void insert_node##SUFFIX (LinkedList##SUFFIX * list, ListNode##SUFFIX * node, ListNode##SUFFIX * new_node);                         \
void remove_node##SUFFIX (LinkedList##SUFFIX * list, ListNode##SUFFIX * node);


#define LINKED_LIST_TEMPLATE_DEF(TYPE, SUFFIX)                                                                                          \
ListNode##SUFFIX * create_list_node##SUFFIX (ListNode##SUFFIX * node, TYPE val) {                                                       \
    node->val = val;                                                                                                                    \
    node->prev = NULL;                                                                                                                  \
    node->next = NULL;                                                                                                                  \
    return node;                                                                                                                        \
}                                                                                                                                       \
                                                                                                                                        \
ListNode##SUFFIX * create_list_node##SUFFIX##_n (ListNode##SUFFIX * node, TYPE val, ListNode##SUFFIX * next) {                          \
    node->val = val;                                                                                                                    \
    node->prev = NULL;                                                                                                                  \
    node->next = next;                                                                                                                  \
    return node;                                                                                                                        \
}                                                                                                                                       \
                                                                                                                                        \
ListNode##SUFFIX * create_list_node##SUFFIX##_p (ListNode##SUFFIX * node, ListNode##SUFFIX * prev, TYPE val) {                          \
    node->val = val;                                                                                                                    \
    node->prev = prev;                                                                                                                  \
    node->next = NULL;                                                                                                                  \
    return node;                                                                                                                        \
}                                                                                                                                       \
                                                                                                                                        \
ListNode##SUFFIX * create_list_node##SUFFIX##_pn (ListNode##SUFFIX * node, ListNode##SUFFIX * prev, TYPE val, ListNode##SUFFIX * next) {\
    node->val = val;                                                                                                                    \
    node->prev = prev;                                                                                                                  \
    node->next = next;                                                                                                                  \
    return node;                                                                                                                        \
}                                                                                                                                       \
                                                                                                                                        \
LinkedList##SUFFIX * init_linked_list##SUFFIX (LinkedList##SUFFIX * list) {                                                             \
    list->root = NULL;                                                                                                                  \
    list->tail = NULL;                                                                                                                  \
    return list;                                                                                                                        \
}                                                                                                                                       \
void free_linked_list##SUFFIX (LinkedList##SUFFIX * list) {                                                                             \
    ListNode##SUFFIX * curr = list->root;                                                                                               \
    while(curr != NULL) {                                                                                                               \
        ListNode##SUFFIX * next = curr->next;                                                                                           \
        free(curr);                                                                                                                     \
        curr = next;                                                                                                                    \
    }                                                                                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
void push_back##SUFFIX (LinkedList##SUFFIX * list, TYPE val) {                                                                          \
    if(list->tail == NULL) {                                                                                                            \
        list->tail = create_list_node##SUFFIX (malloc(sizeof(ListNode##SUFFIX)), val);                                                  \
        list->root = list->tail;                                                                                                        \
        return;                                                                                                                         \
    }                                                                                                                                   \
    list->tail->next = create_list_node##SUFFIX##_p (malloc(sizeof(ListNode##SUFFIX)), list->tail, val);                                \
    list->tail = list->tail->next;                                                                                                      \
}                                                                                                                                       \
                                                                                                                                        \
void push_front##SUFFIX (LinkedList##SUFFIX * list, TYPE val) {                                                                         \
    if(list->root == NULL) {                                                                                                            \
        list->root = create_list_node##SUFFIX (malloc(sizeof(ListNode##SUFFIX)), val);                                                  \
        list->tail = list->root;                                                                                                        \
        return;                                                                                                                         \
    }                                                                                                                                   \
    list->root->prev = create_list_node##SUFFIX##_n (malloc(sizeof(ListNode##SUFFIX)), val, list->root);                                \
    list->root = list->root->prev;                                                                                                      \
}                                                                                                                                       \
                                                                                                                                        \
TYPE pop_back##SUFFIX (LinkedList##SUFFIX * list) {                                                                                     \
    ListNode##SUFFIX * temp = list->tail;                                                                                               \
    TYPE res = temp->val;                                                                                                               \
                                                                                                                                        \
    list->tail = temp->prev;                                                                                                            \
    free(temp);                                                                                                                         \
    if(list->tail == NULL)                                                                                                              \
        list->root = NULL;                                                                                                              \
    else                                                                                                                                \
        list->tail->next = NULL;                                                                                                        \
                                                                                                                                        \
    return res;                                                                                                                         \
}                                                                                                                                       \
                                                                                                                                        \
TYPE pop_front##SUFFIX (LinkedList##SUFFIX * list) {                                                                                    \
    ListNode##SUFFIX * temp = list->root;                                                                                               \
    TYPE res = temp->val;                                                                                                               \
                                                                                                                                        \
    list->root = temp->next;                                                                                                            \
    free(temp);                                                                                                                         \
    if(list->root == NULL)                                                                                                              \
        list->tail = NULL;                                                                                                              \
    else                                                                                                                                \
        list->root->prev = NULL;                                                                                                        \
                                                                                                                                        \
    return res;                                                                                                                         \
}                                                                                                                                       \
                                                                                                                                        \
void insert_node##SUFFIX (LinkedList##SUFFIX * list, ListNode##SUFFIX * node, ListNode##SUFFIX * new_node) {                            \
    if(node != NULL) {                                                                                                                  \
        new_node->next = node->next;                                                                                                    \
        if (node != list->tail)                                                                                                         \
            node->next->prev = new_node;                                                                                                \
        else                                                                                                                            \
            list->tail = new_node;                                                                                                      \
                                                                                                                                        \
        new_node->prev = node;                                                                                                          \
        node->next = new_node;                                                                                                          \
    } else {                                                                                                                            \
        new_node->next = list->root;                                                                                                    \
        list->root->prev = new_node;                                                                                                    \
        list->root = new_node;                                                                                                          \
        if(new_node->next == NULL)                                                                                                      \
            list->tail = new_node;                                                                                                      \
    }                                                                                                                                   \
}                                                                                                                                       \
                                                                                                                                        \
void remove_node##SUFFIX (LinkedList##SUFFIX * list, ListNode##SUFFIX * node) {                                                         \
    if(node != list->root)                                                                                                              \
        node->prev->next = node->next;                                                                                                  \
    else                                                                                                                                \
        list->root = node->next;                                                                                                        \
                                                                                                                                        \
    if(node != list->tail)                                                                                                              \
        node->next->prev = node->prev;                                                                                                  \
    else                                                                                                                                \
        list->tail = node->prev;                                                                                                        \
                                                                                                                                        \
    free(node);                                                                                                                         \
}


#define FOR_LIST_AT(SUFFIX, PREFIX, NODE, COMMAND) for(ListNode##SUFFIX * PREFIX##itr = NODE; PREFIX##itr != NULL; PREFIX##itr = PREFIX##itr->next) COMMAND
#define FOR_LIST(SUFFIX, PREFIX, LIST, COMMAND) FOR_LIST_AT(SUFFIX, PREFIX, LIST.root, COMMAND)
#define REVERSE_FOR_LIST_AT(SUFFIX, PREFIX, NODE, COMMAND) for(ListNode##SUFFIX * PREFIX##itr = NODE; PREFIX##itr != NULL; PREFIX##itr = PREFIX##itr->prev) COMMAND
#define REVERSE_FOR_LIST(SUFFIX, PREFIX, LIST, COMMAND) REVERSE_FOR_LIST_AT(SUFFIX, PREFIX, LIST.tail, COMMAND)

#define FOR_LIST_REF_AT(TYPE, SUFFIX, PREFIX, NODE, COMMAND) for(ListNode##SUFFIX * PREFIX##itr = NODE; PREFIX##itr != NULL; PREFIX##itr = PREFIX##itr->next) { TYPE val = (TYPE)(PREFIX##itr->val); COMMAND }
#define FOR_LIST_REF(TYPE, SUFFIX, PREFIX, LIST, COMMAND) FOR_LIST_REF_AT(TYPE, SUFFIX, PREFIX, LIST.root, COMMAND)
#define REVERSE_FOR_LIST_REF_AT(TYPE, SUFFIX, PREFIX, NODE, COMMAND) for(ListNode##SUFFIX * PREFIX##itr = NODE; PREFIX##itr != NULL; PREFIX##itr = PREFIX##itr->prev) { TYPE val = (TYPE)(PREFIX##itr->val); COMMAND }
#define REVERSE_FOR_LIST_REF(TYPE, SUFFIX, PREFIX, LIST, COMMAND) REVERSE_FOR_LIST_REF_AT(TYPE, SUFFIX, PREFIX, LIST.root, COMMAND)

LINKED_LIST_TEMPLATE(void*,)
LINKED_LIST_TEMPLATE(char,8t)
LINKED_LIST_TEMPLATE(short,16t)
LINKED_LIST_TEMPLATE(int,32t)
LINKED_LIST_TEMPLATE(long,64t)

#endif