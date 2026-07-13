/**
 * @file matrix1.h
 * @brief Librería de matrices genéricas con soporte para operaciones algebraicas,
 *        eliminación gaussiana paralela con std::thread, y bindings Python via pybind11.
 * @author Cesar
 * @date 2026
 * @course MCC004 - Programación Avanzada
 */
#ifndef __MATRIX_H__
#define __MATRIX_H__
#include <functional>
#include <iostream>
#include <cassert>
#include <utility> 
#include <thread>
#include <vector>

using namespace std;

/**
 * @brief Función auxiliar para imprimir un elemento en un stream.
 * @tparam T Tipo del elemento.
 * @param n Elemento a imprimir.
 * @param os Stream de salida.
 */
template <typename T>
void Print44(T &n, ostream &os) { os << n << " "; }

/**
 * @brief Clase template que representa una matriz 2D dinámica.
 *
 * Implementa operaciones algebraicas básicas (suma, resta, multiplicación),
 * eliminación gaussiana (REF y RREF) con paralelismo via std::thread,
 * cálculo de determinante, transposición, y acceso por índice.
 *
 * @tparam T Tipo numérico de los elementos (int, float, double).
 *
 * @par Ejemplo de uso:
 * @code
 * Matrix1<double> m(3, 3);
 * m[0][0] = 1.0; m[1][1] = 5.0; m[2][2] = 9.0;
 * Matrix1<double> t = m.Transpose();
 * double det = m.Determinant();
 * @endcode
 */
template <typename T>
class Matrix1 {
    private:
        T      **m_pMat = nullptr; ///< Puntero a la matriz de datos.
        size_t   m_rows = 0;        ///< Número de filas.
        size_t   m_cols = 0;        ///< Número de columnas.

        /**
         * @brief Núcleo compartido de eliminación gaussiana con threads.
         *
         * Realiza eliminación hacia adelante (REF) o completa (RREF).
         * Cada fila de eliminación se procesa en un std::thread independiente.
         *
         * @param matrix Matriz de trabajo (modificada in-place).
         * @param sign   Signo acumulado por intercambios de filas (para determinante).
         * @param reduced true para RREF, false para REF (default).
         * @return Vector de pivotes encontrados en cada columna.
         */
        std::vector<T> GaussElim(Matrix1<T> &matrix, size_t &sign, bool reduced = false) const;
    public:
        /**
         * @brief Constructor por defecto. Crea una matriz vacía.
         */
        Matrix1()      { }
        //Matrix1(Matrix1 &other) = delete; // No copy constructor
        
        /**
         * @brief Constructor de copia.
         * @param other Matriz a copiar.
         */
        Matrix1(const Matrix1 &other);
        /**
         * @brief Constructor con dimensiones. Elementos inicializados a cero.
         * @param rows Número de filas.
         * @param cols Número de columnas.
         */
        Matrix1(size_t rows, size_t cols);
 
        /**
         * @brief Constructor de movimiento.
         * @param other Matriz fuente (queda vacía después del move).
         */
        Matrix1(Matrix1 &&other);
 
        /**
         * @brief Destructor. Libera toda la memoria dinámica.
         */
        ~Matrix1() { Destroy(); }
 
        /**
         * @brief Reserva memoria para la matriz.
         * @pre m_rows > 0 && m_cols > 0
         * @note Inicializa todos los elementos a cero con new T[m_cols]{}.
         */
        void Create();
 
        /**
         * @brief Lee la matriz desde un stream.
         *
         * Formato esperado:
         * @code
         * rows cols
         * a00 a01 ... a0n
         * a10 a11 ... a1n
         * @endcode
         *
         * @param is Stream de entrada.
         * @return Referencia al stream de entrada.
         */
        istream &Read(istream &is);
 
        /**
         * @brief Aplica una función a todos los elementos de la matriz.
         *
         * @tparam Func Tipo de la función o functor.
         * @tparam Args Tipos de los argumentos adicionales.
         * @param func Función a aplicar. Firma: void func(T&, Args...).
         * @param args Argumentos adicionales forwarded a func.
         *
         * @par Ejemplo:
         * @code
         * matrix.ApplyFunctionToAll(Square<double>);
         * matrix.ApplyFunctionToAll(AddX<double>, 5.0, 10.0);
         * @endcode
         */
        template <typename Func, typename... Args>
        void ApplyFunctionToAll(Func func, Args&& ...args);
 
        /**
         * @brief Imprime la matriz en un stream.
         *
         * Formato de salida:
         * @code
         * filas:N columnas:M
         * [ a00 a01 ... ]
         * [ a10 a11 ... ]
         * @endcode
         *
         * @param os Stream de salida.
         * @return Referencia al stream de salida.
         */
        ostream &Print(ostream &os);
 
        /**
         * @brief Suma en lugar: this += other.
         * @param other Matriz a sumar. Debe tener las mismas dimensiones.
         */
        void operator+=(const Matrix1 &other);
 
        /**
         * @brief Resta en lugar: this -= other.
         * @param other Matriz a restar. Debe tener las mismas dimensiones.
         */
        void operator-=(const Matrix1 &other);
 
        /**
         * @brief Multiplicación matricial en lugar: this *= other.
         * @param other Matriz multiplicadora. m_cols debe ser == other.m_rows.
         */
        void operator*=(const Matrix1 &other);
 
        /**
         * @brief Multiplicación por escalar en lugar: this *= scalar.
         * @param scalar Escalar multiplicador.
         */
        void operator*=(const T &scalar);
 
        /**
         * @brief Asignación por movimiento.
         * @param other Matriz fuente (queda vacía después del move).
         * @return Referencia a this.
         */
        Matrix1<T>& operator=(Matrix1<T> &&other);
 
        /**
         * @brief Acceso por fila (versión mutable).
         *
         * Permite sintaxis m[i][j] = valor.
         *
         * @param row Índice de fila.
         * @return Puntero al inicio de la fila row.
         */
        T* operator[](size_t row);
 
        /**
         * @brief Acceso por fila (versión const).
         * @param row Índice de fila.
         * @return Puntero const al inicio de la fila row.
         */
        const T* operator[](size_t row) const;
 
        /**
         * @brief Calcula la transpuesta de la matriz.
         * @return Nueva matriz transpuesta (m_cols x m_rows).
         */
        Matrix1<T> Transpose() const;
 
        /**
         * @brief Forma escalonada (REF o RREF) via eliminación gaussiana paralela.
         *
         * Usa GaussElim internamente con std::thread para paralelizar
         * la eliminación de cada columna.
         *
         * @param reduced false = REF (default), true = RREF.
         * @return Nueva matriz en forma escalonada.
         *
         * @par Ejemplo:
         * @code
         * Matrix1<double> ref  = m.RowEchelon();       // REF
         * Matrix1<double> rref = m.RowEchelon(true);   // RREF
         * @endcode
         */
        Matrix1<T> RowEchelon(bool reduced = false) const;
 
        /**
         * @brief Calcula el determinante via eliminación gaussiana paralela.
         *
         * Reutiliza GaussElim y acumula el producto de los pivotes,
         * ajustado por el signo de los intercambios de filas.
         *
         * @pre La matriz debe ser cuadrada (m_rows == m_cols).
         * @return Determinante de tipo T.
         * @note Para matrices enteras puede haber truncamiento. Usar Matrix1<double>.
         */
        T Determinant() const;
 
        /**
         * @brief Libera toda la memoria dinámica y resetea dimensiones a 0.
         */
        void Destroy();
};

/**
 * @brief Constructor de la matriz.
 * @tparam T Tipo numérico.
 * @param rows Número de filas.
 * @param cols Número de columnas.
 */
template <typename T>
Matrix1<T>::Matrix1(size_t rows, size_t cols)
    : m_rows(rows), m_cols(cols) {
    if (m_rows > 0 && m_cols > 0)
        Create();
}

/**
 * @brief Constructor de copia.
 * @tparam T Tipo numérico.
 * @param other Matriz a copiar.
 */
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

/**
 * @brief Crea la estructura interna de la matriz.
 * @tparam T Tipo numérico.
 */
template <typename T>
void Matrix1<T>::Create()
{   assert(m_rows > 0 && m_cols > 0);
    m_pMat = new T *[m_rows];
    for(size_t i = 0 ; i < m_rows ; ++i)
        m_pMat[i] = new T[m_cols]{};
}

/**
 * @brief Constructor de movimiento.
 * @tparam T Tipo numérico.
 * @param other Matriz a mover.
 */
// Move constructor
template <typename T>
Matrix1<T>::Matrix1(Matrix1 &&other) {
    m_pMat = exchange(other.m_pMat, nullptr);
    m_rows = exchange(other.m_rows, 0);
    m_cols = exchange(other.m_cols, 0);
}

/**
 * @brief Lee una matriz desde un stream.
 * @tparam T Tipo numérico.
 * @param is Stream de entrada.
 * @return Referencia al stream de entrada.
 */
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

/**
 * @brief Libera toda la memoria dinámica y resetea dimensiones a 0.
 * @tparam T Tipo numérico.
 */
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

/**
 * @brief Aplica una función a todos los elementos de la matriz.
 * @tparam T Tipo numérico.
 * @tparam Func Tipo de la función a aplicar.
 * @tparam Args Tipos de los argumentos de la función.
 * @param func Función a aplicar.
 * @param args Argumentos para la función.
 */
template <typename T>
template <typename Func, typename... Args>
void Matrix1<T>::ApplyFunctionToAll(Func func, Args&& ...args) {
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            func(m_pMat[i][j], forward<Args>(args)...);
}   

/** @brief Imprime la matriz en un stream.
 * @tparam T Tipo numérico.
 * @param os Stream de salida.
 * @return Referencia al stream de salida.
 */
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

/**
 * @brief Operador de entrada para leer una matriz desde stream.
 * @tparam T Tipo numérico.
 * @param is Stream de entrada.
 * @param matrix Matriz destino.
 * @return Referencia al stream de entrada.
 */
template <typename T>
istream &operator>>(istream &is, Matrix1<T> &matrix){
    return matrix.Read(is);
}

/**
 * @brief Operador de salida para imprimir una matriz en stream.
 * @tparam T Tipo numérico.
 * @param os Stream de salida.
 * @param matrix Matriz a imprimir.
 * @return Referencia al stream de salida.
 */
template <typename T>
ostream &operator<<(ostream &os, Matrix1<T> &matrix){\
    matrix.Print(os);
    return os;
}

/** @brief Operador de suma en lugar de asignación.
 * @tparam T Tipo numérico.
 * @param other Matriz a sumar.
 */
template <typename T>
void Matrix1<T>::operator+=(const Matrix1<T> &other) {
    assert(m_rows == other.m_rows && m_cols == other.m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] += other.m_pMat[i][j];
}

/** @brief Operador de resta en lugar de asignación.
 * @tparam T Tipo numérico.
 * @param other Matriz a restar.
 */
template <typename T>
void Matrix1<T>::operator-=(const Matrix1<T> &other) {
    assert(m_rows == other.m_rows && m_cols == other.m_cols);
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] -= other.m_pMat[i][j];
}

/** @brief Operador de multiplicación en lugar de asignación.
 * @tparam T Tipo numérico.
 * @param other Matriz a multiplicar.
 */
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

/** @brief Operador de multiplicación escalar en lugar de asignación.
 * @tparam T Tipo numérico.
 * @param scalar Escalar multiplicador.
 */
template <typename T>
void Matrix1<T>::operator*=(const T &scalar) {
    for (size_t i = 0; i < m_rows; ++i)
        for (size_t j = 0; j < m_cols; ++j)
            m_pMat[i][j] *= scalar;
}

/**
 * @brief Operador de asignación por movimiento.
 * @tparam T Tipo numérico.
 * @param other Matriz a mover.
 * @return Referencia a la matriz actual.
 */
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

/**
 * @brief Suma de dos matrices. Retorna nueva matriz.
 * @tparam T Tipo numérico.
 * @param lhs Operando izquierdo (copia).
 * @param rhs Operando derecho.
 * @return Nueva matriz resultado.
 */
template <typename T>
Matrix1<T> operator+(Matrix1<T> lhs, const Matrix1<T> &rhs) {
    lhs += rhs;
    return move(lhs);
}

/**
 * @brief Resta de dos matrices. Retorna nueva matriz.
 * @tparam T Tipo numérico.
 * @param lhs Operando izquierdo (copia).
 * @param rhs Operando derecho.
 * @return Nueva matriz resultado.
 */
template <typename T>
Matrix1<T> operator-(Matrix1<T> lhs, const Matrix1<T> &rhs) {
    lhs -= rhs;
    return move(lhs);
}

/**
 * @brief Multiplicación de dos matrices. Retorna nueva matriz.
 * @tparam T Tipo numérico.
 * @param lhs Operando izquierdo (copia).
 * @param rhs Operando derecho.
 * @return Nueva matriz resultado.
 */
template <typename T>
Matrix1<T> operator*(Matrix1<T> lhs, const Matrix1<T> &rhs) {
    lhs *= rhs;
    return move(lhs);
}

/**
 * @brief Multiplicación escalar por la izquierda: scalar * matriz.
 * @tparam T Tipo numérico.
 * @param scalar Escalar multiplicador.
 * @param lhs Matriz (copia).
 * @return Nueva matriz escalada.
 */
// escalar * matriz
template <typename T>
Matrix1<T> operator*(const T &scalar, Matrix1<T> lhs) {
    lhs *= scalar;
    return move(lhs);
}

/**
 * @brief Multiplicación escalar por la derecha: matriz * scalar.
 * @tparam T Tipo numérico.
 * @param lhs Matriz (copia).
 * @param scalar Escalar multiplicador.
 * @return Nueva matriz escalada.
 */
template <typename T>
Matrix1<T> operator*(Matrix1<T> lhs, const T &scalar) {
    lhs *= scalar;
    return move(lhs);
}

/** @brief Operador de acceso a elementos.
 * @tparam T Tipo numérico.
 * @param row Índice de la fila.
 * @return Puntero a la fila correspondiente.
 */
template <typename T>
T* Matrix1<T>::operator[](size_t row) {
    assert(row <= m_rows);
    return m_pMat[row];
}

/** @brief Operador de acceso a elementos (const).
 * @tparam T Tipo numérico.
 * @param row Índice de la fila.
 * @return Puntero a la fila correspondiente.
 */
template <typename T>
const T* Matrix1<T>::operator[](size_t row) const {
    assert(row <= m_rows);
    return m_pMat[row];
}

/** @brief Transpuesta de la matriz.
 * @tparam T Tipo numérico.
 * @return Nueva matriz transpuesta.
 */
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

/** @brief Eliminación de Gauss.
 * @tparam T Tipo numérico.
 * @param matrix Matriz a procesar.
 * @param sign Referencia al signo del determinante.
 * @param reduced Indica si se desea la forma escalonada reducida.
 * @return Vector con los pivotes.
 */
template <typename T>
std::vector<T> Matrix1<T>::GaussElim(Matrix1<T> &matrix, size_t &sign, bool reduced) const {
    std::vector<T> pivots;
    size_t pivot_row = 0;

    for (size_t col = 0; col < matrix.m_cols && pivot_row < matrix.m_rows; ++col) {

        // Buscar pivote
        size_t found = matrix.m_rows;
        for (size_t row = pivot_row; row < matrix.m_rows; ++row)
            if (matrix.m_pMat[row][col] != T{}) { found = row; break; }
        if (found == matrix.m_rows) { pivots.push_back(T{0}); continue; }

        // Intercambiar filas
        if (found != pivot_row) {
            swap(matrix.m_pMat[found], matrix.m_pMat[pivot_row]);
            sign *= -1;
        }

        T pivot = matrix.m_pMat[pivot_row][col];
        pivots.push_back(pivot);

        // Normalizar fila pivote
        for (size_t j = 0; j < matrix.m_cols; ++j)
            matrix.m_pMat[pivot_row][j] /= pivot;

        // Eliminar filas - paralelizado con std::thread
        size_t start = reduced ? 0 : pivot_row + 1;

        std::vector<std::thread> threads;
        for (size_t row = start; row < matrix.m_rows; ++row) {
            if (row == pivot_row) continue;
            threads.emplace_back([&matrix, col, row, pivot_row]() {
                T factor = matrix.m_pMat[row][col];
                for (size_t j = 0; j < matrix.m_cols; ++j)
                    matrix.m_pMat[row][j] -= factor * matrix.m_pMat[pivot_row][j];
            });
        }
        for (auto &t : threads) t.join();

        ++pivot_row;
    }
    return pivots;
}

/** @brief Forma escalonada de la matriz.
 * @tparam T Tipo numérico.
 * @param reduced Indica si se desea la forma escalonada reducida.
 * @return Nueva matriz en forma escalonada.
 */
template <typename T>
Matrix1<T> Matrix1<T>::RowEchelon(bool reduced) const {
    Matrix1<T> result(*this);
    size_t sign = 1;
    GaussElim(result, sign, reduced);
    return result;
}

/** @brief Determinante de la matriz.
 * @tparam T Tipo numérico.
 * @return Valor del determinante.
 */
template <typename T>
T Matrix1<T>::Determinant() const {
    assert(m_rows == m_cols);

    Matrix1<T> work(*this);
    size_t sign = 1;
    auto pivots = GaussElim(work, sign);

    T det = T{1};
    for (T p : pivots)
        if (p == T{0}) return T{0};  // columna cero -> det = 0
        else det *= p;

    return det * sign;
}

#endif // __MATRIX_H__