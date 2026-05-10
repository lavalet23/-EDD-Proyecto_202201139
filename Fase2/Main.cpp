#include <iostream>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <vector>
#include <list>
#include <stack>
#include <ctime>


using namespace std;
using json = nlohmann::json;

// Estructura para representar una solicitud de amistad
struct Solicitud {
    string emisor;
    string receptor;
    string estado;  // "PENDIENTE", "ACEPTADA", "RECHAZADA"
};

// Estructura del usuario
struct Usuario {
    string nombres;
    string apellidos;
    string fechaNacimiento;
    string correo;
    string contrasena;
    bool esAdministrador;
    Usuario* siguiente;
    stack<Solicitud> solicitudesPendientes; // Cambiamos el nombre para claridad
    list<Solicitud> solicitudesEnviadas;
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

    Usuario* buscarUsuario(const string& correo) const {
        Usuario* actual = cabeza;
        while (actual != nullptr) {
            if (actual->correo == correo) {
                return actual;
            }
            actual = actual->siguiente;
        }
        return nullptr;
    }

    void enviarSolicitudAmistad(const string& emisor, const string& receptor) {
        Usuario* usuarioEmisor = buscarUsuario(emisor);
        Usuario* usuarioReceptor = buscarUsuario(receptor);

        if (usuarioEmisor && usuarioReceptor) {
            // Verificar si ya existe una solicitud pendiente
            for (const auto& solicitud : usuarioEmisor->solicitudesEnviadas) {
                if (solicitud.receptor == receptor && solicitud.estado == "PENDIENTE") {
                    cout << "Ya existe una solicitud de amistad pendiente para este usuario.\n";
                    return;
                }
            }

            // Crear una nueva solicitud con estado "PENDIENTE"
            Solicitud nuevaSolicitud = {emisor, receptor, "PENDIENTE"};
            usuarioEmisor->solicitudesEnviadas.push_back(nuevaSolicitud); // Agregar a la lista de solicitudes enviadas
            usuarioReceptor->solicitudesPendientes.push(nuevaSolicitud);  // Agregar a la pila de solicitudes recibidas
            cout << "Solicitud de amistad enviada con éxito.\n";
        } else {
            cout << "Error: Usuario no encontrado.\n";
        }
    }

    void aceptarSolicitudAmistad(const string& receptor, const string& emisor) {
    Usuario* usuarioReceptor = buscarUsuario(receptor);
    Usuario* usuarioEmisor = buscarUsuario(emisor);

    if (usuarioReceptor && usuarioEmisor) {
        stack<Solicitud> tempStack;
        bool found = false;

        while (!usuarioReceptor->solicitudesPendientes.empty()) {
            Solicitud solicitud = usuarioReceptor->solicitudesPendientes.top();
            usuarioReceptor->solicitudesPendientes.pop();

            if (solicitud.emisor == emisor && solicitud.estado == "PENDIENTE") {
                solicitud.estado = "ACEPTADA";
                found = true;

                // Actualizar en el emisor
                for (auto& solEnv : usuarioEmisor->solicitudesEnviadas) {
                    if (solEnv.receptor == receptor && solEnv.estado == "PENDIENTE") {
                        solEnv.estado = "ACEPTADA";
                        break;
                    }
                }

                cout << "Solicitud de amistad aceptada.\n";
            } else {
                tempStack.push(solicitud);
            }
        }

        // Restaurar solo las solicitudes que siguen pendientes
        while (!tempStack.empty()) {
            usuarioReceptor->solicitudesPendientes.push(tempStack.top());
            tempStack.pop();
        }

        if (!found) {
            cout << "No hay solicitud pendiente de este usuario.\n";
        }
    } else {
        cout << "Error: Usuario no encontrado.\n";
    }
}

void rechazarSolicitudAmistad(const string& receptor, const string& emisor) {
    Usuario* usuarioReceptor = buscarUsuario(receptor);
    Usuario* usuarioEmisor = buscarUsuario(emisor);

    if (usuarioReceptor && usuarioEmisor) {
        stack<Solicitud> tempStack;
        bool found = false;

        while (!usuarioReceptor->solicitudesPendientes.empty()) {
            Solicitud solicitud = usuarioReceptor->solicitudesPendientes.top();
            usuarioReceptor->solicitudesPendientes.pop();

            if (solicitud.emisor == emisor && solicitud.estado == "PENDIENTE") {
                solicitud.estado = "RECHAZADA";
                found = true;

                // Actualizar en el emisor
                for (auto& solEnv : usuarioEmisor->solicitudesEnviadas) {
                    if (solEnv.receptor == receptor && solEnv.estado == "PENDIENTE") {
                        solEnv.estado = "RECHAZADA";
                        break;
                    }
                }

                cout << "Solicitud de amistad rechazada.\n";
            } else {
                tempStack.push(solicitud);
            }
        }

        // Restaurar solo las solicitudes que siguen pendientes
        while (!tempStack.empty()) {
            usuarioReceptor->solicitudesPendientes.push(tempStack.top());
            tempStack.pop();
        }

        if (!found) {
            cout << "No hay solicitud pendiente de este usuario.\n";
        }
    } else {
        cout << "Error: Usuario no encontrado.\n";
    }
}

void eliminarUsuario(const string& correo) {
    Usuario* actual = cabeza;
    Usuario* anterior = nullptr;

    while (actual != nullptr && actual->correo != correo) {
        anterior = actual;
        actual = actual->siguiente;
    }

    if (actual == nullptr) {
        cout << "Usuario no encontrado.\n";
        return;
    }

    if (anterior == nullptr) {
        cabeza = actual->siguiente;
    } else {
        anterior->siguiente = actual->siguiente;
    }

    delete actual;
    cout << "Usuario eliminado con éxito.\n";
}

};

struct Publicacion {
    string correoUsuario;
    string contenido;
    string fecha;
    string hora;
    Publicacion* siguiente;
    Publicacion* anterior;
};

// Clase para manejar la lista doblemente enlazada de publicaciones
class ListaPublicaciones {
private:
    Publicacion* cabeza;
    Publicacion* cola;

public:
    ListaPublicaciones() : cabeza(nullptr), cola(nullptr) {}

    void agregarPublicacion(const string& correo, const string& contenido) {
        Publicacion* nuevaPublicacion = new Publicacion();
        nuevaPublicacion->correoUsuario = correo;
        nuevaPublicacion->contenido = contenido;

        // Obtener fecha y hora actual
        time_t now = time(0);
        tm* ltm = localtime(&now);
        char fecha[11];
        char hora[9];
        strftime(fecha, sizeof(fecha), "%d/%m/%Y", ltm);
        strftime(hora, sizeof(hora), "%H:%M:%S", ltm);

        nuevaPublicacion->fecha = fecha;
        nuevaPublicacion->hora = hora;

        if (!cabeza) {
            cabeza = cola = nuevaPublicacion;
        } else {
            cola->siguiente = nuevaPublicacion;
            nuevaPublicacion->anterior = cola;
            cola = nuevaPublicacion;
        }
    }

    // Método para obtener publicaciones de un usuario y sus amigos
    list<Publicacion*> obtenerPublicacionesDeUsuario(const string& correoUsuario) const {
        list<Publicacion*> publicacionesUsuario;
        Publicacion* actual = cabeza;
        while (actual) {
            if (actual->correoUsuario == correoUsuario) {
                publicacionesUsuario.push_back(actual);
            }
            actual = actual->siguiente;
        }
        return publicacionesUsuario;
    }
    // ... resto de la clase ...
};

class FeedCircular {
private:
    list<Publicacion*> publicaciones;
    list<Publicacion*>::iterator it;

public:
    FeedCircular(const list<Publicacion*>& pubs) : publicaciones(pubs) {
        if (!publicaciones.empty()) {
            it = publicaciones.begin();
        }
    }

    void mostrarPublicacionActual() {
        if (publicaciones.empty()) {
            cout << "No hay publicaciones para mostrar." << endl;
            return;
        }

        cout << "Autor: " << (*it)->correoUsuario << endl;
        cout << "Fecha: " << (*it)->fecha << " Hora: " << (*it)->hora << endl;
        cout << "Contenido: " << (*it)->contenido << endl;
    }

    void siguiente() {
        if (publicaciones.empty()) return;
        ++it;
        if (it == publicaciones.end()) {
            it = publicaciones.begin();
        }
    }

    void anterior() {
        if (publicaciones.empty()) return;
        if (it == publicaciones.begin()) {
            it = publicaciones.end();
        }
        --it;
    }
};

void cargarSolicitudes(ListaUsuarios& usuarios, const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filePath << endl;
        return;
    }

    json data;
    try {
        file >> data;

        if (!data.is_array()) {
            cerr << "Error: El archivo JSON debe contener un array de solicitudes.\n";
            return;
        }

        for (const auto& solicitud : data) {
            string emisor = solicitud.at("emisor").get<string>();
            string receptor = solicitud.at("receptor").get<string>();
            string estado = solicitud.at("estado").get<string>();

            Usuario* usuarioEmisor = usuarios.buscarUsuario(emisor);
            Usuario* usuarioReceptor = usuarios.buscarUsuario(receptor);

            if (usuarioEmisor && usuarioReceptor) {
                Solicitud nuevaSolicitud = {emisor, receptor, estado};
                usuarioEmisor->solicitudesEnviadas.push_back(nuevaSolicitud); // Enviar solicitud a lista de envíos
                usuarioReceptor->solicitudesPendientes.push(nuevaSolicitud);  // Enviar solicitud a pila de recibidos
            }
        }
        cout << "Carga masiva de solicitudes completada.\n";
    } catch (const json::exception& e) {
        cerr << "Error al procesar el archivo JSON de solicitudes: " << e.what() << endl;
    }
}

void cargarPublicaciones(ListaPublicaciones& publicaciones, const string& filePath) {
    ifstream file(filePath);
    if (!file.is_open()) {
        cerr << "Error: No se pudo abrir el archivo " << filePath << endl;
        return;
    }

    json data;
    try {
        file >> data;

        if (!data.is_array()) {
            cerr << "Error: El archivo JSON debe contener un array de publicaciones.\n";
            return;
        }

        for (const auto& pub : data) {
            string correo = pub.at("correo").get<string>();
            string contenido = pub.at("contenido").get<string>();
            publicaciones.agregarPublicacion(correo, contenido);
        }
        cout << "Carga masiva de publicaciones completada.\n";
    } catch (const json::exception& e) {
        cerr << "Error al procesar el archivo JSON de publicaciones: " << e.what() << endl;
    }
}

// Declaraciones de las funciones
void mostrarMenuAdministrador(ListaUsuarios& usuarios, ListaPublicaciones& publicaciones);
void mostrarMenuUsuario(ListaUsuarios& usuarios, ListaPublicaciones& publicaciones, const string& correoUsuario);
void gestionarUsuarios(ListaUsuarios& usuarios);


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

bool iniciarSesion(ListaUsuarios& usuarios, ListaPublicaciones& publicaciones) {
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
            mostrarMenuAdministrador(usuarios, publicaciones); // Mostrar el menú de administrador
        } else {
            cout << "Has iniciado sesion como Usuario regular.\n";
            mostrarMenuUsuario(usuarios, publicaciones, correo); // Mostrar el menú de usuario
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

void mostrarMenuAdministrador(ListaUsuarios& usuarios, ListaPublicaciones& publicaciones) {
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
            case 2: {
                string filePath;
                cout << "Ingrese el path del archivo JSON para cargar solicitudes: ";
                getline(cin, filePath);
                cargarSolicitudes(usuarios, filePath);
                break;
            }
            case 3: {
                string filePath;
                cout << "Ingrese el path del archivo JSON para cargar publicaciones: ";
                getline(cin, filePath);
                cargarPublicaciones(publicaciones, filePath);
                break;
            }
            case 4:
                gestionarUsuarios(usuarios);
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

void mostrarMenuUsuario(ListaUsuarios& usuarios, ListaPublicaciones& publicaciones, const string& correoUsuario) {
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
            case 1: {
                int opcionPerfil;
                cout << "\n--- Perfil ---\n";
                cout << "1. Ver perfil\n";
                cout << "2. Eliminar cuenta\n";
                cout << "Seleccione una opcion: ";
                cin >> opcionPerfil;
                cin.ignore();

                switch (opcionPerfil) {
                    case 1: {
                        Usuario* usuarioActual = usuarios.buscarUsuario(correoUsuario);
                        if (usuarioActual) {
                            cout << "\n--- Perfil de Usuario ---\n";
                            cout << "Nombres: " << usuarioActual->nombres << endl;
                            cout << "Apellidos: " << usuarioActual->apellidos << endl;
                            cout << "Fecha de Nacimiento: " << usuarioActual->fechaNacimiento << endl;
                            cout << "Correo: " << usuarioActual->correo << endl;
                            
                            cout << "\nPublicaciones:\n";
                            list<Publicacion*> publicacionesUsuario = publicaciones.obtenerPublicacionesDeUsuario(correoUsuario);
                            if (publicacionesUsuario.empty()) {
                                cout << "Este usuario aún no ha realizado publicaciones.\n";
                            } else {
                                for (const auto& pub : publicacionesUsuario) {
                                    cout << "Fecha: " << pub->fecha << " Hora: " << pub->hora << endl;
                                    cout << "Contenido: " << pub->contenido << endl;
                                    cout << "------------------------\n";
                                }
                            }
                        } else {
                            cout << "Error: Usuario no encontrado.\n";
                        }
                        break;
                    }
                    case 2: {
                        char confirmacion;
                        cout << "¿Está seguro que desea eliminar su cuenta? (s/n): ";
                        cin >> confirmacion;
                        cin.ignore();
                        if (confirmacion == 's' || confirmacion == 'S') {
                            usuarios.eliminarUsuario(correoUsuario);
                            return; // Salir del menú de usuario después de eliminar la cuenta
                        }
                        break;
                    }
                    default:
                        cout << "Opcion invalida.\n";
                }
                break;
            }
            case 2:{
                int opcionSolicitud;
                cout << "\n--- Solicitudes ---\n";
                cout << "1. Ver solicitudes recibidas\n";
                cout << "2. Enviar solicitud\n";
                cout << "Seleccione una opcion: ";
                cin >> opcionSolicitud;
                cin.ignore();

                switch (opcionSolicitud) {
                    case 1: {
    Usuario* usuarioActual = usuarios.buscarUsuario(correoUsuario);
    if (usuarioActual) {
        if (usuarioActual->solicitudesPendientes.empty()) {
            cout << "No tienes solicitudes de amistad pendientes.\n";
        } else {
            stack<Solicitud> tempStack;
            stack<Solicitud> stackRestauracion;  // Pila adicional para restaurar todas las solicitudes

            while (!usuarioActual->solicitudesPendientes.empty()) {
                Solicitud solicitud = usuarioActual->solicitudesPendientes.top();
                usuarioActual->solicitudesPendientes.pop();

                cout << "Solicitud de: " << solicitud.emisor << " - Estado: " << solicitud.estado << endl;

                if (solicitud.estado == "PENDIENTE") {
                    char respuesta;
                    cout << "Desea aceptar la solicitud de " << solicitud.emisor << "? (s/n): ";
                    cin >> respuesta;
                    cin.ignore(); // Limpiar el buffer

                    if (respuesta == 's' || respuesta == 'S') {
                        usuarios.aceptarSolicitudAmistad(correoUsuario, solicitud.emisor);
                        solicitud.estado = "ACEPTADA";  // Actualizar el estado localmente
                    } else {
                        usuarios.rechazarSolicitudAmistad(correoUsuario, solicitud.emisor);
                        solicitud.estado = "RECHAZADA";  // Actualizar el estado localmente
                    }
                }

                stackRestauracion.push(solicitud);  // Agregar todas las solicitudes a la pila de restauración
            }

            // Restaurar todas las solicitudes con sus estados actualizados
            while (!stackRestauracion.empty()) {
                usuarioActual->solicitudesPendientes.push(stackRestauracion.top());
                stackRestauracion.pop();
            }

            cout << "Todas las solicitudes han sido procesadas.\n";
        }
    } else {
        cout << "Usuario no encontrado.\n";
    }
    break;
}
                    case 2: {
                        string receptor;
                        cout << "Ingrese el correo del usuario a quien enviar solicitud: ";
                        getline(cin, receptor);
                        usuarios.enviarSolicitudAmistad(correoUsuario, receptor);
                        break;
                    }
                }
                break;
            }
            
            case 3: {
                int opcionPublicacion;
                cout << "\n--- Publicaciones ---\n";
                cout << "1. Ver todas las publicaciones\n";
                cout << "2. Crear nueva publicacion\n";
                cout << "Seleccione una opcion: ";
                cin >> opcionPublicacion;
                cin.ignore();

                switch (opcionPublicacion) {
                    case 1: {
                        list<Publicacion*> publicacionesUsuario = publicaciones.obtenerPublicacionesDeUsuario(correoUsuario);
                        FeedCircular feed(publicacionesUsuario);
                        char navegacion;
                        do {
                            feed.mostrarPublicacionActual();
                            cout << "\n(A) Anterior, (S) Siguiente, (Q) Salir: ";
                            cin >> navegacion;
                            if (navegacion == 'A' || navegacion == 'a') feed.anterior();
                            else if (navegacion == 'S' || navegacion == 's') feed.siguiente();
                        } while (navegacion != 'Q' && navegacion != 'q');
                        break;
                    }
                    case 2: {
                        string contenido;
                        cout << "Ingrese el contenido de su publicacion: ";
                        getline(cin, contenido);
                        publicaciones.agregarPublicacion(correoUsuario, contenido);
                        cout << "Publicacion creada con exito.\n";
                        break;
                    }
                    default:
                        cout << "Opcion invalida.\n";
                }
                break;
            }
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

void gestionarUsuarios(ListaUsuarios& usuarios) {
    int opcion;
    do {
        cout << "\n--- Gestionar Usuarios ---\n";
        cout << "1. Eliminar usuario\n";
        cout << "2. Volver al menu anterior\n";
        cout << "Seleccione una opcion: ";
        cin >> opcion;
        cin.ignore();

        switch (opcion) {
            case 1: {
                string correo;
                cout << "Ingrese el correo del usuario a eliminar: ";
                getline(cin, correo);
                
                // Verificar que no se intente eliminar al administrador
                if (correo == "admin@gmail.com") {
                    cout << "No se puede eliminar la cuenta de administrador.\n";
                } else {
                    char confirmacion;
                    cout << "¿Está seguro que desea eliminar este usuario? (s/n): ";
                    cin >> confirmacion;
                    cin.ignore();
                    if (confirmacion == 's' || confirmacion == 'S') {
                        usuarios.eliminarUsuario(correo);
                    }
                }
                break;
            }
            case 2:
                cout << "Volviendo al menu anterior...\n";
                break;
            default:
                cout << "Opcion invalida. Por favor, intente de nuevo.\n";
        }
    } while (opcion != 2);
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
    ListaPublicaciones publicaciones;
    
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
            case 2: {
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
                        mostrarMenuAdministrador(usuarios, publicaciones);
                    } else {
                        cout << "Has iniciado sesion como Usuario regular.\n";
                        mostrarMenuUsuario(usuarios, publicaciones, correo);
                    }
                } else {
                    cout << "Error: Correo o contrasena incorrectos.\n";
                }
                break;
            }
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