#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>

struct Regla {
    std::string nombre;
    std::string condiciones;
    std::string consecuencia;
};

std::vector<Regla> leerArchivoDeReglas(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo con nombre: " << filename << '\n';
        return {};
    }

    std::vector<Regla> reglas;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue; // Se salta las lineas vacias
        }

        // Desglosa la linea en atributos
        std::string Regla;
        std::string condiciones;
        std::string consecuencia;
        std::istringstream iss(line);
        std::string ifStr, arrowStr;
        iss >> Regla >> ifStr;

        std::vector<std::string> tokens;
        std::string token;
        while (iss >> token) {
            if (token == "=>") {
                break;
            }
            tokens.push_back(token);
        }

        condiciones = tokens[0];
        for (int i = 1; i < tokens.size(); i++) {
            condiciones += " " + tokens[i];
        }

        iss >> consecuencia;

        // Se crea la regla y se anade al vector de las mismas
        reglas.push_back({Regla, condiciones, consecuencia});
    }

    file.close();

    return reglas;
}

std::vector<char> leerArchivoDeHechosConocidos(const std::string& filename) {
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo con nombre: " << filename << '\n';
        return {};
    }

    std::vector<char> hechosConocidos;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) {
            continue; // Se salta las lineas vacias
        }

        std::istringstream iss(line);
        std::string factStr;

        // Si el substring contenido es mas largo que 1...
        while (std::getline(iss, factStr, ',')) {
            if (factStr.size() > 1) {
                std::cerr << "Formato invalido en archivo: " << filename << '\n';
                return {};
            }

            hechosConocidos.push_back(factStr[0]);
        }
    }

    file.close();

    return hechosConocidos;
}

std::vector<char> forwardChaining(const std::vector<Regla>& reglas, const std::vector<char>& hechosConocidos) {
    std::vector<char> hechosInferidos(hechosConocidos.begin(), hechosConocidos.end());

    bool nuevosHechos = true;
    while (nuevosHechos) {
        nuevosHechos = false;

        // Se manejan tanto condiciones como los operadores AND (&) y OR (|)
        for (const auto& regla : reglas) {
            std::istringstream iss(regla.condiciones);
            std::vector<char> condiciones;
            std::vector<char> operadores;

            char token;
            while (iss >> token) {
                if (token == '&' || token == '|') {
                    operadores.push_back(token);
                } else if (token == '~') {
                    iss >> token;
                    condiciones.push_back('!' + token);
                } else {
                    condiciones.push_back(token);
                }
            }

            bool condicionSatisfecha = true;
            for (size_t i = 0; i < condiciones.size(); i++) {
                char hecho = condiciones[i];
                if (hecho == '&' || hecho == '|') {
                    continue;
                }

                bool hechoNegado = false;
                if (hecho < 0) {
                    hechoNegado = true;
                    hecho = -hecho;
                }

                bool hechoPresente = false;
                for (size_t j = 0; j < hechosInferidos.size(); j++) {
                    if (hechosInferidos[j] == hecho) {
                        hechoPresente = true;
                        break;
                    }
                }
                if (hechoNegado) {
                    hechoPresente = !hechoPresente;
                }

                if (i == 0) {
                    condicionSatisfecha = hechoPresente;
                } else if (operadores[i - 1] == '&') {
                    condicionSatisfecha = condicionSatisfecha && hechoPresente;
                } else if (operadores[i - 1] == '|') {
                    condicionSatisfecha = condicionSatisfecha || hechoPresente;
                }
            }

            if (condicionSatisfecha) {
                bool hechoPreviamenteInferido = false;
                for (char hechoInferido : hechosInferidos) {
                    if (hechoInferido == regla.consecuencia[0]) {
                        hechoPreviamenteInferido = true;
                        break;
                    }
                }
                if (!hechoPreviamenteInferido) {
                    hechosInferidos.push_back(regla.consecuencia[0]);
                    nuevosHechos = true;
                }
            }
        }
    }

    return hechosInferidos;
}

int main() {

    // Luis Bravo, Fabio Buitrago, Camilo Garcia

    // ~ se usa en vez de ¬	ya que el mismo da errores en la lectura del archivo.

    // Leer reglas
    std::vector<Regla> reglas = leerArchivoDeReglas("reglas.txt");
    for (const auto& regla : reglas) {
            std::cout << "Regla: " << regla.nombre << '\n';
            std::cout << "Condiciones: " << regla.condiciones << '\n';
            std::cout << "Consecuencias: " << regla.consecuencia << "\n\n";
    }


    // Leer los hechos conocidos
    std::vector<char> hechosConocidos = leerArchivoDeHechosConocidos("hechos_conocidos.txt");

    std::cout << "Hechos conocidos: ";
    for (const auto& hecho : hechosConocidos) {
        std::cout << hecho << " ";
    }
    std::cout << '\n';

    // Inferir hechos, basandose en las reglas
    std::vector<char> hechosInferidos = forwardChaining(reglas, hechosConocidos);

    std::cout << "Hechos inferidos haciendo uso de las reglas y hechos previamente conocidos: ";
    for (char hecho : hechosInferidos) {
        std::cout << hecho << " ";
    }
    std::cout << "\n";

    return 0;
}
