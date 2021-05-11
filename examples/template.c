#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <utillib/core.h>
#include <utillib/cli.h>

#define VERSION "v1.0"
#define PROGNAME "template"

typedef struct{
    bool help;
    bool version;
    bool foo;
}settings_t;

options_t *args = NULL;
settings_t settings;

void argparse(int argc, char **argv);
void memclean(void);

int main(int argc, char **argv){
    atexit_init();
    atexit_register(memclean);

    argparse(argc, argv);

    if(settings.help){
        options_print_help(args);
        exit(EXIT_SUCCESS);
    }
    else if(settings.version){
        options_print_version(args);
        exit(EXIT_SUCCESS);
    }
    else if(settings.foo){
        printf("Foo!\r\n");
        exit(EXIT_SUCCESS);
    }
    else{
        printf("No action specified!\r\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void argparse(int argc, char **argv){
    options_init(&args, VERSION, PROGNAME);

    options_append_flag_3(args,
        "h", "help",
        "Print this help."
    );
    options_append_flag_2(args,
        "version",
        "Print version info."
    );
    options_append_flag_3(args,
        "f", "foo",
        "Will set foo."
    );

    options_parse(args, argc, argv);

    if(options_is_flag_set(args, "h") || options_is_flag_set(args, "help"))
        settings.help = true;
    else
        settings.help = false;

    if(options_is_flag_set(args, "version"))
        settings.version = true;
    else
        settings.version = false;

    if(options_is_flag_set(args, "f") || options_is_flag_set(args, "foo"))
        settings.foo = true;
    else
        settings.foo = false;

    return;
}

void memclean(void){
    if(args != NULL)
        options_destroy(args);
}
