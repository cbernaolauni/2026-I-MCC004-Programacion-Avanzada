#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <functional>
#include <iostream>
#include <cassert>
#include <utility> 

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
        Matrix1(Matrix1 &other) = delete; // No copy constructor
        Matrix1(const Matrix1 &other);
        Matrix1(Matrix1 &&other); 
        ~Matrix1()     { Destroy(); }
        void     Create();
        istream &Read(istream &is);
        template <typename Func, typename... Args>
        void ApplyFunctionToAll(Func func, Args&& ...args);
        ostream &Print(ostream &os);
        void operator+=(const Matrix1 &other);
        void operator-=(const Matrix1 &other);
        void operator*=(const Matrix1 &other);
        void operator*=(const T &scalar);
        Matrix1<T>& operator=(Matrix1<T> &&other);
        void Destroy();
};

template <typename T>
Matrix1<T>::Matrix1(const Matrix1<T> &other) 
    : m_rows(other.m_rows), m_cols(other.m_cols) {
    if (m_rows > 0 && m_cols > 0) {
        Create();
        for (size_t i = 0; i < m_rows; ++i)
            for (size_t j = 0; j < m_cols; ++j)
                m_pMat[i][j] = other.m_pMat[i][j];
    }
}

template <typename T>
void Matrix1<T>::Create()
{   assert(m_rows > 0 && m_cols > 0);
    m_pMat = new T *[m_rows];
    for(size_t i = 0 ; i < m_rows ; ++i)
        m_pMat[i] = new T[m_cols];
}

// Move constructor
template <typename T>
Matrix1<T>::Matrix1(Matrix1 &&other) {
    m_pMat = exchange(other.m_pMat, nullptr);
    m_rows = exchange(other.m_rows, 0);
    m_cols = exchange(other.m_cols, 0);
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

template <typename T>
void Matrix1<T>::operator+=(const Matrix1<T> &other) {
    assert(m_rows == other.m_rows && m_cols == other.m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] += other.m_pMat[i][j];
}

template <typename T>
void Matrix1<T>::operator-=(const Matrix1<T> &other) {
    assert(m_rows == other.m_rows && m_cols == other.m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] -= other.m_pMat[i][j];
}

template <typename T>
void Matrix1<T>::operator*=(const Matrix1<T> &other) {
    assert(m_cols == other.m_rows);
    size_t new_cols = other.m_cols;

    T **result = new T*[m_rows];
    for (size_t i = 0; i < m_rows; ++i) {
        result[i] = new T[new_cols]{};
        for (size_t j = 0; j < new_cols; ++j)
            for (size_t k = 0; k < m_cols; ++k)
                result[i][j] += m_pMat[i][k] * other.m_pMat[k][j];
    }
    
    for (size_t i = 0; i < m_rows; ++i)
        delete[] m_pMat[i];
    delete[] m_pMat;

    m_pMat = result;
    m_cols = new_cols;
}

template <typename T>
void Matrix1<T>::operator*=(const T &scalar) {
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] *= scalar;
}

template <typename T>
Matrix1<T>& Matrix1<T>::operator=(Matrix1<T> &&other) {
    if (this != &other) {
        Destroy();
        m_pMat = exchange(other.m_pMat, nullptr);
        m_rows = exchange(other.m_rows, 0);
        m_cols = exchange(other.m_cols, 0);
    }
    return *this;
}

template <typename T>
Matrix1<T> operator+(Matrix1<T> &&lhs, const Matrix1<T> &rhs) {
    lhs += rhs;
    return move(lhs);
}

template <typename T>
Matrix1<T> operator-(Matrix1<T> &&lhs, const Matrix1<T> &rhs) {
    lhs -= rhs;
    return move(lhs);
}

template <typename T>
Matrix1<T> operator*(Matrix1<T> &&lhs, const Matrix1<T> &rhs) {
    lhs *= rhs;
    return move(lhs);
}

// escalar * matriz
template <typename T>
Matrix1<T> operator*(const T &scalar, Matrix1<T> &&rhs) {
    rhs *= scalar;
    return move(rhs);
}

#endif // __MATRIX_H__