#include <cstddef>
#include <iostream>
#include <new>
#include <memory_resource>
#include "types.h"

// ============================================================
// Demo1 - Sobrecarga de operator new[] / delete[] (arreglos)
// ============================================================
class NodoArreglo {
private:
    TI valor;
 
public:
    explicit NodoArreglo(TI valor = 0)
        : valor(valor) {
        std::cout << "Constructor: NodoArreglo("
                  << valor << ")\n";
    }
 
    ~NodoArreglo() {
        std::cout << "Destructor: NodoArreglo("
                  << valor << ")\n";
    }
 
    // Sobrecarga para objetos individuales
    static void* operator new(std::size_t size) {
        std::cout << "[operator new] Solicitando "
                  << size << " bytes\n";
        return ::operator new(size);
    }
 
    static void operator delete(void* memoria) noexcept {
        std::cout << "[operator delete] Liberando en: "
                  << memoria << "\n";
        ::operator delete(memoria);
    }
 
    // Sobrecarga para arreglos (new NodoArreglo[N])
    static void* operator new[](std::size_t size) {
        std::cout << "[operator new[]] Solicitando "
                  << size << " bytes para el arreglo\n";
        void* memoria = ::operator new(size);
        std::cout << "Memoria de arreglo asignada en: "
                  << memoria << "\n";
        return memoria;
    }
 
    static void operator delete[](void* memoria) noexcept {
        std::cout << "[operator delete[]] Liberando arreglo en: "
                  << memoria << "\n";
        ::operator delete(memoria);
    }
};
 
void demo1() {
    std::cout << "Creando arreglo de 3 objetos...\n\n";
 
    NodoArreglo* arreglo = new NodoArreglo[3];
 
    std::cout << "\nEliminando arreglo...\n\n";
 
    delete[] arreglo;
}
 
// ============================================================
// Demo2 - Sobrecarga de operator new con std::nothrow
// En vez de lanzar std::bad_alloc, retorna nullptr si falla.
// ============================================================
class NodoNothrow {
private:
    TI valor;
 
public:
    explicit NodoNothrow(TI valor)
        : valor(valor) {
        std::cout << "Constructor: NodoNothrow("
                  << valor << ")\n";
    }
 
    ~NodoNothrow() {
        std::cout << "Destructor: NodoNothrow("
                  << valor << ")\n";
    }
 
    static void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
        std::cout << "[operator new nothrow] Solicitando "
                  << size << " bytes (sin excepciones)\n";
 
        void* memoria = ::operator new(size, std::nothrow);
 
        if (!memoria) {
            std::cout << "Fallo la asignacion, retornando nullptr\n";
        } else {
            std::cout << "Memoria asignada en: "
                      << memoria << "\n";
        }
 
        return memoria;
    }
 
    static void operator delete(void* memoria, const std::nothrow_t&) noexcept {
        std::cout << "[operator delete nothrow] Liberando en: "
                  << memoria << "\n";
        ::operator delete(memoria);
    }

    // operator delete "usual": requerido para el "delete nodo;" normal.
    // Sin este, el compilador no sabe con que funcion liberar el objeto
    // una vez construido con exito (el overload con nothrow_t solo se
    // usa si el constructor lanza una excepcion).
    static void operator delete(void* memoria) noexcept {
        std::cout << "[operator delete] Liberando en: "
                  << memoria << "\n";
        ::operator delete(memoria);
    }
};
 
void demo2() {
    std::cout << "Creando objeto con new(std::nothrow)...\n\n";
 
    NodoNothrow* nodo = new (std::nothrow) NodoNothrow(7);
 
    if (nodo) {
        std::cout << "\nEliminando objeto...\n\n";
        delete nodo;
    }
}
 
// ============================================================
// Demo3 - Sobrecarga de operator new con parametros extra
// Util para rastrear en que archivo/linea se hizo cada asignacion
// (tecnica comun de "debug allocators").
// ============================================================
class NodoDebug {
private:
    TI valor;
 
public:
    explicit NodoDebug(TI valor)
        : valor(valor) {
        std::cout << "Constructor: NodoDebug("
                  << valor << ")\n";
    }
 
    ~NodoDebug() {
        std::cout << "Destructor: NodoDebug("
                  << valor << ")\n";
    }
 
    // Overload adicional: recibe archivo y linea como parametros extra
    static void* operator new(std::size_t size, const char* archivo, int linea) {
        std::cout << "[operator new debug] Solicitando "
                  << size << " bytes desde "
                  << archivo << ":" << linea << "\n";
 
        void* memoria = ::operator new(size);
 
        std::cout << "Memoria asignada en: "
                  << memoria << "\n";
 
        return memoria;
    }
 
    // delete "de emparejamiento" para el new con parametros extra
    // (solo se invoca si el constructor lanza una excepcion)
    static void operator delete(void* memoria, const char*, int) noexcept {
        std::cout << "[operator delete debug] Liberando en: "
                  << memoria << "\n";
        ::operator delete(memoria);
    }
 
    // delete normal, para el "delete nodo;" comun
    static void operator delete(void* memoria) noexcept {
        std::cout << "[operator delete] Liberando en: "
                  << memoria << "\n";
        ::operator delete(memoria);
    }
};
 
// Macro de conveniencia, como se usa en muchos allocators de debug
#define NEW_DEBUG(tipo) new (__FILE__, __LINE__) tipo
 
void demo3() {
    std::cout << "Creando objeto con new con rastreo de archivo/linea...\n\n";
 
    NodoDebug* nodo = NEW_DEBUG(NodoDebug)(8);
 
    std::cout << "\nEliminando objeto...\n\n";
 
    delete nodo;
}
 
// ============================================================
// Demo4 - Sobrecarga de operator new con alineacion
// Para tipos con requisitos de alineacion mayores al default
// (relevante desde C++17, con std::align_val_t).
// ============================================================
class alignas(64) NodoAlineado {
private:
    TI valor;
 
public:
    explicit NodoAlineado(TI valor)
        : valor(valor) {
        std::cout << "Constructor: NodoAlineado("
                  << valor << ")\n";
    }
 
    ~NodoAlineado() {
        std::cout << "Destructor: NodoAlineado("
                  << valor << ")\n";
    }
 
    static void* operator new(std::size_t size, std::align_val_t alineacion) {
        std::cout << "[operator new alineado] Solicitando "
                  << size << " bytes con alineacion de "
                  << static_cast<std::size_t>(alineacion) << " bytes\n";
 
        void* memoria = ::operator new(size, alineacion);
 
        std::cout << "Memoria alineada asignada en: "
                  << memoria << "\n";
 
        return memoria;
    }
 
    static void operator delete(void* memoria, std::align_val_t alineacion) noexcept {
        std::cout << "[operator delete alineado] Liberando en: "
                  << memoria << "\n";
        ::operator delete(memoria, alineacion);
    }
};
 
void demo4() {
    std::cout << "Creando objeto con alineacion de 64 bytes...\n\n";
 
    // El compilador pasa automaticamente std::align_val_t{64}
    // porque alignas(64) supera la alineacion por defecto.
    NodoAlineado* nodo = new NodoAlineado(9);
 
    std::cout << "\nEliminando objeto...\n\n";
 
    delete nodo;
}
 
void DemoNew() {
    std::cout << "Demo de operator new[] / delete[]:\n\n";
    demo1();
 
    std::cout << "\nDemo de operator new con std::nothrow:\n\n";
    demo2();
 
    std::cout << "\nDemo de operator new con parametros extra (debug):\n\n";
    demo3();
 
    std::cout << "\nDemo de operator new con alineacion personalizada:\n\n";
    demo4();
}