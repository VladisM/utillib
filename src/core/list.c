#include "list.h"

#include "dynmem.h"
#include "error.h"
#include "check.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static list_t *_new_list(size_t item_size);
static list_item_t *_new_list_item(size_t data_size);
static void _free_list_item(list_item_t *item);
static void _free_list(list_t *list);
static void _append_item(list_t *list, list_item_t *item, long position);
static list_item_t *_takeout_item(list_t *list, long position);
static list_item_t *_at(list_t *list, long position);
static void _destroy(list_t *list);

// -------------------------------------
// Implemenation of core functionality

static list_t *_new_list(size_t item_size){
    list_t *tmp = NULL;

    tmp = (list_t *)malloc(sizeof(list_t));
    dynmem_check_malloc((void *)tmp);

    tmp->count = 0;
    tmp->first = NULL;
    tmp->last = NULL;
    tmp->item_size = item_size;

    return tmp;
}

static list_item_t *_new_list_item(size_t data_size){
    list_item_t *tmp = NULL;

    tmp = (list_item_t *)malloc(sizeof(list_item_t));
    dynmem_check_malloc((void *)tmp);

    tmp->next = NULL;
    tmp->prev = NULL;

    tmp->data = malloc(data_size);
    dynmem_check_malloc(tmp->data);

    return tmp;
}

static void _free_list_item(list_item_t *item){
    free(item->data);
    free(item);
}

static void _free_list(list_t *list){
    free(list);
}

static void _append_item(list_t *list, list_item_t *item, long position){
    if(position == list->count)
        position = -1;

    if(list->count > 0){
        if(position == -1){
            list->last->next = item;
            item->prev = list->last;
            list->last = item;
        }
        else if(position == 0){
            item->next = list->first;
            list->first->prev = item;
            list->first = item;
        }
        else{
            list_item_t *head = list->first;

            for(long i = 1; i < position; i++)
                head = head->next;

            item->next = head->next;
            item->prev = head;
            head->next->prev = item;
            head->next = item;
        }
    }
    else{
        list->first = item;
        list->last = item;
    }

    list->count++;
}

static list_item_t *_takeout_item(list_t *list, long position){
    list_item_t *tmp = NULL;

    if(position == list->count - 1)
        position = -1;

    if(position == -1){
        tmp = list->last;

        if(list->first == list->last){
            list->first = NULL;
            list->last = NULL;
        }
        else{
            list->last = list->last->prev;
            list->last->next = NULL;
        }
    }
    else if(position == 0){
        tmp = list->first;

        if(list->first == list->last){
            list->first = NULL;
            list->last = NULL;
        }
        else{
            list->first = list->first->next;
            list->first->prev = NULL;
        }
    }
    else{
        tmp = list->first->next;

        for(long i = 1; i != position; i++){
            tmp = tmp->next;
        }

        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
    }

    tmp->prev = NULL;
    tmp->next = NULL;

    list->count--;

    return tmp;
}

static list_item_t *_at(list_t *list, long position){
    list_item_t *tmp = NULL;

    if(position == list->count - 1)
        position = -1;

    if(position == -1){
        tmp = list->last;
    }
    else if(position == 0){
        tmp = list->first;
    }
    else{
        list_item_t *head = list->first;

        for(long i = 0; i != position; i++){
            head = head->next;
        }

        tmp = head;
    }

    return tmp;
}

static void _destroy(list_t *list){
    list_item_t *head = list->first;
    list_item_t *tmp = NULL;

    while(head != NULL){
        tmp = head;
        head = head->next;
        _free_list_item(tmp);
    }

    _free_list(list);
}

// -------------------------------------
// Implemenation of generic lists

void list_init(list_t **list, size_t item_size){
    CHECK_NULL_ARGUMENT(list);
    CHECK_NOT_NULL_ARGUMENT(*list);

    *list = _new_list(item_size);
}

void list_append(list_t *list, void *data){
    CHECK_NULL_ARGUMENT(list);
    CHECK_NULL_ARGUMENT(data);

    list_item_t *item = _new_list_item(list->item_size);
    memcpy(item->data, data, list->item_size);
    _append_item(list, item, -1);
}

void list_push(list_t *list, void *data){
    CHECK_NULL_ARGUMENT(list);
    CHECK_NULL_ARGUMENT(data);

    list_item_t *item = _new_list_item(list->item_size);
    memcpy(item->data, data, list->item_size);
    _append_item(list, item, -1);
}

void list_insert(list_t *list, unsigned position, void *data){
    CHECK_NULL_ARGUMENT(list);
    CHECK_NULL_ARGUMENT(data);

    if(position > list->count){
        error("Position can't be larger than size of list!");
    }

    list_item_t *item = _new_list_item(list->item_size);
    memcpy(item->data, data, list->item_size);
    _append_item(list, item, (long)position);
}

void list_pop(list_t *list, void *data){
    CHECK_NULL_ARGUMENT(list);
    CHECK_NULL_ARGUMENT(data);

    if(list->count == 0){
        error("Called pop at empty list!");
    }

    list_item_t *tmp = NULL;

    tmp = _at(list, -1);
    memcpy(data, tmp->data, list->item_size);

    tmp = _takeout_item(list, -1);
    _free_list_item(tmp);
}

void list_windraw(list_t *list, void *data){
    CHECK_NULL_ARGUMENT(list);
    CHECK_NULL_ARGUMENT(data);

    if(list->count == 0){
        error("Called windraw at empty list!");
    }

    list_item_t *tmp = NULL;

    tmp = _at(list, 0);
    memcpy(data, tmp->data, list->item_size);

    tmp = _takeout_item(list, 0);
    _free_list_item(tmp);
}

unsigned list_count(list_t *list){
    CHECK_NULL_ARGUMENT(list);

    return list->count;
}

void list_at(list_t *list, unsigned position, void *data){
    CHECK_NULL_ARGUMENT(list);
    CHECK_NULL_ARGUMENT(data);

    if(position >= list->count){
        error("Position can't be larger than size of list!");
    }

    list_item_t *tmp = _at(list, (long)position);
    memcpy(data, tmp->data, list->item_size);
}

void list_peek(list_t *list, void *data){
    CHECK_NULL_ARGUMENT(list);
    CHECK_NULL_ARGUMENT(data);

    if(list->count == 0){
        error("Called peek at empty list!");
    }

    list_item_t *tmp = _at(list, -1);
    memcpy(data, tmp->data, list->item_size);
}

void list_remove_at(list_t *list, unsigned position){
    CHECK_NULL_ARGUMENT(list);

    if(list->count == 0){
        error("Called list_remove_at for empty list!");
    }

    if(position >= list->count){
        error("Position can't be larger than size of list!");
    }

    list_item_t *tmp = _takeout_item(list, position);
    _free_list_item(tmp);
}

void list_destroy(list_t *list){
    if(list != NULL)
        _destroy(list);
}

void list_copy(list_t *list_in, list_t **list_out){
    CHECK_NULL_ARGUMENT(list_in);
    CHECK_NULL_ARGUMENT(list_out);
    CHECK_NOT_NULL_ARGUMENT(*list_out);


    size_t item_size = list_in->item_size;
    list_t *tmp_list = _new_list(item_size);

    for(unsigned i = 0; i < list_in->count; i++){
        list_item_t *tmp_item = _new_list_item(item_size);
        list_item_t *item_in = _at(list_in, i);

        memcpy(tmp_item->data, item_in->data, item_size);
        _append_item(tmp_list, tmp_item, -1);
    }

    *list_out = tmp_list;
}

void list_merge(list_t *list_A, list_t *list_B){
    CHECK_NULL_ARGUMENT(list_A);
    CHECK_NULL_ARGUMENT(list_B);

    if(list_A->item_size != list_B->item_size){
        error("Merging two differently sized lists!");
    }

    size_t item_size = list_A->item_size;

    for(unsigned i = 0; i < list_B->count; i++){
        list_item_t *new_item = _new_list_item(item_size);
        list_item_t *item_to_copy = _at(list_B, i);

        memcpy(new_item->data, item_to_copy->data, item_size);
        _append_item(list_A, new_item, -1);
    }
}

// -------------------------------------
// implement stack using lists

void stack_init(stack_t **stack, size_t item_size){
    list_init((list_t **)stack, item_size);
}

void stack_push(stack_t *stack, void *data){
    list_push((list_t *)stack, data);
}

void stack_pop(stack_t *stack, void *data){
    list_pop((list_t *)stack, data);
}

unsigned stack_count(stack_t *stack){
    return list_count((list_t *)stack);
}

void stack_peek(stack_t *stack, void *data){
    list_peek((list_t *)stack, data);
}

void stack_destroy(stack_t *stack){
    list_destroy((list_t *)stack);
}

void stack_copy(stack_t *stack_in, stack_t **stack_out){
    list_copy((list_t *)stack_in, (list_t **)stack_out);
}

void stack_merge(stack_t *stack_A, stack_t *stack_B){
    list_merge((list_t *)stack_A, (list_t *)stack_B);
}

// -------------------------------------
// implement queue using lists

void queue_init(queue_t **queue, size_t item_size){
    list_init((list_t **)queue, item_size);
}

void queue_append(queue_t *queue, void *data){
    list_append((list_t *)queue, data);
}

void queue_windraw(queue_t *queue, void *data){
    list_windraw((list_t *)queue, data);
}

unsigned queue_count(queue_t *queue){
    return list_count((list_t *)queue);
}

void queue_destroy(queue_t *queue){
    list_destroy((list_t *)queue);
}

extern void queue_copy(queue_t *queue_in, queue_t **queue_out){
    list_copy((list_t *)queue_in, (list_t **)queue_out);
}

void queue_merge(queue_t *queue_A, queue_t *queue_B){
    list_merge((list_t *)queue_A, (list_t *)queue_B);
}

// -------------------------------------
// debug purposes

#ifndef NDEBUG
static void print_struct(list_t *list, char *name, void (*print_data)(void *)){
    if(list == NULL){
        fprintf(stdout, "%s NULL\n", name);
    }
    else{
        list_item_t *head = list->first;

        fprintf(stdout, "%s (count: %u)\n", name, list->count);

        while(head != NULL){
            if(head->next == NULL)
                fprintf(stdout, " '- ");
            else
                fprintf(stdout, " |- ");

            (*print_data)(head->data);
            fprintf(stdout, "\n");
            head = head->next;
        }
    }
    fflush(stdout);
}

void print_list(list_t *list, void (*print_data)(void *)){
    print_struct(list, "List", print_data);
}

void print_stack(stack_t *stack, void (*print_data)(void *)){
    print_struct((list_t *) stack, "Stack", print_data);
}

void print_queue(queue_t *queue, void (*print_data)(void *)){
    print_struct((list_t *) queue, "Queue", print_data);
}
#endif
