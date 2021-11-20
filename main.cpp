#include <iostream>
#include <vector>
#include <tuple>
#include <algorithm>
#include <set>

using namespace std;

struct ALSP_representation {
    float **planes_separation_matrix;
    int **planes_domain;
    vector<int> domains_size;
    int planes_qty;
};

struct PlaneCandidate {
    int plane;
    int time;
    int domain_pointer;
};

static vector< vector<PlaneCandidate> > DEFAULT_VECTOR;

bool accept(ALSP_representation ALSP, vector<PlaneCandidate> evaluating_solution) {
    if (evaluating_solution.size() == ALSP.planes_qty) return true;
    else return false;
}

bool ComparePlanesTime(PlaneCandidate p1, PlaneCandidate p2) {
    return (p1.time < p2.time);
}


bool ComparePlanes(PlaneCandidate p1, PlaneCandidate p2) {
  return (p1.plane < p2.plane);
}


vector<vector<PlaneCandidate> > Solve(ALSP_representation ALSP) {
    /*
        Recibe una matriz de dominio y una de la separacion minima entre los aviones.
        Las filas representan los valores del avion i
        En el caso de los dominios, las columnas son los dominios
        En el caso de la matrix de separacion las columnas es que tan separado debe estar de
        el avion j(columna).

        Realiza back tracking para resolver el problema ALSP.
    */
    vector<vector<PlaneCandidate> > solutions;
    vector<PlaneCandidate> evaluating_solution;
    int domain_pointer = 0;
    int saltos = 0;
    set<int> conflicts;

    while (domain_pointer != -1) {
        if (accept(ALSP, evaluating_solution)) solutions.push_back(evaluating_solution);
        int variable_pointer = evaluating_solution.size();
        if (evaluating_solution.size() < ALSP.planes_qty && domain_pointer < ALSP.domains_size[variable_pointer]) {
            // si no, instanciamos
            PlaneCandidate new_plane = {variable_pointer, ALSP.planes_domain[variable_pointer][domain_pointer], domain_pointer};
            evaluating_solution.push_back(new_plane);
            sort(evaluating_solution.begin(), evaluating_solution.end(), ComparePlanesTime);
            int breaking_point = -1;
            // chequeo de restricciones
            for(int i = 1; i <= variable_pointer; i++) {
                PlaneCandidate before = evaluating_solution[i-1];
                PlaneCandidate after = evaluating_solution[i];
                if (after.time - before.time < ALSP.planes_separation_matrix[before.plane][after.plane]) {
                    if (before.plane > after.plane){
                        breaking_point =  i - 1;
                        conflicts.insert(after.plane);
                    }
                    else {
                        breaking_point = i;
                        conflicts.insert(before.plane);
                    }
                    cout << "conflictoo ";
                    set<int, greater<int> >::iterator itr;
                    for (itr = conflicts.begin(); itr != conflicts.end(); itr++)
                    {
                        cout << *itr<<" ";
                    }
                    cout << "\n";
                    break;
                }
            }
            // si hay restriccion sacamos la variable para escoger la siguiente
            if (breaking_point != -1) {
                evaluating_solution.erase(evaluating_solution.begin() + breaking_point);
                // vamos a la siguiente variable
                domain_pointer++;
            }
            else {
                cout << "limpiando\n";
                conflicts.clear();
                domain_pointer = 0;
            }
        }
        else {
            // si no tenemos opciones volvemos atras
            if (evaluating_solution.size() == 0){
                break;
            }
            sort(evaluating_solution.begin(), evaluating_solution.end(), ComparePlanes);
            saltos++;
            if(!conflicts.empty() && true) {
                int premature_conflict = *conflicts.rbegin();
                conflicts.clear();
                int erases = evaluating_solution.size() - premature_conflict;
                cout << "borrados " << erases << " conflicto " << premature_conflict << " otro " << *conflicts.begin() << "\n";
                for(int i = 0; i < erases; i++) {
                    PlaneCandidate last_plane = evaluating_solution.back();
                    domain_pointer = last_plane.domain_pointer + 1;
                    evaluating_solution.pop_back();
                }
            }
            else {
                    PlaneCandidate last_plane = evaluating_solution.back();
                    domain_pointer = last_plane.domain_pointer + 1;
                    evaluating_solution.pop_back();
            }
        }
        cout << "Estado actual\n";
        for (int i = 0; i < evaluating_solution.size(); i++) cout << evaluating_solution[i].plane << " ";
        cout << "\n";
        for (int i = 0; i < evaluating_solution.size(); i++) cout << evaluating_solution[i].time << " ";
        cout << "\n";
    }
    cout << "Saltos = " << saltos << "\n";
    return solutions;
}

int main() {
    // Leer todo
    int planes_qty;
    cin >> planes_qty;
    vector<int> planes_earliest_t(planes_qty);
    vector<int> planes_ideal_t(planes_qty);
    vector<int> planes_latest_t(planes_qty);
    vector<float> planes_early_penalty(planes_qty);
    vector<float> planes_late_penalty(planes_qty);
    float** planes_separation_matrix = new float*[planes_qty];
    for (int i = 0; i < planes_qty; i ++) {
        cin >> planes_earliest_t[i];
        cin >> planes_ideal_t[i];
        cin >> planes_latest_t[i];
        cin >> planes_early_penalty[i];
        cin >> planes_late_penalty[i];
        planes_separation_matrix[i] = new float[planes_qty];
        for (int j = 0; j < planes_qty; j++)
            cin >> planes_separation_matrix[i][j];
        cout << planes_earliest_t[i] << " " << planes_ideal_t[i] << " ";
        cout << planes_latest_t[i] << " " << planes_early_penalty[i] << " ";
        cout << planes_late_penalty[i] << "\n";
        for (int j = 0; j < planes_qty; j++)
            cout << planes_separation_matrix[i][j] << " ";
        cout << "\n";
    }
    vector<int> planes_domain_len(planes_qty);
    int ** planes_domain = new int*[planes_qty];
    for(int i = 0; i < planes_qty; i++) {
        planes_domain_len[i] = planes_latest_t[i] - planes_earliest_t[i] + 1;
        planes_domain[i] = new int[planes_domain_len[i]];
        for(int j = 0; j < planes_domain_len[i]; j++)
            planes_domain[i][j] = planes_earliest_t[i] + j;
    }
    ALSP_representation ALSP;
    ALSP.planes_qty = planes_qty;
    ALSP.planes_separation_matrix = planes_separation_matrix;
    ALSP.planes_domain = planes_domain;
    ALSP.domains_size = planes_domain_len;
    cout << ALSP.planes_qty << "\n";
    for(int i = 0; i < ALSP.planes_qty; i++) {
      for(int j = 0; j < ALSP.planes_qty; j ++) cout << ALSP.planes_separation_matrix[i][j] << " ";
      cout << "\n";
    }
    // algoritmo
    vector< vector<PlaneCandidate> > solutions = Solve(ALSP);
    for(int i=0; i < solutions.size(); i++) {
        for(int j=0; j < solutions[i].size(); j++) cout << solutions[i][j].time << ' ';
        cout << "\n";
    }
    // limpiar heap
    for (int i = 0; i < planes_qty; i++)
        delete [] planes_separation_matrix[i];
    delete [] planes_separation_matrix;
    return 0;
}
