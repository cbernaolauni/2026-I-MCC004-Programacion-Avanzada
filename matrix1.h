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
        //Matrix1(Matrix1 &other) = delete; // No copy constructor
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
        T*       operator[](size_t row);
        const T* operator[](size_t row) const;
        Matrix1<T> Transpose() const;
        Matrix1<T> RowEchelon(bool reduced = false) const;
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
Matrix1<T> operator+(Matrix1<T> lhs, const Matrix1<T> &rhs) {
    lhs += rhs;
    return move(lhs);
}

template <typename T>
Matrix1<T> operator-(Matrix1<T> lhs, const Matrix1<T> &rhs) {
    lhs -= rhs;
    return move(lhs);
}

template <typename T>
Matrix1<T> operator*(Matrix1<T> lhs, const Matrix1<T> &rhs) {
    lhs *= rhs;
    return move(lhs);
}

// escalar * matriz
template <typename T>
Matrix1<T> operator*(const T &scalar, Matrix1<T> lhs) {
    lhs *= scalar;
    return move(lhs);
}

template <typename T>
Matrix1<T> operator*(Matrix1<T> lhs, const T &scalar) {
    lhs *= scalar;
    return move(lhs);
}

template <typename T>
T* Matrix1<T>::operator[](size_t row) {
    cout << "Accediendo a fila " << row << endl;
    cout << "Fila " << m_rows << endl;
    assert(row <= m_rows);
    return m_pMat[row];
}

template <typename T>
const T* Matrix1<T>::operator[](size_t row) const {
    assert(row <= m_rows);
    return m_pMat[row];
}

template <typename T>
Matrix1<T> Matrix1<T>::Transpose() const {
    Matrix1<T> result;
    result.m_rows = m_cols;
    result.m_cols = m_rows;
    result.Create();
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            result.m_pMat[j][i] = m_pMat[i][j];
    return result;
}

template <typename T>
Matrix1<T> Matrix1<T>::RowEchelon(bool reduced) const {
    Matrix1<T> result(*this);
    size_t pivot_row = 0;

    for (size_t col = 0; col < result.m_cols && pivot_row < result.m_rows; ++col) {
        size_t found = result.m_rows; // centinela
        for (size_t row = pivot_row; row < result.m_rows; ++row) {
            if (result.m_pMat[row][col] != T{}) { found = row; break; }
        }
        if (found == result.m_rows) continue;

        // Intercambiar fila encontrada con la fila pivote
        if (found != pivot_row)
            swap(result.m_pMat[found], result.m_pMat[pivot_row]);

        T pivot = result.m_pMat[pivot_row][col];

        // Normalizar fila pivote (pivote = 1)
        for (size_t j = 0; j < result.m_cols; ++j)
            result.m_pMat[pivot_row][j] /= pivot;

        size_t start = reduced ? 0 : pivot_row + 1;
        for (size_t row = start; row < result.m_rows; ++row) {
            if (row == pivot_row) continue;
            T factor = result.m_pMat[row][col];
            for (size_t j = 0; j < result.m_cols; ++j)
                result.m_pMat[row][j] -= factor * result.m_pMat[pivot_row][j];
        }
        ++pivot_row;
    }
    return result;
}

#endif // __MATRIX_H__