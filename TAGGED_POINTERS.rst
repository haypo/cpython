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
