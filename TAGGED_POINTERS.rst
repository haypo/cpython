CPython fork reference: commit b26a0db8ea2de3a8a8e4b40e69fc8642c7d7cb68.


Memory Usage
============

Sizes on Linux x86-64.

* ``sys.getsizeof(None)``, ``sys.getsizeof(True)``, ``sys.getsizeof(False)``:
  0 bytes (tagged pointer)
* ``sys.getsizeof(100)``: 0 bytes (tagged pointer)
* ``sys.getsizeof(1000)`` 28 bytes (regular PyLongObject with 1 digit)
* ``list(range(200))``:

  * Reference: 7252 bytes
  * Tagged pointers: 1656 bytes (-5596 bytes, 4.4x less)

* ``(None, None, None)``:

  * Reference: 112 bytes
  * Tagged pointers: 64 bytes (-48 bytes, 1.8x less)

Code::

    import sys
    def size(l):
        return sys.getsizeof(l) + sum(sys.getsizeof(item) for item in l)
    print(size(list(range(200))), "bytes")
    print(size((None, None, None)), "bytes")


Optimize 1+1
============

Optimize int + int (BINARY_ADD) for tagged pointers in ceval.c.

Microbenchmark::

    env/bin/python -m pyperf timeit -s 'x=1; y=2' 'x+y' --duplicate=1024

Result::

    [ref] 14.3 ns +- 0.2 ns -> [optim] 8.76 ns +- 0.21 ns: 1.63x faster (-39%)


Benchmark
=========

Results using the commit b26a0db8ea2de3a8a8e4b40e69fc8642c7d7cb68
as the reference.

Run benchmark::

    # edit run_bench.sh to update BENCHS path
    ./run_bench.sh

Results at commit d7348e0aad "longobject.c get_tagged_value()":

+-----------+---------+------------------------------+
| Benchmark | ref     | tagptr                       |
+===========+=========+==============================+
| deltablue | 10.8 ms | 12.3 ms: 1.13x slower (+13%) |
+-----------+---------+------------------------------+
| go        | 364 ms  | 400 ms: 1.10x slower (+10%)  |
+-----------+---------+------------------------------+
| telco     | 8.49 ms | 9.44 ms: 1.11x slower (+11%) |
+-----------+---------+------------------------------+

Results at commit b5dc3a5a7fe649505c5f6938437a16fdde76c9e5
("Optimize Py_SIZE(), Py_TYPE(), MEDIUM_VALUE()").

+-----------+---------+-----------------------------+
| Benchmark | ref     | tagptr                      |
+===========+=========+=============================+
| deltablue | 10.9 ms | 11.8 ms: 1.08x slower (+8%) |
+-----------+---------+-----------------------------+
| go        | 346 ms  | 398 ms: 1.15x slower (+15%) |
+-----------+---------+-----------------------------+
| telco     | 8.18 ms | 8.77 ms: 1.07x slower (+7%) |
+-----------+---------+-----------------------------+


Main changes
============

* Py_IS_NONE(x): similar to "x == Py_None"
* Py_IS_TRUE(x): similar to "x == Py_True"
* Py_IS_FALSE(x): similar to "x == Py_False"

Use tagged pointers
===================

* Py_None, Py_RETURN_NONE
* Py_True, Py_RETURN_TRUE
* Py_False, Py_RETURN_FALSE
* get_small_int()


Compiler
========

Py_INCREF
---------

``Py_INCREF(op)``::

    if (_Py_TAGPTR_IS_TAGGED(op)) return;

is compiled to::

    # op = $rdi
    test   dil,0x7
    jne    ...

``Py_DECREF(op)`` gets the same test.

Py_TYPE
-------

``Py_TYPE(op)``::

    switch (_Py_TAGPTR_TAG(op)) {
    case _Py_TAGPTR_TAG_NONE: return &_PyNone_Type;
    case _Py_TAGPTR_TAG_TRUE: return &PyBool_Type;
    case _Py_TAGPTR_TAG_FALSE: return &PyBool_Type;
    case _Py_TAGPTR_TAG_INT: return &PyLong_Type;
    default:
        Py_UNREACHABLE();
    }

becomes a simple table lookup::

    # op -> $rax
    sub    eax,0x1
    mov    rbp,QWORD PTR [rax*8+0x6c9a60]
    # return $rbp (type)


Implementation
==============

_Py_TAGPTR_UNBOX()
------------------

For a tagged pointer, get a borrowed reference to a singleton.

Tagged integers are limited to the range [-5; 256].

Py_REFCNT()
-----------

Tagged pointers are immutable: pretent that their reference counter is always
2. Some functions modify objects in-place if their reference counter is equal
to 1.


Incompatible C API change
=========================

"x == Py_None" is no longer valid (same for "x != Py_None"). Same for Py_True
and Py_False.

"x == y" to test if "x == y == None" is no longer valid.

Dangerous code
==============

Don't write::

    v = _Py_TAGPTR_UNBOX(v)
    ...
    Py_DECREF(v);

Use::

    PyObject *unboxed_v = _Py_TAGPTR_UNBOX(v);
    ...
    Py_DECREF(v);

TODO
====

* Latin1 character singleton as tagged pointer? Need to patch 16k lines
  of unicodeobject.c to add UNBOX calls.
* Float as tagged pointer? _Py_TAGPTR_UNBOX() returns a borrowed reference.
* Fix test_gdb: implemented tagged pointer in python-gdb.py.


Previous attempt
================

Neil Schemenauer PoC (Sept 2018):

* https://mail.python.org/archives/list/capi-sig@python.org/thread/EGAY55ZWMF2WSEMP7VAZSFZCZ4VARU7L/#EGAY55ZWMF2WSEMP7VAZSFZCZ4VARU7L
* https://github.com/nascheme/cpython/commits/tagged_int

PyPy "Integers as Tagged Pointers" featur "Integers as Tagged Pointers" feature:
https://doc.pypy.org/en/latest/interpreter-optimizations.html#integers-as-tagged-pointers
