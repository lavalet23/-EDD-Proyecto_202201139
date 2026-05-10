#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

struct Usuario {
    string nombres;
    string apellidos;
    string fechaNacimiento;
    string correo;
    string contrasena;
    bool esAdministrador;
    Usuario* siguiente;
};

class ListaUsuarios {
private:
    Usuario* cabeza;

public:
    ListaUsuarios() : cabeza(nullptr) {}

    void agregarUsuario(Usuario* nuevoUsuario) {
        nuevoUsuario->siguiente = cabeza;
        cabeza = nuevoUsuario;
    }

    Usuario* buscarUsuario(const string& correo) {
        Usuario* actual = cabeza;
        while (actual != nullptr) {
            if (actual->correo == correo) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }
};

// Declaraciones de las funciones
void mostrarMenuAdministrador(ListaUsuarios& usuarios);
void mostrarMenuUsuario();

void registrarUsuario(ListaUsuarios& usuarios) {
    Usuario* nuevoUsuario = new Usuario();
    
    cout << "Registro de nuevo usuario\n";
    cout << "Nombres: ";
    getline(cin, nuevoUsuario->nombres);
    cout << "Apellidos: ";
    getline(cin, nuevoUsuario->apellidos);
    cout << "Fecha de nacimiento: ";
    getline(cin, nuevoUsuario->fechaNacimiento);
    cout << "Correo electronico: ";
    getline(cin, nuevoUsuario->correo);
    
    if (usuarios.buscarUsuario(nuevoUsuario->correo) != nullptr) {
        cout << "Error: El correo ya esta registrado.\n";
        delete nuevoUsuario;
        return;
    }
    
    cout << "Contrasena: ";
    getline(cin, nuevoUsuario->contrasena);
    
    nuevoUsuario->esAdministrador = false; // Los nuevos usuarios no son administradores por defecto
    
    usuarios.agregarUsuario(nuevoUsuario);
    cout << "Usuario registrado con exito.\n";
}

bool iniciarSesion(ListaUsuarios& usuarios) {
    string correo, contrasena;
    
    cout << "Inicio de sesion\n";
    cout << "Correo electronico: ";
    getline(cin, correo);
    cout << "Contrasena: ";
    getline(cin, contrasena);
    
    Usuario* usuario = usuarios.buscarUsuario(correo);
    
    if (usuario != nullptr && usuario->contrasena == contrasena) {
        cout << "Inicio de sesion exitoso. Bienvenido, " << usuario->nombres << "!\n";
        if (usuario->esAdministrador) {
            cout << "Has iniciado sesion como Administrador.\n";
            mostrarMenuAdministrador(usuarios); // Mostrar el menú de administrador
        } else {
            cout << "Has iniciado sesion como Usuario regular.\n";
            mostrarMenuUsuario(); // Mostrar el menú de usuario
        }
        return true;
    } else {
        cout << "Error: Correo o contrasena incorrectos.\n";
        return false;
    }
}

// Función para cargar usuarios desde un archivo JSON
void cargarUsuarios(ListaUsuarios& usuarios, const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo " << filePath << std::endl;
        return;
    }

    json data;
    try {
        file >> data;

        if (!data.is_array()) {
            std::cerr << "Error: El archivo JSON debe contener un array de usuarios.\n";
            return;
        }

        for (const auto& user : data) {
            Usuario* nuevoUsuario = new Usuario();
            nuevoUsuario->nombres = user.at("nombres").get<string>();
            nuevoUsuario->apellidos = user.at("apellidos").get<string>();
            nuevoUsuario->fechaNacimiento = user.at("fecha_de_nacimiento").get<string>();
            nuevoUsuario->correo = user.at("correo").get<string>();
            nuevoUsuario->contrasena = user.at("contraseña").get<string>();
            nuevoUsuario->esAdministrador = false; // Asumimos que los usuarios cargados no son administradores

            if (usuarios.buscarUsuario(nuevoUsuario->correo) == nullptr) {
                usuarios.agregarUsuario(nuevoUsuario);
                std::cout << "Usuario " << nuevoUsuario->correo << " registrado con éxito.\n";
            } else {
                delete nuevoUsuario;
                std::cout << "El usuario con correo " << nuevoUsuario->correo << " ya existe y no se añadirá.\n";
            }
        }
        std::cout << "Carga masiva de usuarios completada.\n";
    } catch (const json::exception& e) {
        std::cerr << "Error al procesar el archivo JSON: " << e.what() << std::endl;
    }
}

void mostrarMenuAdministrador(ListaUsuarios& usuarios) {
    int opcion;
    
    do {
        cout << "\n--- Menu de Administrador ---\n";
        cout << "1. Carga de Usuarios\n";
        cout << "2. Carga de Relaciones (Solicitudes de Amistad)\n";
        cout << "3. Carga de Publicaciones\n";
        cout << "4. Gestionar Usuarios\n";
        cout << "5. Reportes\n";
        cout << "6. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();
        
        switch (opcion) {
            case 1: {
                std::string filePath;
                std::cout << "Ingrese el path del archivo JSON para cargar usuarios: ";
                std::getline(cin, filePath);
                cargarUsuarios(usuarios, filePath);
                break;
            }
            case 2:
                cout << "Cargando relaciones...\n";
                // Implementar la lógica para cargar relaciones (solicitudes de amistad)
                break;
            case 3:
                cout << "Cargando publicaciones...\n";
                // Implementar la lógica para cargar publicaciones
                break;
            case 4:
                cout << "Gestionando usuarios...\n";
                // Implementar la lógica para gestionar usuarios
                break;
            case 5:
                cout << "Generando reportes...\n";
                // Implementar la lógica para generar reportes
                break;
            case 6:
                cout << "Regresando al menu principal...\n";
                break;
            default:
                cout << "Opcion invalida. Por favor, intente de nuevo.\n";
        }
    } while (opcion != 6);
}

void mostrarMenuUsuario() {
    int opcion;
    
    do {
        cout << "\n--- Menu de Usuario ---\n";
        cout << "1. Perfil\n";
        cout << "2. Solicitudes\n";
        cout << "3. Publicaciones\n";
        cout << "4. Reportes\n";
        cout << "5. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();
        
        switch (opcion) {
            case 1:
                cout << "Mostrando perfil...\n";
                // Implementar la lógica para mostrar el perfil
                break;
            case 2:
                cout << "Mostrando solicitudes...\n";
                // Implementar la lógica para mostrar solicitudes
                break;
            case 3:
                cout << "Mostrando publicaciones...\n";
                // Implementar la lógica para mostrar publicaciones
                break;
            case 4:
                cout << "Generando reportes...\n";
                // Implementar la lógica para generar reportes
                break;
            case 5:
                cout << "Regresando al menu principal...\n";
                break;
            default:
                cout << "Opcion invalida. Por favor, intente de nuevo.\n";
        }
    } while (opcion != 5);
}

void inicializarAdministrador(ListaUsuarios& usuarios) {
    Usuario* administrador = new Usuario();
    administrador->nombres = "Admin";
    administrador->apellidos = "Admin";
    administrador->fechaNacimiento = "01/01/1970";
    administrador->correo = "admin@gmail.com";
    administrador->contrasena = "EDD2S2024";
    administrador->esAdministrador = true; // Especificar que este usuario es administrador
    
    usuarios.agregarUsuario(administrador);
}

int main() {
    ListaUsuarios usuarios;
    
    // Inicializar el usuario administrador predeterminado
    inicializarAdministrador(usuarios);
    
    int opcion;
    
    do {
        cout << "\n--- Red Social ---\n";
        cout << "1. Registrar usuario\n";
        cout << "2. Iniciar sesion\n";
        cout << "3. Informacion del Estudiante\n";
        cout << "4. Salir\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();
        
        switch (opcion) {
            case 1:
                registrarUsuario(usuarios);
                break;
            case 2:
                if (iniciarSesion(usuarios)) {
                    // `iniciarSesion` ya maneja la llamada al menú correspondiente.
                }
                break;
            case 3:
                cout << "Nombre: Keitlyn Valentina Tunchez Castaneda\n";
                cout << "Carnet: 202113524\n";
                break;
            case 4:
                cout << "Saliendo...\n";
                break;
            default:
                cout << "Opcion invalida. Por favor, intente de nuevo.\n";
        }
    } while (opcion != 4);

    return 0;
}