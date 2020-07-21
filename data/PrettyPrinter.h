#ifndef __PRETTY_data
#define __PRETTY_data

__attribute__((section(".debug_gdb_scripts"), used))
static inline const char __gdb_pretty_printer_data[] = R"foo(gdb.inlined-script

class listIterator:
    def __init__(self, ptr):
        self.ptr = ptr
        self.count = 0

    def __iter__(self):
        return self

    def __next__(self):
        if self.ptr == 0:
            raise StopIteration
        count, val = self.count, self.ptr.dereference()
        self.count, self.ptr = count + 1, val["next"]
        return ('[%d]' % count, val)

class linkedListPrinter:
    def __init__(self, val):
        self.val = val
	
    def display_hint(self):
        return 'array'

    def children (self):
        return listIterator(self.val["first"])

    def to_string (self):
        n = 0
        it = self.val["first"]
        while it != 0:
            n = n + 1
            it = it["next"]
        return "list with " + str(n) + " elements"

pp = gdb.printing.RegexpCollectionPrettyPrinter("pet")
pp.add_printer('LinkedList', '^pet::LinkedList.*$', linkedListPrinter)
pp.add_printer('DoubleList', '^pet::DoubleList.*$', linkedListPrinter)
gdb.printing.register_pretty_printer(gdb.current_objfile(), pp)
)foo";

#endif
