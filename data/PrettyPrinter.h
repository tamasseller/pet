#ifndef __PRETTY_data
#define __PRETTY_data

__attribute__((section(".debug_gdb_scripts"), used))
static inline const char __gdb_pretty_printer_data[] = R"foo(gdb.inlined-script
import gdb.xmethod
import re

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

class unionPrinter:
    def __init__(self, val):
        self.val = val
	
    def display_hint(self):
        return None

    def to_string (self):
        tag = self.val["tag"]

        if tag < 0:
            return "uninitialized union"

        daddr=val["data"].address
        return daddr.cast(val.type.template_argument(tag).pointer()).dereference()

class smartPtrPrinter:
    def __init__(self, val):
        self.val = val
	
    def display_hint(self):
        return None

    def to_string (self):
        target=self.val["target"]
        if target == 0:
            return "null"
        
        return "target=" + str(target) + " [reference count:" + str(target.dereference()["counter"]["data"]) + "]"

pp = gdb.printing.RegexpCollectionPrettyPrinter("pet")
pp.add_printer('LinkedList', '^pet::LinkedList.*$', linkedListPrinter)
pp.add_printer('DoubleList', '^pet::DoubleList.*$', linkedListPrinter)
pp.add_printer('Union', '^pet::Union.*$', unionPrinter)
pp.add_printer('Union', '^pet::RefCnt.*::Ptr.*$', smartPtrPrinter)
gdb.printing.register_pretty_printer(gdb.current_objfile(), pp)

class linkedListIndexerWorker(gdb.xmethod.XMethodWorker):
    def __init__(self, class_type):
        self.class_type = class_type
        self.element_type = class_type.template_argument(0)

    def get_arg_types(self):
        return gdb.lookup_type('int')

    def get_result_type(self):
        return self.element_type.poiter()

    def __call__(self, obj, n):
        it = obj["first"]
        while n != 0 and it != 0:
            n = n - 1
            it = it["next"]
        return it

class linkedListIndexerMatcher(gdb.xmethod.XMethodMatcher):
    def __init__(self):
        gdb.xmethod.XMethodMatcher.__init__(self, 'linkedListIndexerMatcher')
 
    def match(self, class_type, method_name):
        print(class_type)
        print(method_name)
        if re.match('pet::DoubleList<[ \t\n]*[_a-zA-Z][ :_a-zA-Z0-9]*>', class_type.tag) and method_name == 'operator[]':
            return linkedListIndexerWorker(class_type)

gdb.xmethod.register_xmethod_matcher(None, linkedListIndexerMatcher())

)foo";

#endif
