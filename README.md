Proper Embedded Templates
=========================

This is a collection of mostly unrelated utilities that are 
specifically designed to be used in advanced microcontroller firmware.

Some parts of this library use modern C++ features, thus it **requires
a decent compiler with stable C++17 support** (like GCC 6 or above).

Most of it is implemented as class templates, and some are even variadic.
But there is some - although very little - code that needs to be compiled and linked in.
Due to the typical use of embedded software components and the minimal 
amount of non-templated code no build suite is provided - simply feed those few cpp files to your own build system.

The structure of the source tree is segmented into directories based on topic, 
but please note that there are significant amount of cross including between the headers, 
so to keep things simple **add the root of the libraray tree to your include path** to get things going.

Intrusive containers (data)
---------------------------

These containers take a completely different route than the STL containers, and are supposed to
be more compatible with MMU-less (microcontroller) environment.  
In modern PC hardware using dynamic arrays whenever possible makes complete sense because the 
increased cache locality outweighs the ocassional copying of the whole array.
The abundance of RAM also makes it quite possible to allocate much more memory up front, then is 
actually needed, in order to avoid the copying when a capacity increase is needed.

However in small MMU-less embedded systems, there is no significant caching happening,
so there is no real benefit in maintaining spatial locality, in other words 'pointer chasing'
is not necassarily a drawback.

Also dynamic memory management needs to be carefully controlled, due to increased probability
of heap fragmentation, thus has to be decoupled from the containers as much as possible.

Heap implementation with some extra (heap)
------------------------------------------

Although completely dynamic memory management can be problematic in MMU-less systems, sometimes it is inevitable.
The heap solution provided is designed in a manner that enables the user to tweak all of the relevant aspects of 
its behavior, and also it is extensible by additional user supplied 'policies' (see docs).

It is designed to have minimal overhead and to provide blocks with arbitrary bits of alignment.
It also provides a checksumming feature, that is invaluable to detect application memory management errors.

All sorts of memory pools (pool)
--------------------------------

Memory pools are memory management helpers - much simpler than a Heap - that can be used to group together several 
allocations request into a single block allocation, which can be satisfied by either static or dynamic (heap) allocation.

There are many variants implemented here, all of which offer a different tradeoff between overhead and flexibility.

More automatic containers (managed)
-----------------------------------

These are much more STL like high-level containers based on the lower level things found in the _data_ directory.
These are mainly convenince wrappers, to be used in scenarios where the lower level ones do not offer any benefit.
Can be used with the provided pools (some of which can in turn be backed by a heap).

Metaprogramming utilites (meta)
-------------------------------

Embedded firmware can benefit pretty much from Zero-overhead C++ features alone, but template metaprogramming techniques
can explicitly shift several kind of computations to compile time leaving the application with pre-processed data.
However it is not for the faint hearted, so several utilities to ease the process are provided in this directory.

Things that could not fit into the others (algorithm)
-----------------------------------------------------

These are really just a bunch of random stuff that have proven to be useful, like: binary-search, fnv hash, string utilites.

Stuff that always come in handy (ubiquitous)
--------------------------------------------

Two things in here: a compile time configurable tracing helper, to avoid spoling the code with 'debug printfs'.
And a utility that helps to ensure that returned errors are checked utilizing an ingenious idea of mine :) . 
It is nice because if you can not do exceptions, wich is fairly tipical, systematic handling checking becomes an 
issue (after all, that is exactly why exceptions were invented).
