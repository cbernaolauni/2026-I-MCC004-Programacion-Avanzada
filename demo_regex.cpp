#include <regex>
#include <string>
#include <iostream>
using namespace std;

void validarEmail() {
    string email = "cesar@gmail.com";
    regex patron(R"([a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})");
    
    if (regex_match(email, patron))
        cout << "Email válido" << endl;
    else
        cout << "Email inválido" << endl;
}

void extraerInformacion() {
    string texto = "Tengo 3 gatos, 12 perros y 1 tortuga";
    regex numero(R"(\d+)");

    auto begin = sregex_iterator(texto.begin(), texto.end(), numero);
    auto end   = sregex_iterator();

    for (auto it = begin; it != end; ++it)
        cout << it->str() << endl;  // 3, 12, 1
}

void reemplazarTexto() {
    string texto = "El color es rojo";
    regex patron(R"(rojo)");
    string reemplazo = "azul";

    string resultado = regex_replace(texto, patron, reemplazo);
    cout << resultado << endl;  // El color es azul
}

void extraerGrupos() {
    string texto = "Mi número de teléfono es 123-456-7890";
    regex patron(R"((\d{3})-(\d{3})-(\d{4}))");
    smatch coincidencia;

    if (regex_search(texto, coincidencia, patron)) {
        cout << "Código de área: " << coincidencia[1] << endl;  // 123
        cout << "Prefijo: " << coincidencia[2] << endl;         // 456
        cout << "Número: " << coincidencia[3] << endl;          // 7890
    }
}

void saltarLinea() {
    string linea = "# esto es un comentario";
    regex comentario(R"(^\s*#.*$)");

    if (regex_match(linea, comentario))
        cout << "Saltando comentario" << endl;
}

void DemoRegex() {
    cout << "Demo de expresiones regulares en C++\n" << endl;
    cout << "Validar Email:" << endl;
    validarEmail();
    cout << "\nExtraer Información:" << endl;
    extraerInformacion();
    cout << "\nReemplazar Texto:" << endl;
    reemplazarTexto();
    cout << "\nExtraer Grupos:" << endl;
    extraerGrupos();
    cout << "\nSaltar Línea:" << endl;
    saltarLinea();
}