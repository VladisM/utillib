/**
 * @defgroup list_group Lists
 *
 * @brief Simple implementation for dynamic double linked lists.
 *
 * Queues and stacks are also implemented using list like macro to
 * a bit simplify ussage.
 *
 * Given data are copyied into internally allocated stack memory. When returning
 * data is copied at specified locations and internally allocated memory is deallocated
 * again. This way user do not have to care about memory mannagent of list, simply
 * call list_init() at beginning and list_destroy() at the end.
 *
 * @todo Add documentation.
 *
 * @ingroup core_group
 *
 * @{
 */

#ifndef LIST_H_included
#define LIST_H_included

#include <stddef.h>

typedef struct{
    struct list_item_s *first;
    struct list_item_s *last;
    unsigned count;
    size_t item_size;
}list_t;

typedef struct list_item_s{
    struct list_item_s *next;
    struct list_item_s *prev;
    void *data;
}list_item_t;

// Generic list
extern void list_init(list_t **list, size_t item_size);
extern void list_append(list_t *list, void *data);
extern void list_push(list_t *list, void *data);
extern void list_insert(list_t *list, unsigned position, void *data);
extern void list_pop(list_t *list, void *data);
extern void list_windraw(list_t *list, void *data);
extern unsigned list_count(list_t *list);
extern void list_at(list_t *list, unsigned position, void *data);
extern void list_peek(list_t *list, void *data);
extern void list_remove_at(list_t *list, unsigned position);
extern void list_destroy(list_t *list);

extern void list_copy(list_t *list_in, list_t **list_out);
extern void list_merge(list_t *list_A, list_t *list_B); //put items from B after last of A

// Use list to implement stack
typedef list_t stack_t;
extern void stack_init(stack_t **stack, size_t item_size);
extern void stack_push(stack_t *stack, void *data);
extern void stack_pop(stack_t *stack, void *data);
extern unsigned stack_count(stack_t *stack);
extern void stack_peek(stack_t *stack, void *data);
extern void stack_destroy(stack_t *stack);

extern void stack_copy(stack_t *stack_in, stack_t **stack_out);
extern void stack_merge(stack_t *stack_A, stack_t *stack_B);

// Use list to implement queue
typedef list_t queue_t;
extern void queue_init(queue_t **queue, size_t item_size);
extern void queue_append(queue_t *queue, void *data);
extern void queue_windraw(queue_t *queue, void *data);
extern unsigned queue_count(queue_t *queue);
extern void queue_destroy(queue_t *queue);

extern void queue_copy(queue_t *queue_in, queue_t **queue_out);
extern void queue_merge(queue_t *queue_A, queue_t *queue_B);

//debug purposes
#ifndef NDEBUG
extern void print_list(list_t *list, void (*print_data)(void *));
extern void print_stack(stack_t *stack, void (*print_data)(void *));
extern void print_queue(queue_t *queue, void (*print_data)(void *));
#endif

#endif

/**
 * @}
 */
