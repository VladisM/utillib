Simple framework for C
======================

Why another C framework? Why not?!

Programming C applications can be seen as little bit old fashion. But there is
still plenty of problems that require solution in pure C. One such problem may
be writing software for custom CPU architecture. If you write down all
requirements that C++ have to linker, you will worship pure ansi C!

What is included?
-----------------------

This library will group together only some very basic utilities, such as
linked lists, converting numbers, argument parsing and so on.

* core/array.c - Variable length array implementation.
* core/atexit.c - Wrapper that will deal with finite count of at exist calls.
* core/dynmem.c - Simple wrapper for memory allocation.
* core/error.c - Wrapper for exit(EXIT_FAILURE), will print call stack.
* core/check.c - Something to sanitize arguments passed to function.
* core/list.c - Double linked list (also queue and stack).
* core/string.c - C now have dynamically reallocated string object.
* cli/options.c - Argument parsing.
* cli/question.c - Simplify user input.
* files/ihex.c - Support for Intel Hex files.
* files/mif.c - Support for memory initialization format used in Quartus.
* utils/convert.c - Parsing strings to number.
* utils/error_buffer.c - Accommodate error string.
* utils/evaluate.c - Calculator! Convert infix to postfix and solve it.
* utils/tokenizer.c - To tokenize input files. Also contain comments.

Build
-----------------------

To build this library you can use CMakeLists.txt included in repository.
At this time, 4 static libraries will be build.

Documentation
-----------------------

Code is prepared to be documented by Doxygen. But doxyfile is not included.
For simple example how to use this framework to create simple cli application
you should try to visit examples/template.c.

TODO
-----------------------

Some modules still doesn't have full documentation.

License
-----------------------

Copyright © 2021 Vladislav Mlejnecký <v.mlejnecky@seznam.cz>

This work is free. You can redistribute it and/or modify it under the
terms of the Do What The Fuck You Want To Public License, Version 2,
as published by Sam Hocevar. See COPYING file for more details.
