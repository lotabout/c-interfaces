This repository is implementation of the library introduced in book "C
Interfaces and Implementation Techniques".

## Some Thoughts
The book is very good. Interfaces and implementations, which is more
important? For me and those who would like to discover all the details, the
implementations are really delicious. However, we should also focus on the
interface design. It can make us a better programmer.

## Some Complains
I use some other implementations for some of the libraries. Personally, I
don't like libraries that depends on each other. For example, After implement
the library "mem", the book will use it wherever it is needed. And mem also
depends on the library "exception".

For me, I want to learn all the details of a data structure or a library,
including the interfaces and implementations. And it would be good if each
libraries are seperated.

Of course, the libraries we create is meant to be used. With higher
abstraction, our mind can go further. However, I would like to exposes all the
details of implementation while learning. That means I don't have to
understand all the book even if all I want is to understand a single chapter.
