@namespace container

@brief Intrusive containers.

These containers take a completely different route than the STL containers, and are supposed to
be more compatible with MMU-less (microcontroller) environment.  
In modern PC hardware using dynamic arrays whenever possible makes complete sense because the 
increased cache locality outweighs the ocassional copying of the whole array.
The abundance of RAM also makes it quite possible to allocate much more memory up front, then is 
actually needed, in order to avoid the copying when a capacity increase is needed.

However with small MMU-less embedded systems, there is no significant caching happening,
so there is no real benefit in spatial maintaining spacial locality, so 'pointer chasing'
is not necassarily a drawback.
Also dynamic memory management needs to be carefully controlled, due to increased probability
of heap fragmentation, thus has to be decoupled from the containers as much as possible.
Also unneded layers of indirection add memory overhead, at best.
And if it is happening from a heap then it also prone to increase fragmentation.

Another interresting aspect is the wired-in copy semantics of the STL: one can not simply pass
data over an STL cointainer without copying it.
And that is like so for a good reason: the container can keep the details of its implementation
for itself, which is nice but not always that terribly important.
However there are times when copying is out of question, namely in severely memory constrained
devices, that would like to manage comparatively large objects.

@dot digraph G {
  rankdir = LR;
  node[shape = box];
  
  { rank = same
    elem1[label = "List\nelement"];
    data1[label = "Data"];
  }

  { rank = same
    elem2[label = "List\nelement"];
    data2[label = "Data"];
  }

  { rank = same
    elem3[label = "List\nelement"];
    data3[label = "Data"];
  }

  node[shape = plaintext];
  so [label = "Soruce"];
  si [label = "Sink"];

  elem1 -> elem2 -> elem3;
  elem1 -> data1;
  elem2 -> data2;
  elem3 -> data3;

  edge[fontcolor = grey color = grey style = dashed] 
  so -> elem1 [label = "add"];
  so -> data1;

  elem3 -> si [label = "remove"];
  data3 -> si;
} @enddot

With STL this could only be done by passing over pointers which adds another layer of indirection
that is not strictly necessary and also splits the responsibility of allocating and releaseing
the list elements and the actual data between the two parties involved.

These containers being _intrusive_ means that some of the metadata needed to store the elements
of the container are required to be present and accessible in the stored element types. 
That way the application is enabled to get the elements from whatever source and pass it to the
container to store it only, and also 'gets it back' from the container when removed.

@dot digraph G {
  rankdir = LR;
  node[shape = record];
  
  elem1[label = "List\nelement|Data"];
  elem2[label = "List\nelement|Data"];
  elem3[label = "List\nelement|Data"];

  node[shape = plaintext];
  so [label = "Soruce"];
  si [label = "Sink"];

  elem1 -> elem2 -> elem3;

  edge[fontcolor = grey color = grey style = dashed] 
  so -> elem1 [label = "add"];
  elem3 -> si [label = "remove"];
} @enddot

Also these class templates are designed carefully so that compiler optimizations can take place, which
results in a configurable tradeoff between the size of the binary code and the runtime performance of it.

Iterator concept
----------------

Iterators are key components to generic programming because they are an interface between containers and generic algorithms.

The iterator concept that has been developed over these containers is fundamentally different to that of the STL.
The STL iterator concept is modelled after a pointer that is used to traverse the element of an array.
The presented aproach is modelled after traversing a linked list instead and actually it works much better for most
things, like trees and DynamicStacks, and basically anything except arrays.
However it not that tremendously important, because there are little to no really useful generic algorithms that work 
fine for both randomly and sequentaly accessible containers.

**Requirements**:

 1. Iterator types are provided by the containers as a nested class called _Iterator_.
 2. They also need to have a method with the name _iterator_ that takes no arguments and returns the adequate _Iterator_ 
    value that references the first element of the container.
 3. The iterators are required to have a _current_ method that returns a pointer to the currently referenced element if 
    there is one (that has the same type specified as the element type of the container).
    If the iterator has been stepped over the last element the _current_ method returns null.
 4. The iterators are also required to have a _step_ method that takes no arguments and has the return type of _void_.
    It steps the iterator to next element if there is any.
    If the iterator is at the last element then it step over and after this point the _current_ method always returns null.
