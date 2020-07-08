CPython fork reference: commit b26a0db8ea2de3a8a8e4b40e69fc8642c7d7cb68


Memory Usage
============

def listsize(l):
    return sys.getsizeof(l) + sum(sys.getsizeof(item) for item in l)


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

Fix test_gdb: implemented tagged pointer in python-gdb.py.
