#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <set>
#include <limits>
#include <fstream>
#include <string>
#include <time.h>
#include <signal.h>
#include <cmath>

using namespace std;

// abstraccion del problema
struct ALSP_representation {
    string file_name;
    float **planes_separation_matrix;
    vector<int> planes_earliest_t;
    vector<int> planes_ideal_t;
    vector<int> planes_latest_t;
    vector<float> planes_early_penalty;
    vector<float> planes_late_penalty;
    unsigned int planes_qty;
};

struct PlaneCandidate {
    int plane;
    int time;
    int domain_pointer;
};

// flags que nos ayudan a medir el tiempo de ejecucion y finalizar el programa
clock_t tStart = clock();
int FINISH_FLAG = 0;

void signal_callback_handler(int signum) {
   cout << "finalizando..." <<  endl;
   cout << "codigo: " << signum << endl;
   // se activa variable de finalizacion
   FINISH_FLAG = 1;
}

int accept(ALSP_representation ALSP, vector<PlaneCandidate> evaluating_solution, int best_solution_val) {
    /*
    *   Si todos los aviones ya han sido instanciados
    *   se evalua su costo  
    *   retorna candidate_cost si el costo es optimo, si no retorna -1
    */
    int candidate_cost = 0;
    if (evaluating_solution.size() == ALSP.planes_qty) {
        for (unsigned int i = 0; i < evaluating_solution.size(); i++)  {
            int arrival_time = evaluating_solution[i].time - ALSP.planes_ideal_t[evaluating_solution[i].plane];
            if (arrival_time < 0) candidate_cost += -arrival_time * ALSP.planes_early_penalty[evaluating_solution[i].plane];
            else if(arrival_time > 0) candidate_cost += arrival_time * ALSP.planes_late_penalty[evaluating_solution[i].plane];
        }
        if (candidate_cost < best_solution_val) return candidate_cost;
    }
    return -1;
}


void Finish(vector<PlaneCandidate> solution, ALSP_representation ALSP, int cost) {
    /*
    *   al llamar genera el archivo solucion_n.txt
    *   toma la solucion mas optima encontrada actualmente y
    *   la imprime
    */
    string delimiter = ".";
    cout << ALSP.file_name << "\n";
    string token = ALSP.file_name.substr(0, ALSP.file_name.find(delimiter));
    char value = token.back();
    cout << token << "\n";
    string solution_name = "solucion_";
    solution_name.push_back(value);
    solution_name.append(".txt");
    ofstream MyFile(solution_name);
    MyFile << "Costo " << cost << "\n";\
    MyFile << "Tiempo total de ejecución " << (double)(clock() - tStart)/CLOCKS_PER_SEC << "[s]\n";
    for(unsigned int i = 0; i < solution.size(); i ++) {
        MyFile << "Tiempo avion " << solution[i].plane << ": " << solution[i].time << "\n";
    }

    MyFile.close();
    return;
}

vector<PlaneCandidate> Solve(ALSP_representation ALSP) {
    /*
    *   Recibe una abstraccion del problema ALSP y retorna su solucion,
    *   si se usa CTRL + C y aun esta corriendo toma la solucion optima
    *   encontrada hasta ahora y la retorna mediante finish al archivo
    *   correspondiente
    *   Realiza back tracking para resolver el problema ALSP.
    */

    // inicializacion de variables
    vector<PlaneCandidate> solution;
    int best_solution_val = numeric_limits<int>::max();
    vector<PlaneCandidate> evaluating_solution;
    int domain_pointer = 0;
    int s;
    vector<bool> touched_optimal(ALSP.planes_qty, false);
    set<int> conflicts;

    // espero CTRL + C
    signal(SIGINT, signal_callback_handler);

    // mientras pueda seguir instanciando y no han dado CTRL + C...
    while (domain_pointer != -1 && FINISH_FLAG == 0) {
        // revisamos que solucion que hayamos encontrada es optima
        s = accept(ALSP, evaluating_solution, best_solution_val);
        if (s != -1) {
            best_solution_val = s;
            solution = evaluating_solution;
        }
        // backtracking, inicializacion de variable que instancia nuevo avion
        // y el valor en el que debe ser instanciado
        int variable_pointer = evaluating_solution.size();
        int domain_value = ALSP.planes_earliest_t[variable_pointer] + domain_pointer;
        // si me quedan variables por instanciar y el valor de la variable actual
        // es menor al limite del dominio
        if (
            evaluating_solution.size() < ALSP.planes_qty
            && domain_value <= ALSP.planes_latest_t[variable_pointer]
        ) {
            // instanciamos nueva variable para solucion
            PlaneCandidate new_plane = {variable_pointer, domain_value, domain_pointer};
            bool found_conflict = false;
            // buscamos que cumpla restricciones con otros aviones
            for (unsigned int i = 0; i < evaluating_solution.size(); i++) {
                float separation_restriction;
                if (new_plane.time > evaluating_solution[i].time) {
                    separation_restriction = ALSP.planes_separation_matrix[evaluating_solution[i].plane][new_plane.plane]; 
                } else {
                    separation_restriction = ALSP.planes_separation_matrix[new_plane.plane][evaluating_solution[i].plane];
                }
                if (abs(new_plane.time - evaluating_solution[i].time) < separation_restriction) {
                    // si no cumplo, pues hay conflicto, dado que se revisa en orden
                    // evidentemente es la variable mas prematura y anhado a conflictos
                    conflicts.insert(evaluating_solution[i].plane);
                    found_conflict = true;
                    domain_pointer++;
                    break;
                }
            }
            // si no encontre conflicto puedo seguir, limpio los conflictos dado que si pude instanciar
            if (!found_conflict) {
                if (new_plane.time == ALSP.planes_ideal_t[new_plane.plane]) touched_optimal[new_plane.plane] = true;
                evaluating_solution.push_back(new_plane);
                conflicts.clear();
                domain_pointer = 0;
            }
        }
        else {                
            int jumps = 0;
            // si tenemos toque optimo guardamos saltos para heuristica
            if (evaluating_solution.size() == ALSP.planes_qty) {
                for (unsigned int i = touched_optimal.size() - 1; i != 0; i --) {
                    if (!touched_optimal[i]) {
                        if(evaluating_solution[i].time > ALSP.planes_ideal_t[i]) jumps ++;
                        break;
                    }
                    jumps ++; 
                }
                fill(touched_optimal.begin(), touched_optimal.end(), false);
            }

            // si saltamos hacia atras y no quedan variables por instanciar
            // significa que ya recorri todo el arbol, por ende se termina el loop
            if (evaluating_solution.size() == 0){
                break;
            }
            // si hay conflictos hacemos CBJ, sacamos la instancia de conflicto
            // mas reciente y borramos
            if(!conflicts.empty() && true) {
                int premature_conflict = *conflicts.rbegin();
                conflicts.clear();
                int erases = evaluating_solution.size() - premature_conflict;
                for(int i = 0; i < erases; i++) {
                    PlaneCandidate last_plane = evaluating_solution.back();
                    domain_pointer = last_plane.domain_pointer + 1;
                    evaluating_solution.pop_back();
                }
            }
            // si no, estamos en el caso que tengo una solucion completa
            // pero debo seguir buscando soluciones para el optimo
            // por ende o reinstanciamos al siguiente valor, o vamos atras
            // o puede darse el caso de la heuristica
            else {
                    // si se puede usar la heuristica
                    // usamos la heuristica para evitar instancia inutil
                    if (jumps > 0) {
                        for(int i = 0; i < jumps; i++) {
                            PlaneCandidate last_plane = evaluating_solution.back();
                            domain_pointer = last_plane.domain_pointer + 1;
                            evaluating_solution.pop_back();
                        }
                    }
                    // dado que salta n - 1 veces, nos evitamos un else
                    // asi que este implica el salto final de la heuristica o,
                    // es solo la reinstanciacion / volver atras
                    PlaneCandidate last_plane = evaluating_solution.back();
                    domain_pointer = last_plane.domain_pointer + 1;
                    evaluating_solution.pop_back();
            }
        }
    }
    Finish(solution, ALSP, best_solution_val);
    return solution;
}


int main(int argc, char *argv[]) {
    // Leer todo
    (void)argc;
    int planes_qty;
    cin >> planes_qty;
    // instancio variables para leer
    vector<int> planes_earliest_t(planes_qty);
    vector<int> planes_ideal_t(planes_qty);
    vector<int> planes_latest_t(planes_qty);
    vector<float> planes_early_penalty(planes_qty);
    vector<float> planes_late_penalty(planes_qty);
    float** planes_separation_matrix = new float*[planes_qty];
    // leo las variables
    for (int i = 0; i < planes_qty; i ++) {
        cin >> planes_earliest_t[i];
        cin >> planes_ideal_t[i];
        cin >> planes_latest_t[i];
        cin >> planes_early_penalty[i];
        cin >> planes_late_penalty[i];
        planes_separation_matrix[i] = new float[planes_qty];
        for (int j = 0; j < planes_qty; j++)
            cin >> planes_separation_matrix[i][j];
    }
    // genero abstraccion del problema
    ALSP_representation ALSP;
    ALSP.planes_qty = planes_qty;
    ALSP.planes_earliest_t = planes_earliest_t;
    ALSP.planes_ideal_t = planes_ideal_t;
    ALSP.planes_latest_t = planes_latest_t;
    ALSP.planes_early_penalty = planes_early_penalty;
    ALSP.planes_late_penalty = planes_late_penalty;
    ALSP.planes_separation_matrix = planes_separation_matrix;
    ALSP.file_name = argv[1];
    // algoritmo
    vector<PlaneCandidate> solution = Solve(ALSP);
    for(unsigned int j = 0; j < solution.size(); j++) cout << solution[j].time << ' ';
    // limpiar heap
    for (int i = 0; i < planes_qty; i++)
        delete [] planes_separation_matrix[i];
    delete [] planes_separation_matrix;
    return 0;
}
