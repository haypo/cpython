CPython fork reference: commit b26a0db8ea2de3a8a8e4b40e69fc8642c7d7cb68


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

tagged_ptr branch at commit b5dc3a5a7fe649505c5f6938437a16fdde76c9e5
("Optimize Py_SIZE(), Py_TYPE(), MEDIUM_VALUE()"), compared to reference
commit b26a0db8ea2de3a8a8e4b40e69fc8642c7d7cb68.

Build Python::

    ./configure --with-lto && make
    ./python -m venv env && env/bin/python -m pip install pyperf

Results.

+-----------+---------+-----------------------------+
| Benchmark | ref     | tagptr                      |
+===========+=========+=============================+
| telco     | 8.18 ms | 8.77 ms: 1.07x slower (+7%) |
+-----------+---------+-----------------------------+
| go        | 346 ms  | 398 ms: 1.15x slower (+15%) |
+-----------+---------+-----------------------------+
| deltablue | 10.9 ms | 11.8 ms: 1.08x slower (+8%) |
+-----------+---------+-----------------------------+


Main changes
============

* Py_IS_NONE(x): similar to "x == Py_None"
* Py_IS_TRUE(x): similar to "x == Py_True"
* Py_IS_FALSE(x): similar to "x == Py_False"

Use tagged pointers
===================

* Py_RETURN_NONE
* Py_RETURN_TRUE
* Py_RETURN_FALSE
* get_small_int()

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

* Float as tagged pointer
* Optimize int + int in ceval?
* Fix test_gdb: implemented tagged pointer in python-gdb.py.
* Fix Modules/_elementtree.c::

    /* macros used to store 'join' flags in string object pointers.  note
       that all use of text and tail as object pointers must be wrapped in
       JOIN_OBJ.  see comments in the ElementObject definition for more
       info. */
    #define JOIN_GET(p) ((uintptr_t) (p) & 1)
    #define JOIN_SET(p, flag) ((void*) ((uintptr_t) (JOIN_OBJ(p)) | (flag)))
    #define JOIN_OBJ(p) ((PyObject*) ((uintptr_t) (p) & ~(uintptr_t)1))



Previous attempt
================

Neil Schemenauer PoC (Sept 2018):

* https://mail.python.org/archives/list/capi-sig@python.org/thread/EGAY55ZWMF2WSEMP7VAZSFZCZ4VARU7L/#EGAY55ZWMF2WSEMP7VAZSFZCZ4VARU7L
* https://github.com/nascheme/cpython/commits/tagged_int

PyPy "Integers as Tagged Pointers" featur "Integers as Tagged Pointers" feature:
https://doc.pypy.org/en/latest/interpreter-optimizations.html#integers-as-tagged-pointers
