#define PY_SSIZE_T_CLEAN
#include <pybind11/pybind11.h>
#include <fstream>
#include <sstream>
#include "matrix1.h"
#include "types.h"

namespace py = pybind11;

PYBIND11_MODULE(matrix1_api_c, m) {

    py::class_<Matrix1<MatrizType>>(m, "Matrix1")

        // Constructores
        .def(py::init<>())
        .def(py::init<size_t, size_t>())
        .def(py::init([](const std::string &filename) {
            auto *mat = new Matrix1<MatrizType>();
            std::ifstream ifs(filename);
            if (!ifs) throw std::runtime_error("No se pudo abrir: " + filename);
            ifs >> *mat;
            return mat;
        }))

        // Punteros a miembro — patrón del profe
        .def("transpose",   &Matrix1<MatrizType>::Transpose)
        .def("row_echelon", &Matrix1<MatrizType>::RowEchelon, py::arg("reduced") = false)

        // Operadores via punteros a miembro
        .def("__add__", [](Matrix1<MatrizType> a, const Matrix1<MatrizType> &b) {
            void (Matrix1<MatrizType>::*pAddEq)(const Matrix1<MatrizType>&) = &Matrix1<MatrizType>::operator+=;
            (a.*pAddEq)(b);
            return a;
        })
        .def("__sub__", [](Matrix1<MatrizType> a, const Matrix1<MatrizType> &b) {
            void (Matrix1<MatrizType>::*pSubEq)(const Matrix1<MatrizType>&) = &Matrix1<MatrizType>::operator-=;
            (a.*pSubEq)(b);
            return a;
        })
        .def("__mul__", [](Matrix1<MatrizType> a, const Matrix1<MatrizType> &b) {
            void (Matrix1<MatrizType>::*pMulEq)(const Matrix1<MatrizType>&) = &Matrix1<MatrizType>::operator*=;
            (a.*pMulEq)(b);
            return a;
        })
        .def("mul_scalar", [](Matrix1<MatrizType> a, MatrizType s) {
            void (Matrix1<MatrizType>::*pMulScalar)(const MatrizType&) = &Matrix1<MatrizType>::operator*=;
            (a.*pMulScalar)(s);
            return a;
        })

        // Acceso m[i, j]
        .def("__getitem__", [](Matrix1<MatrizType> &mat, std::pair<size_t,size_t> idx) {
            return mat[idx.first][idx.second];
        })
        .def("__setitem__", [](Matrix1<MatrizType> &mat, std::pair<size_t,size_t> idx, MatrizType val) {
            mat[idx.first][idx.second] = val;
        })

        .def("__repr__", [](Matrix1<MatrizType> &mat) {
            std::ostringstream oss;
            mat.Print(oss);
            return oss.str();
        });
}