#ifndef VECTOR_H
#define VECTOR_H


#define VECTOR_TEMPLATE_S(TYPE, SIZE_TYPE, SUFFIX)                                              \
typedef struct {                                                                                \
    TYPE* val;                                                                                  \
    unsigned SIZE_TYPE size;                                                                    \
    unsigned SIZE_TYPE capacity;                                                                \
} Vector##SUFFIX ;                                                                              \
                                                                                                \
Vector##SUFFIX * init_vector##SUFFIX (Vector##SUFFIX * vector, const SIZE_TYPE capacity);       \
void push##SUFFIX (Vector##SUFFIX * vector, TYPE val);                                          \
void reserve##SUFFIX (Vector##SUFFIX * vector, const SIZE_TYPE size);


#define VECTOR_TEMPLATE_SDEF(TYPE, SIZE_TYPE, SUFFIX)                                           \
Vector##SUFFIX * init_vector##SUFFIX (Vector##SUFFIX * vector, const SIZE_TYPE capacity) {      \
    TYPE* res = malloc(sizeof(TYPE)*capacity);                                                  \
    vector->val = res;                                                                          \
    vector->capacity = capacity;                                                                \
    vector->size = 0;                                                                           \
    memset(res, 0, sizeof(TYPE)*capacity);                                                      \
    return vector;                                                                              \
}                                                                                               \
                                                                                                \
void push##SUFFIX (Vector##SUFFIX * vector, TYPE val) {                                         \
    if(vector->size == vector->capacity) {                                                      \
        TYPE* temp = malloc(sizeof(TYPE)*vector->capacity*2);                                   \
        memcpy(temp, vector, sizeof(TYPE)*vector->capacity);                                    \
        memset(temp+vector->capacity, 0, sizeof(TYPE)*vector->capacity);                        \
        vector->val = temp;                                                                     \
        vector->capacity *= 2;                                                                  \
    }                                                                                           \
    vector->val[vector->size] = val;                                                            \
    vector->size++;                                                                             \
}                                                                                               \
                                                                                                \
void reserve##SUFFIX (Vector##SUFFIX * vector, const SIZE_TYPE size) {                          \
    TYPE* temp = malloc(sizeof(TYPE)*(vector->capacity + size));                                \
    memcpy(temp, vector, sizeof(TYPE)*vector->capacity);                                        \
    memset(temp+vector->capacity, 0, sizeof(TYPE)*size);                                        \
    vector->val = temp;                                                                         \
    vector->capacity += size;                                                                   \
}

#define VECTOR_TEMPLATE(TYPE, SUFFIX) VECTOR_TEMPLATE_S(TYPE, long, SUFFIX)
#define VECTOR_TEMPLATE_DEF(TYPE, SUFFIX) VECTOR_TEMPLATE_SDEF(TYPE, long, SUFFIX)

#define FOR_VECTOR(TYPE, VECTOR, PREFIX, COMMAND) {const TYPE* const end = VECTOR.val + VECTOR.size; for(TYPE* PREFIX##itr = VECTOR.val; PREFIX##itr != end; ++PREFIX##itr) COMMAND}
#define FOR_VECTOR_REF(TYPE, VECTOR, PREFIX, COMMAND) {const TYPE* const end = VECTOR.val + VECTOR.size; for(TYPE* PREFIX##itr = VECTOR.val; itr != end; ++PREFIX##itr) {TYPE val = *PREFIX##itr; COMMAND}}

VECTOR_TEMPLATE(void*,)
VECTOR_TEMPLATE(char,8t)
VECTOR_TEMPLATE(short,16t)
VECTOR_TEMPLATE(int,32t)
VECTOR_TEMPLATE(long,64t)

#endif