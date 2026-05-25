#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <functional>
#include <iostream>
#include <cassert>

using namespace std;

template <typename T>
void Print44(T &n, ostream &os) { os << n << " "; }

template <typename T>
class Matrix1 {
    private:
        T      **m_pMat = nullptr;
        size_t   m_rows = 0, m_cols = 0;
    public:
        Matrix1()      { }
        ~Matrix1()     { Destroy(); }
        void     Create();
        istream &Read(istream &is);
        template <typename Func, typename... Args>
        void ApplyFunctionToAll(Func func, Args&& ...args);
        ostream &Print(ostream &os);
        void Destroy();
};

template <typename T>
void Matrix1<T>::Create()
{   assert(m_rows > 0 && m_cols > 0);
    m_pMat = new T *[m_rows];
    for(size_t i = 0 ; i < m_rows ; ++i)
        m_pMat[i] = new T[m_cols];
}


template <typename T>
istream &Matrix1<T>::Read(istream &is) {
    Destroy();
    is >> m_rows >> m_cols;
    Create();
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            is >> m_pMat[i][j];
    return is;
}

template <typename T>
void Matrix1<T>::Destroy() {
    if (m_pMat != nullptr) {
        for(size_t i = 0 ; i < m_rows ; ++i)
            delete[] m_pMat[i];
        delete[] m_pMat;
        m_pMat = nullptr;
        m_rows = m_cols = 0;
    }
}

template <typename T>
template <typename Func, typename... Args>
void Matrix1<T>::ApplyFunctionToAll(Func func, Args&& ...args) {
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            func(m_pMat[i][j], forward<Args>(args)...);
}   

template <typename T>
ostream &Matrix1<T>::Print(ostream &os) {
    os << "filas:" << m_rows << " columnas:" << m_cols << "\n";
    for (size_t i = 0; i < m_rows; ++i) {
        os << "[ ";
        for (size_t j = 0; j < m_cols; ++j)
            os << m_pMat[i][j] << " ";
        os << "]\n";
    }
    return os;
}

template <typename T>
istream &operator>>(istream &is, Matrix1<T> &matrix){
    return matrix.Read(is);
}

template <typename T>
ostream &operator<<(ostream &os, Matrix1<T> &matrix){\
    matrix.Print(os);
    return os;
}

#endif // __MATRIX_H__