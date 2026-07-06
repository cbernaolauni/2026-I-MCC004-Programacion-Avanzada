#define PY_SSIZE_T_CLEAN
#include <fstream>
#include <sstream>
#include <Python.h>
#include "matrix1.h"
#include "types.h"

// Wrapper struct
typedef struct {
    PyObject_HEAD
    Matrix1<MatrizType> *mat;
} Matrix1Object;

// Punteros a miembro
static Matrix1<MatrizType> (Matrix1<MatrizType>::*pTranspose)() const  = &Matrix1<MatrizType>::Transpose;
static Matrix1<MatrizType> (Matrix1<MatrizType>::*pREF)(bool) const    = &Matrix1<MatrizType>::RowEchelon;
static void (Matrix1<MatrizType>::*pAddEq)(const Matrix1<MatrizType>&) = &Matrix1<MatrizType>::operator+=;
static void (Matrix1<MatrizType>::*pSubEq)(const Matrix1<MatrizType>&) = &Matrix1<MatrizType>::operator-=;
static void (Matrix1<MatrizType>::*pMulEq)(const Matrix1<MatrizType>&) = &Matrix1<MatrizType>::operator*=;

// new / dealloc
static PyObject *Matrix1_new(PyTypeObject *type, PyObject *args, PyObject *kw) {
    Matrix1Object *self = (Matrix1Object *)type->tp_alloc(type, 0);
    if (self) self->mat = nullptr;
    return (PyObject *)self;
}

static void Matrix1_dealloc(Matrix1Object *self) {
    delete self->mat;
    Py_TYPE(self)->tp_free((PyObject *)self);
}

// init
static MatrizT Matrix1_init(Matrix1Object *self, PyObject *args, PyObject *kw) {
    delete self->mat;
    self->mat = new Matrix1<MatrizType>();

    // Intentar con string (nombre de archivo)
    const char *filename = nullptr;
    if (PyArg_ParseTuple(args, "s", &filename)) {
        std::ifstream ifs(filename);
        if (!ifs) { PyErr_SetString(PyExc_FileNotFoundError, filename); return -1; }
        ifs >> *self->mat;
        return 0;
    }

    // Intentar con rows, cols
    PyErr_Clear();
    size_t rows = 0, cols = 0;
    if (PyArg_ParseTuple(args, "|nn", &rows, &cols)) {
        if (rows > 0 && cols > 0) {
            *self->mat = Matrix1<MatrizType>(rows, cols);
        }
        return 0;
    }
    return -1;
}

// repr
static PyObject *Matrix1_repr(Matrix1Object *self) {
    std::ostringstream oss;
    self->mat->Print(oss);
    return PyUnicode_FromString(oss.str().c_str());
}

// getitem__ / __setitem__─
static PyObject *Matrix1_getitem(Matrix1Object *self, PyObject *key) {
    size_t i, j;
    if (!PyArg_ParseTuple(key, "nn", &i, &j)) return nullptr;
    return PyFloat_FromDouble((*self->mat)[i][j]);
}

static MatrizT Matrix1_setitem(Matrix1Object *self, PyObject *key, PyObject *val) {
    size_t i, j;
    if (!PyArg_ParseTuple(key, "nn", &i, &j)) return -1;
    (*self->mat)[i][j] = (MatrizType)PyFloat_AsDouble(val);
    return 0;
}

// Métodos usando punteros a miembro
static PyObject *Matrix1_transpose(Matrix1Object *self, PyObject *) {
    Matrix1Object *result = PyObject_New(Matrix1Object, Py_TYPE(self));
    result->mat = new Matrix1<MatrizType>((self->mat->*pTranspose)());
    return (PyObject *)result;
}

static PyObject *Matrix1_row_echelon(Matrix1Object *self, PyObject *args) {
    MatrizT reduced = 0;
    if (!PyArg_ParseTuple(args, "|p", &reduced)) return nullptr;
    Matrix1Object *result = PyObject_New(Matrix1Object, Py_TYPE(self));
    result->mat = new Matrix1<MatrizType>((self->mat->*pREF)(reduced));
    return (PyObject *)result;
}

static PyObject *Matrix1_add(Matrix1Object *self, PyObject *other) {
    Matrix1Object *o = (Matrix1Object *)other;
    Matrix1Object *result = PyObject_New(Matrix1Object, Py_TYPE(self));
    result->mat = new Matrix1<MatrizType>(*self->mat);
    (result->mat->*pAddEq)(*o->mat);
    return (PyObject *)result;
}

static PyObject *Matrix1_sub(PyObject *self, PyObject *other) {
    Matrix1Object *a = (Matrix1Object *)self;
    Matrix1Object *b = (Matrix1Object *)other;
    Matrix1Object *result = PyObject_New(Matrix1Object, Py_TYPE(a));
    result->mat = new Matrix1<MatrizType>(*a->mat);
    (result->mat->*pSubEq)(*b->mat);
    return (PyObject *)result;
}

static PyObject *Matrix1_mul(PyObject *self, PyObject *other) {
    Matrix1Object *a = (Matrix1Object *)self;
    Matrix1Object *b = (Matrix1Object *)other;
    Matrix1Object *result = PyObject_New(Matrix1Object, Py_TYPE(a));
    result->mat = new Matrix1<MatrizType>(*a->mat);
    (result->mat->*pMulEq)(*b->mat);
    return (PyObject *)result;
}

static PyObject *Matrix1_mul_scalar(Matrix1Object *self, PyObject *args) {
    MatrizType scalar;
    if (!PyArg_ParseTuple(args, "d", &scalar)) return nullptr;
    Matrix1Object *result = PyObject_New(Matrix1Object, Py_TYPE(self));
    result->mat = new Matrix1<MatrizType>(*self->mat);
    *result->mat *= scalar;
    return (PyObject *)result;
}

// Tabla de métodos
static PyMethodDef Matrix1_methods[] = {
    { "transpose",   (PyCFunction)Matrix1_transpose,   METH_NOARGS,  "Transpuesta"     },
    { "row_echelon", (PyCFunction)Matrix1_row_echelon, METH_VARARGS, "REF o RREF"      },
    { "mul_scalar",  (PyCFunction)Matrix1_mul_scalar,  METH_VARARGS, "Escalar x matriz"},
    { nullptr }
};

// Mapping (m[i,j])
static PyMappingMethods Matrix1_mapping = {
    nullptr,
    (binaryfunc)Matrix1_getitem,
    (objobjargproc)Matrix1_setitem,
};

// Número (__add__)
static PyNumberMethods Matrix1_number = {
    (binaryfunc)Matrix1_add,
    (binaryfunc)Matrix1_sub,
    (binaryfunc)Matrix1_mul,
};

// PyTypeObject
static PyTypeObject Matrix1Type = {
    PyVarObject_HEAD_INIT(nullptr, 0)
    .tp_name      = "matrix1_capi.Matrix1",
    .tp_basicsize = sizeof(Matrix1Object),
    .tp_dealloc   = (destructor)Matrix1_dealloc,
    .tp_repr      = (reprfunc)Matrix1_repr,
    .tp_as_number = &Matrix1_number,
    .tp_as_mapping= &Matrix1_mapping,
    .tp_methods   = Matrix1_methods,
    .tp_init      = (initproc)Matrix1_init,
    .tp_new       = Matrix1_new,
};

// Módulo
static PyModuleDef matrix1_module = {
    PyModuleDef_HEAD_INIT, "matrix1_api_c", nullptr, -1, nullptr
};

PyMODINIT_FUNC PyInit_matrix1_api_c() {
    if (PyType_Ready(&Matrix1Type) < 0) return nullptr;
    PyObject *m = PyModule_Create(&matrix1_module);
    Py_INCREF(&Matrix1Type);
    PyModule_AddObject(m, "Matrix1", (PyObject *)&Matrix1Type);
    return m;
}