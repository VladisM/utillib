#include "string_cache.h"

#include <string.h>

void string_cache_new(string_cache_t **cache){
    list_init(cache, sizeof(char *));
}

void string_cache_destroy(string_cache_t *cache){
    if(cache != NULL){
        while(list_count(cache) > 0){
            char *tmp = NULL;
            list_windraw(cache, (void *)&tmp);
            dynmem_free(tmp);
        }

        list_destroy(cache);
    }
}

char *string_cache_process(string_cache_t *cache, char *string){
    CHECK_NULL_ARGUMENT(cache);
    CHECK_NULL_ARGUMENT(string);

    for(unsigned int i = 0; i < list_count(cache); i++){
        char *head = NULL;
        list_at(cache, i, (void *)&head);

        if(strcmp(head, string) == 0){
            return head;
        }
    }

    // If the string is not found in the cache, add it
    char *tmp = dynmem_strdup(string);
    list_append(cache, (void *)&tmp);

    return tmp;

}