Heap
====

The heap solution provided consists of two layers: the host and a policy layer.
An actual heap is made of the host component and one policy bound together.
Also the user can easily create additional policies if that fit his/her specific requirements more closely.

### Techincalities

The host and the policies are implemented in the form of class templates, under the name Heap for the host
and < somename >Policy for the 'builtin' policies.
The binding of a policy with the host is techincally done by instantiating the both with adequate parameters.
The policy does not depend strictly on the heap itself, instead there is a common base for both, that provides
the entities that are required for the interoperation of both layers, that is the HeapBase template.
Both the host and each policy inherits from the HeapBase.
The template arguments of the HeapBase has to match for the host and the policy that are to be bound together.
The choosen policy becomes a base class of the host, so the host does not inherit from the HeapBase directly, 
to avoid ambiguity. 
there is a HeapHostBase in between the Host and the HeapBase, it also adds features to the common entities
that are not required for the policy, but only for the host.

### Host features

The host manages splitting and joining blocks of free or used memory.
Its basic responsibility is only for the 'packetization' of the memory according the needs of the application.
It manages (knows about) both free and used blocks, mainly to split the available free blocks to exact length
and keep freed up blocks joined together to form the largest possible continuous areas, in order to avoid
fragmentation of the heap.

It also provides a checksumming feature, that is invaluable to detect application memory management errors.

The industry standard stdlibc like dynamic memory management systems provide only an allocation-release pair
of operations, that is most of the times just enough, these are the _malloc_ and _free_ functions.
There are usually an additional _realloc_ function, that - depending on the implementation - either enlarges
the block without copying if possible or not. 
But obviously it can not do it always without relocating, and thus copying the data over to a new place, that
has enough continuous space.
However the internal structure of the heap enables an additional operation without any significant overhead:
shrinking an allocated block to a size smaller than it is.
This operation can always be done without copying, and all the other internal operations make it possible to
preserve the realtime capabilities of the heap.

@see The documentation of Heap class template for an in-depth overview of the internal data structure.

### Policies

There several refernce heap policies provided with different tradeoffs, but new ones can be implemented 
based on the currently implemented ones with minimal effort.
Here is a brief overview of the builtin heap policies, in terms of:

 - Time complexity: the highest of the worst case time complexities of the policy operations (insert, update, remove, find matching).
 - Static (one-time) cost: the statically allocated 'house-keeping' storage of the heap.
 - Per-block cost: the size required inside the payload area of a free block to include it in the free-store.
   This along with the heap headers' size determines the actual minimal size of an allocation, because no block
   can be allocated with a smaller payload area than what is needed for storing it in the free store when released.

| Policy          | Time-complexity         | Static storage cost       | Per-block cost         | Match accuracy   |
| --------------- | -------------------     | ------------------------- | ---------------------- | ---------------- |
| _TlsfPolicy_    | Constant (**realtime**) | 256 pointers + 17 x 2byte | 2 pointers             | Rounded best fit |
| _AvlTreePolicy_ | Logarithmic             | 1 pointer                 | 3 pointers + 2 * 2byte | Exact best fit   |
| _BestFitPolicy_ | Linear                  | 1 pointer                 | 1 pointer              | Exact best fit   |

It is obvious from these figures that each policy has its strengths and weaknesses, there is no single best one.

The two level segregated fit policy provides realtime capability that is a must in some scenarios, however all the
other properties are worse than the others'. 
The especially weak parts are that it requires significant static storage, and also that it does not provide an
exact match, which can impact the _fragmentation_ negatively. 
Although the effect of this is _neglectable in most scenarios_.

The AvlTree based policy provides the anticipaced logarithmic time complexity with exact matches which scales very 
well, but on the other hand it has huge per-block cost, because it has to hold not only the two child and one parent
pointers but also the additional metadata needed for the balancing algorithm.

The LinkedList based best fit policy is by far the simplest and also has (optimally) small storage cost, but it can
have serious scalabilty problems, should the application do frequent allocation and release of numerous small blocks.

### Further improvements

Hints could be introduced to enable tailoring the selection algorithm of block allocation from the free store to enable
application aided locality enhancement, in order to reduce heap fragmentation.

That could be used for example for:

 - Simple hot-cold separation of blocks,
 - Grouping blocks with related lifecycles,
 - Grouping blocks with related contexts (threads maybe).

However this feature to be useful or even not to be counterproductive requires the application to provide useful hints.
This places a the burden of strict lifecycle and context planning on the application developer, so it would hardly be used widely.

Allocator concept
-----------------

Just as with the containers a very different aproach is taken compared to that of the STL.
Allocators used in here are very simple types that provide a plain stdlibc like interface.
The only point being that by using this abstraction the low-level dynamic memory manager
becomes a declaratively injectable dependency of the client. 

**Requirements:**

 - The allocator has to provide a static method that allocates uninitialized memory and returns a _void_ pointer to the 
   lowest address of the allocated block.
   This method has to be named as _alloc_ and it receives one positive integer argument that signifies the requested amount
   of memory in bytes.
 - The allocator also has to provide a static method that reclaims memory that have become unused.
   This method has to be named as _free_ and it receives one _void_ pointer argument that is the lowest address of 
   the block. 
   This address is one that was returned by the _alloc_ method, and have not been freed since.
 - The allocator also has to provide a static method that takes a hint of freeing up a portion of an allocated block.
   This method has to be named as _shrink_ and it receives a _void_ pointer argument that is the lowest address of 
   the block and an unsigned integer that specifies the desired final size of the block in bytes as a second argument.
   It returns the new size of the block, that can not be lower then the required size and can not be higher then the original.
   This is method may be left unimplemented by siply returning the required size, however it is beneficial for some containers
   to provide an implementation that frees up the surplus in order to conserve heap space.
   The address passed to the _shrink_ method is one that was returned by the _alloc_ method, and have not been freed since.

It is not coincidence that the heap contains methods with very similar signature to that of an Allocator.
Note that though it is very analogous the allocator is required to provide **static** methods.
But as the heap objects themselves are usual statically allocated (from where would you allocate the first one otherwise)
an allocator can be implemented trivially to wrap a static instance of a heap object.
