/**
 * @defgroup tokenizer_group Tokenizer
 *
 * @brief Tokenize input strings.
 *
 * @todo missing documentation
 *
 * @ingroup utils_group
 *
 * @{
 */

#ifndef TOKENIZER_H_included
#define TOKENIZER_H_included

#include <stdbool.h>
#include <utillib/core.h>

typedef struct{
    char *token;
    long line_number;
    long column;
    char *filename;
}token_t;

typedef struct tokenizer_s{
    array_t *buffer;
    queue_t *output;
    struct{
        bool comment_block_active;
        bool previous_char_was_comment_mark;
        bool string_block_active;
        char current_char;
        char previous_char;
        long current_line_number;
        long current_column_number;
        char *current_filename;
        struct{
            bool multiline_active;
            bool enabled;
        }clike_comments;
    }state;
    struct{
        bool (*is_comment_start)(struct tokenizer_s *this);
        bool (*is_comment_end)(struct tokenizer_s *this);
        bool (*is_separator)(struct tokenizer_s *this);
    }methods;
}tokenizer_t;

extern void tokenizer_init(tokenizer_t **tokenizer);

extern void tokenizer_tokenize_string(tokenizer_t *tokenizer, string_t *input);
extern void tokenizer_tokenize_char_string(tokenizer_t *tokenizer, char *input);
extern bool tokenizer_tokenize_file(tokenizer_t *tokenizer, char *filename);

/**
 * @brief
 *
 * @param tokenizer
 * @param output Contains an pointers to token_t structures. Each with one token.
 */
extern void tokenizer_end(tokenizer_t *tokenizer, queue_t **output);

/**
 * @brief Set up custom callback for comment block detection.
 *
 * @param tokenizer Tokenizer instance to be configured.
 * @param is_comment_start Function that will tell if current state is to comment.
 * @param is_comment_end Function that check end of comment block.
 *
 * @note If you used two chars to detect comment, raise flag in
 * this->state.previous_char_was_comment_mark
 */
extern void tokenizer_config_comment(
    tokenizer_t *tokenizer,
    bool (*is_comment_start)(tokenizer_t *this),
    bool (*is_comment_end)(tokenizer_t *this)
);

extern void tokenizer_config_separator(
    tokenizer_t *tokenizer,
    bool (*is_separator)(tokenizer_t *this)
);

/**
 * @brief Enable c like comments style.
 *
 * @param tokenizer Pointer to tokenizer object.
 *
 * @warning Working only with default comment detect methods!
 */
void tokenizer_config_enable_c_like_comment(
    tokenizer_t *tokenizer
);

extern void tokenizer_clean_output_queue(queue_t *output);

extern void tokenizer_token_destroy(token_t *token);

#endif

/**
 * @}
 */
