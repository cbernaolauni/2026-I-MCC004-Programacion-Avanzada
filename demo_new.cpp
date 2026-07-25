#include <cstddef>
#include <iostream>
#include <new>
#include <memory_resource>
#include "types.h"

class Nodo {
private:
    TI valor;

public:
    explicit Nodo(TI valor)
        : valor(valor) {
        std::cout << "Constructor: Nodo("
                  << valor << ")\n";
    }

    ~Nodo() {
        std::cout << "Destructor: Nodo("
                  << valor << ")\n";
    }

    static void* operator new(std::size_t size) {

        std::cout << "[operator new]\n";
        std::cout << "Solicitando "
                  << size
                  << " bytes\n";

        void* memoria = ::operator new(size);

        std::cout << "Memoria asignada en: "
                  << memoria
                  << "\n";

        return memoria;
    }

    static void operator delete(void* memoria) noexcept {

        std::cout << "[operator delete]\n";
        std::cout << "Liberando memoria en: "
                  << memoria
                  << "\n";

        ::operator delete(memoria);
    }
};

void demo1() {

    std::cout << "Creando objeto...\n\n";

    Nodo* nodo = new Nodo(42);

    std::cout << "\nEliminando objeto...\n\n";

    delete nodo;
}

void demo2() {
    // Buffer administrado por la aplicación
    std::byte buffer[1024 * 1024];

    // Recurso de memoria
    std::pmr::monotonic_buffer_resource resource{
        buffer,
        sizeof(buffer)
    };

    // Vector que utiliza nuestro recurso
    std::pmr::vector<TI> valores{
        &resource
    };

    for (size_t i = 0; i < 100000; ++i) {
        valores.push_back(i);
    }

    std::cout << "Elementos almacenados: "
              << valores.size()
              << '\n';

    std::cout << "Capacidad: "
              << valores.capacity()
              << '\n';
}

void DemoNew() {
    std::cout << "Demo de new y delete:\n\n";
    demo1();

    std::cout << "\nDemo de memoria administrada:\n\n";
    demo2();
}