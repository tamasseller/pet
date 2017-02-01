Pools
=====

Memory pools are memory management helpers - much simpler than a Heap - that can be used to group together several allocations request 
into a single block allocation, which can be satisfied by either static or dynamic (heap) allocation.

### Pool

The Pool class template implements a simple static memory pool, that can serve predefined number of objects of a fixed type. 
Its main purpose is manage the frames of a DynamicPool, but for simple scenarios probably it can be used alone as well.
Items can be obtained from and released to a pool in arbitrary order.

### Arena

The Arena is an extremely simlple **one-shot dynamic allocator** that can only allocate elements, but can not reclaim those.
It groups together the individual allocations to form bigger frames that are allocated at once.
The frames that are allocated during the lifecycle of the Arena object are freed up when the Arena is destroyed.

### DynamicPool

The DynamicPool class template implements a dynamic memory pool, that groups together the individual allocations to form
bigger frames that are allocated and freed at once. It can serve predefined number of objects of a fixed type from each frame.
Items can be obtained from and released to a pool in arbitrary order.
Incoming allocation requests use free space on the already allocated frames if there is any.
If it is out of free slots, then allocates a new frame.

### DynamicStack

The DynamicStack class template implements a dynamic stack-like pool, that groups together the individual allocations to form
bigger frames that are allocated and freed at once. It can serve predefined number of objects of a fixed type from each frame.
It can only allocate from the end (or top) of the virtual stack and only the last element can be freed. Also there is a dedicated
accessor for the top-of-stack element, to facilitate real stack like usage.

The allocation and freeing in this case is very similar to what is done on the call stack of a function.
The fundatmental difference is that it does not have to use a continuous memory area.
It also provides two differnt iterators that can traverse the contents in either direction.

The main goal of this special memory pool is to allow **substituting recursion with iteration** in cases where it is actually 
necessary to have a stack like storage.

### DynamicFifo

The DynamicFifo class template implements a **dynamically resized buffer**, that groups together the individual allocations to
form bigger frames that are allocated and freed at once. It can serve predefined number of objects of a fixed type from each frame.

It can only allocate from the end (or top) of the virtual FIFO buffer and only the first contained element can be 
freed. Also there is a dedicated accessor for the top-of-stack element.

It is somewhat similar to the STL deque container, but it is implemented with a segmented storage instead of a reallocating 
a continuous one.
Also it is very similar to the DynamicStack, but it has its frames linked in the opposite direction to allow constant time complexity
in-order traversal.

### StringCollector

The string collector serves a very specific purpose and it is probably not that universaly useful.
See the IdentifierRegistry container for details.

Pool concept
------------

A pool is capable of issuing and repossessing objects of a certain type only.
There are very few basic requirements that they need to fulfill in order to be drop-in replacements of each other.

**Requirements:**

 - The pool has to accept a type parameter _Element_ that is the type of the objects that are to be allocated.
 - The pool has to accept a positive integer parameter signifying the size parameter of the pool.
   This can be used as a hint for the anticipated number of elements to be stored.
 - The pool has to accept a type parameter _Allocator_ that is a type complying to the Allocator concept.
   This can be ignored, if not needed.
 - The pool has to provide an _acquire_ method that takes no parameters and returns a pointer to an _Element_ object
   allocated from the pool or null if it is exhausted.
 - The pool has to provide a _release_ method that takes a pointer to an _Element_ object that was allocated from this
   pool and reclaims it.
