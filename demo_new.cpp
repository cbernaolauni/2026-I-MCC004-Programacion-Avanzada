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

// Demo1 - Creación y destrucción de un objeto usando new y delete
void demo1() {

    std::cout << "Creando objeto...\n\n";

    Nodo* nodo = new Nodo(42);

    std::cout << "\nEliminando objeto...\n\n";

    delete nodo;
}

// Demo2 - Uso de memoria administrada con std::pmr
// std::pmr::monotonic_buffer_resource hace que la memoria se asigne de manera contigua y no se libere hasta que el recurso sea destruido. 
// Esto es útil para estructuras de datos que crecen y no necesitan liberar memoria individualmente.
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

// Demo3 - Uso de pool de memoria no sincronizado
// std::pmr::unsynchronized_pool_resource es un recurso de memoria que permite la asignación
// y liberación rápida de bloques de memoria de tamaño fijo.
void demo3() {

    std::pmr::unsynchronized_pool_resource pool;

    std::pmr::vector<TI> valores{
        &pool
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

// Demo4 - Uso de pool de memoria sincronizado
// std::pmr::synchronized_pool_resource es similar a unsynchronized_pool_resource, pero es seguro para el uso en múltiples hilos.
void demo4() {

    std::pmr::synchronized_pool_resource pool;

    std::pmr::vector<TI> valores{
        &pool
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

// Demo5 - Uso de polimorphic_allocator
// std::pmr::polymorphic_allocator es un asignador que puede trabajar con cualquier recurso de memoria polimórfico.
void demo5() {

    std::pmr::monotonic_buffer_resource resource;

    std::pmr::polymorphic_allocator<TI> allocator{
        &resource
    };

    TI* valor = allocator.allocate(1);

    allocator.construct(valor, 42);

    std::cout << "Valor: "
              << *valor
              << '\n';

    allocator.destroy(valor);

    allocator.deallocate(valor, 1);
}

void DemoNew() {
    std::cout << "Demo de new y delete:\n\n";
    demo1();

    std::cout << "\nDemo de memoria administrada:\n\n";
    demo2();

    std::cout << "\nDemo de pool de memoria (no sincronizado):\n\n";
    demo3();

    std::cout << "\nDemo de pool de memoria (sincronizado):\n\n";
    demo4();

    std::cout << "\nDemo de polimorphic_allocator:\n\n";
    demo5();
}