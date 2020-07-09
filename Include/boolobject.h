/* Boolean object interface */

#ifndef Py_BOOLOBJECT_H
#define Py_BOOLOBJECT_H
#ifdef __cplusplus
extern "C" {
#endif


PyAPI_DATA(PyTypeObject) PyBool_Type;

#define PyBool_Check(x) Py_IS_TYPE(x, &PyBool_Type)

/* Py_False and Py_True are the only two bools in existence.
Don't forget to apply Py_INCREF() when returning either!!! */

/* Don't use these directly */
PyAPI_DATA(struct _longobject) _Py_FalseStruct, _Py_TrueStruct;

/* Function to return a bool from a C long */
PyAPI_FUNC(PyObject *) PyBool_FromLong(long);

#define _Py_TAGPTR_TRUE ((PyObject*)(uintptr_t)_Py_TAGPTR_TAG_TRUE)
#define _Py_TAGPTR_FALSE ((PyObject*)(uintptr_t)_Py_TAGPTR_TAG_FALSE)

/* Use these macros */
#define Py_False _Py_TAGPTR_FALSE
#define Py_True _Py_TAGPTR_TRUE

static inline int Py_IS_TRUE(PyObject *op) {
    return (op == Py_True || op == _Py_TAGPTR_TRUE);
}

static inline int Py_IS_FALSE(PyObject *op) {
    return (op == Py_False || op == _Py_TAGPTR_FALSE);
}

/* Macros for returning Py_True or Py_False, respectively */
#define Py_RETURN_TRUE return _Py_TAGPTR_TRUE
#define Py_RETURN_FALSE return _Py_TAGPTR_FALSE

#ifdef __cplusplus
}
#endif
#endif /* !Py_BOOLOBJECT_H */
