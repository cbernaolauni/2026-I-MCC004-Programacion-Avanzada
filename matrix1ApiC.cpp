#define PY_SSIZE_T_CLEAN
#include <pybind11/pybind11.h>
#include <fstream>
#include <sstream>
#include "matrix1.h"
#include "types.h"

namespace py = pybind11;

// Template para no repetir el binding para cada tipo
template <typename T>
void bind_matrix(py::module &m, const char *name) {

    py::class_<Matrix1<T>>(m, name)
        .def(py::init<>())
        .def(py::init<size_t, size_t>())
        .def(py::init([](const std::string &filename) {
            auto *mat = new Matrix1<T>();
            std::ifstream ifs(filename);
            if (!ifs) throw std::runtime_error("No se pudo abrir: " + filename);
            ifs >> *mat;
            return mat;
        }))
        .def("__add__", [](Matrix1<T> a, const Matrix1<T> &b) {
            void (Matrix1<T>::*pAddEq)(const Matrix1<T>&) = &Matrix1<T>::operator+=;
            (a.*pAddEq)(b); return a;
        })
        .def("__sub__", [](Matrix1<T> a, const Matrix1<T> &b) {
            void (Matrix1<T>::*pSubEq)(const Matrix1<T>&) = &Matrix1<T>::operator-=;
            (a.*pSubEq)(b); return a;
        })
        .def("__mul__", [](Matrix1<T> a, const Matrix1<T> &b) {
            void (Matrix1<T>::*pMulEq)(const Matrix1<T>&) = &Matrix1<T>::operator*=;
            (a.*pMulEq)(b); return a;
        })
        .def("__mul__", [](Matrix1<T> a, T s) {          // matriz * escalar
            void (Matrix1<T>::*pMulScalar)(const T&) = &Matrix1<T>::operator*=;
            (a.*pMulScalar)(s); return a;
        })
        .def("__rmul__", [](Matrix1<T> a, T s) {         // escalar * matriz
            void (Matrix1<T>::*pMulScalar)(const T&) = &Matrix1<T>::operator*=;
            (a.*pMulScalar)(s); return a;
        })
        .def("mul_scalar", [](Matrix1<T> a, T s) {
            void (Matrix1<T>::*pMulScalar)(const T&) = &Matrix1<T>::operator*=;
            (a.*pMulScalar)(s); return a;
        })

        .def("__getitem__", [](Matrix1<T> &matrix, std::pair<size_t,size_t> idx) {
            return matrix[idx.first][idx.second];
        })
        .def("__setitem__", [](Matrix1<T> &matrix, std::pair<size_t,size_t> idx, T val) {
            matrix[idx.first][idx.second] = val;
        })

        .def("__repr__", [](Matrix1<T> &matrix) {
            std::ostringstream oss;
            matrix.Print(oss);
            return oss.str();
        })

        .def("transpose",   &Matrix1<T>::Transpose)
        .def("row_echelon", &Matrix1<T>::RowEchelon, py::arg("reduced") = false)
        .def("det", [](Matrix1<T> &matrix) {
            T (Matrix1<T>::*pDet)() const = &Matrix1<T>::Determinant;
            return (matrix.*pDet)();
        });
}

PYBIND11_MODULE(matrix1_api_c, m) {
    bind_matrix<MatrizInt>   (m, "Matrix_I");
    bind_matrix<MatrizFloat> (m, "Matrix_F");
    bind_matrix<MatrizDouble>(m, "Matrix_D");
}