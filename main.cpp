#include <iostream>
#include <vector>

using namespace std;
static vector<int> DEFAULT_VECTOR;

struct ALSP_representation {
    float **planes_separation_matrix;
    int **planes_domain;
    vector<int> domains_size;
    int planes_qty;
};

struct PlaneCandidate {
    int plane;
    int time;
}


bool accept(ALSP_representation ALSP, vector<int> evaluating_solution) {
    if (evaluating_solution.size() == ALSP.planes_qty) return true;
    else return false;
}

int JumpBack(ALSP_representation ALSP, vector<int> evaluating_solution) {
    evaluating_solution.size() 
}

bool ComparePlanesTime(PlaneCandidate p1, PlaneCandidate p2) {
    return (p1.time < p2.time);
}

int NextInstance(ALSP_representation ALSP, vector<int> evaluating_solution, int domain_pointer) {
    variable_pointer = evaluating_solution.size();
    if (evaluating_solution.size() < ALSP.planes_qty) {
        // añadimos avion
        PlaneCandidate new_plane{variable_pointer, ALSP.planes_domain[variable_pointer][domain_pointer]};
        evaluating_solution.push_back(new_plane);
        sort(evaluating_solution.begin(), evaluating_solution.end(), ComparePlanesTime);
        int breaking_point = -1;
        // chequeo de restricciones
        for(int i = 1; i <= variable_pointer; i++) {
            before = evaluating_solution[i-1];
            after = evaluating_solution[i];
            if (after.time before.time > ALSP.planes_separation_matrix[before.plane][after.plane]) {
                breaking_point = i;
                break;
            }
        }
        // si hay restricciones vamos para atras
        if (breaking_point != -1) {
            evaluating_solution.erase(evaluating_solution.begin() + breaking_point);
            return JumpBack();
        }
    }
    return domain_pointer;
}

vector<vector<int>> Solve(ALSP_representation ALSP, vector<vector<int>> solutions) {
    /*
        Recibe una matriz de dominio y una de la separacion minima entre los aviones.
        Las filas representan los valores del avion i
        En el caso de los dominios, las columnas son los dominios
        En el caso de la matrix de separacion las columnas es que tan separado debe estar de
        el avion j(columna).

        Realiza back tracking para resolver el problema ALSP.
    */
    vector<PlaneCandidate> evaluating_solution;
    int s = nextInstance(ALSP, solutions, evaluating_solution, 0);
    while s != -1 {    
        if (reject(ALSP, evaluating_solution)) s = JumpBack();
        else {
            if (accept(ALSP, evaluating_solution)) solutions.push_back(evaluating_solution);
            s = nextInstance(ALSP, solutions, evaluating_solution, s);
        }
    }
    return DEFAULT_VECTOR;
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
        for(int j = 0; j < planes_domain_len[i]; i++)
            planes_domain[i][j] = planes_earliest_t[i] + j;
    }
    ALSP_representation ALSP;
    ALSP.planes_qty = planes_qty;
    ALSP.planes_separation_matrix = planes_separation_matrix;
    ALSP.planes_domain = planes_domain;
    ALSP.domains_size = planes_domain_len;
    vector<vector<int>> solutions;
    // algoritmo
    Solve(ALSP,solutions);
    // limpiar heap
    for (int i = 0; i < planes_qty; i++)
        delete [] planes_separation_matrix[i];
    delete [] planes_separation_matrix;
    return 0;
}