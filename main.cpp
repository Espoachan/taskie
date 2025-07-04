#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <algorithm>
#include "json.hpp"

using json = nlohmann::json;
namespace fs = std::filesystem;

const std::string TASKIE_DIR = ".taskie";
const std::string TASKS_FILE = ".taskie/tasks.json";

// Prototipos de funciones
fs::path findTaskieRoot(fs::path dir = fs::current_path());
json load_tasks(const fs::path& root);
void save_tasks(const fs::path& root, const json& tasks);
void cmd_init();
void cmd_add(const std::vector<std::string>& args);
void cmd_list();
void cmd_done(const std::vector<std::string>& args);
void cmd_remove(const std::vector<std::string>& args);
void cmd_help();

// Busca la raíz del proyecto (.taskie/)
fs::path findTaskieRoot(fs::path dir) {
    while (!fs::exists(dir / TASKIE_DIR)) {
        if (dir.has_parent_path())
            dir = dir.parent_path();
        else
            return "";
    }
    return dir;
}

void cmd_init() {
    fs::path dir = fs::current_path() / TASKIE_DIR;
    if (!fs::exists(dir)) {
        fs::create_directory(dir);
        std::ofstream file(dir / "tasks.json");
        file << "[]";
        file.close();
        std::cout << "Inicializado en .taskie/\n";
    } else {
        std::cout << ".taskie ya existe.\n";
    }
}

json load_tasks(const fs::path& root) {
    std::ifstream file(root / TASKS_FILE);
    json tasks;
    file >> tasks;
    return tasks;
}

void save_tasks(const fs::path& root, const json& tasks) {
    std::ofstream file(root / TASKS_FILE);
    file << tasks.dump(2);
}

void cmd_add(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cerr << "Uso: taskie -a \"tarea\"\n";
        return;
    }

    fs::path root = findTaskieRoot();
    if (root.empty()) {
        std::cerr << "No se encontró .taskie. Usa 'taskie init' primero.\n";
        return;
    }

    json tasks = load_tasks(root);

    std::string desc;
    for (const auto& word : args) {
        desc += word + " ";
    }

    int next_id = tasks.empty() ? 1 : tasks.back()["id"].get<int>() + 1;

    tasks.push_back({
        {"id", next_id},
        {"done", false},
        {"desc", desc}
    });

    save_tasks(root, tasks);
    std::cout << "Tarea agregada con ID " << next_id << "\n";
}

void cmd_list() {
    fs::path root = findTaskieRoot();
    if (root.empty()) {
        std::cerr << "No se encontró .taskie. Usa 'taskie init' primero.\n";
        return;
    }

    json tasks = load_tasks(root);
    if (tasks.empty()) {
        std::cout << "No hay tareas.\n";
        return;
    }

    for (const auto& task : tasks) {
        std::cout << task["id"] << " ["
                  << (task["done"].get<bool>() ? "x" : " ") << "] "
                  << task["desc"] << "\n";
    }
}

void cmd_done(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        std::cerr << "Uso: taskie -d <id>\n";
        return;
    }

    int id = std::stoi(args[0]);
    fs::path root = findTaskieRoot();
    if (root.empty()) {
        std::cerr << "No se encontró .taskie.\n";
        return;
    }

    json tasks = load_tasks(root);
    bool found = false;

    for (auto& task : tasks) {
        if (task["id"] == id) {
            task["done"] = true;
            found = true;
            break;
        }
    }

    if (found) {
        save_tasks(root, tasks);
        std::cout << "Tarea marcada como completada.\n";
    } else {
        std::cerr << "No se encontró la tarea con ID " << id << "\n";
    }
}

void cmd_remove(const std::vector<std::string>& args) {
    if (args.size() != 1) {
        std::cerr << "Uso: taskie -r <id>\n";
        return;
    }

    int id = std::stoi(args[0]);
    fs::path root = findTaskieRoot();
    if (root.empty()) {
        std::cerr << "No se encontró .taskie.\n";
        return;
    }

    json tasks = load_tasks(root);
    size_t original_size = tasks.size();

    tasks.erase(std::remove_if(tasks.begin(), tasks.end(),
        [id](const json& task) { return task["id"] == id; }),
        tasks.end()
    );

    if (tasks.size() < original_size) {
        save_tasks(root, tasks);
        std::cout << "Tarea eliminada.\n";
    } else {
        std::cerr << "No se encontró la tarea con ID " << id << "\n";
    }
}

void cmd_help() {
    std::cout << "Comandos disponibles:\n";
    std::cout << "  init               Inicializa un nuevo proyecto .taskie\n";
    std::cout << "  -a \"tarea\"         Agrega una nueva tarea\n";
    std::cout << "  -l                 Muestra la lista de tareas\n";
    std::cout << "  -d <id>            Marca la tarea como completada\n";
    std::cout << "  -r <id>            Elimina una tarea por ID\n";
    std::cout << "  -h                 Muestra este mensaje\n\n\n";
    std::cout << "  Do not run \"taskie init\" on your personal folder" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Uso: taskie <comando o flag> [args...]\n";
        cmd_help();
        return 1;
    }

    std::string command = argv[1];
    std::vector<std::string> args(argv + 2, argv + argc);

    if (command == "init") {
        cmd_init();
    } else if (command == "-a") {
        cmd_add(args);
    } else if (command == "-l") {
        cmd_list();
    } else if (command == "-d") {
        cmd_done(args);
    } else if (command == "-r") {
        cmd_remove(args);
    } else if (command == "-h") {
        cmd_help();
    } else {
        std::cerr << "Comando desconocido: " << command << "\n";
        cmd_help();
    }

    return 0;
}
