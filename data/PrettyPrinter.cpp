/*******************************************************************************
 *
 * Copyright (c) 2020 Tamás Seller. All rights reserved.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *******************************************************************************/

extern "C"
__attribute__((section(".debug_gdb_scripts"), used))
const char __gdb_pretty_printer_data[] = R"foo(gdb.inlined-script
import gdb.xmethod
import re

class listIterator:
    def __init__(self, ptr):
        self.ptr = ptr
        self.count = 0

    def __iter__(self):
        return self

    def __next__(self):
        if self.ptr == gdb.Value(0).cast(self.ptr.type):
            raise StopIteration

        ret = ("[%d]" % self.count, self.ptr)
        self.count, self.ptr = self.count + 1, self.ptr.dereference()['next'] 
        return ret

class linkedListPrinter:
    def __init__(self, val):
        self.first = val["first"]
    
    def display_hint(self):
        return 'array'

    def children (self):
        return listIterator(self.first)

    def to_string (self):
        n = 0
        it = self.first

        while it != gdb.Value(0).cast(it.type):
            n = n + 1
            it = it.dereference()['next']

        return "list with " + str(n) + " elements"

class unionPrinter:
    def __init__(self, val):
        self.val = val
    
    def display_hint(self):
        return None

    def to_string (self):
        tag = self.val["tag"]

        if tag < 0:
            return "Uninitialized union"

        type = self.val.type.template_argument(tag)
        return "Tagged union currently holding: '" + type.name + "'" 
        
    def children (self):
        tag = self.val["tag"]

        if 0 <= tag:
            daddr = self.val["data"].address
            type = self.val.type.template_argument(tag)
            value = daddr.cast(type.pointer()).dereference()
            return [("<content>", value)]

        return [];

class smartPtrPrinter:
    def __init__(self, val):
        self.val = val
    
    def display_hint(self):
        return None

    def to_string (self):
        target=self.val["target"]
        if target == 0:
            return "null"
        
        return "(" + self.val.type.template_argument(0).name + "*) " + str(target) + " [reference count:" + str(target.dereference()["counter"]["data"]) + "]"

class listIteratorPrinter:
    def __init__(self, val):
        self.val = val
    
    def display_hint(self):
        return None

    def to_string (self):
        pnx=self.val["prevsNext"]
        if pnx == 0:
            return "<after-the-end>"
        
        return str(pnx.dereference())

pp = gdb.printing.RegexpCollectionPrettyPrinter("pet")
pp.add_printer('LinkedList', '^pet::LinkedPtrList<.*>$', linkedListPrinter)
pp.add_printer('LinkedListIterator', '^pet::LinkedPtrList<.*>::Iterator$', listIteratorPrinter)
pp.add_printer('DoubleList', '^pet::DoubleList<.*>$', linkedListPrinter)
pp.add_printer('Union', '^pet::Union.*$', unionPrinter)
pp.add_printer('RefCnt', '^pet::RefCnt.*::Ptr.*$', smartPtrPrinter)
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
        if re.match('pet::DoubleList<[ \t\n]*[_a-zA-Z][ :_a-zA-Z0-9]*>', class_type.tag) and method_name == 'operator[]':
            return linkedListIndexerWorker(class_type)

gdb.xmethod.register_xmethod_matcher(None, linkedListIndexerMatcher(), True)

class refCntPtrDereferenceWorker(gdb.xmethod.XMethodWorker):
    def __init__(self, class_type):
        self.class_type = class_type
        self.target_type = class_type.template_argument(0)

    def get_arg_types(self):
        return []

    def get_result_type(self, *args):
        return self.target_type.reference()

    def __call__(self, obj, *args):
        return obj["target"].cast(self.target_type.pointer()).dereference()

class refCntPtrDereferenceMatcher(gdb.xmethod.XMethodMatcher):
    def __init__(self):
        gdb.xmethod.XMethodMatcher.__init__(self, 'refCntPtrDereferenceMatcher')
 
    def match(self, class_type, method_name):
        if re.match('^pet::RefCnt<.*>::Ptr<.*>$', class_type.tag) and method_name in ['operator->', 'operator*']:
            return refCntPtrDereferenceWorker(class_type)

gdb.xmethod.register_xmethod_matcher(None, refCntPtrDereferenceMatcher(), True)

)foo";
