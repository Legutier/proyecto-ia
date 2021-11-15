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

bool reject(ALSP_representation ALSP, vector <int> evaluating_solution) {
    return true;
}


bool accept(ALSP_representation ALSP, vector<int> evaluating_solution) {
    return true;
}

int get(ALSP_representation ALSP, vector<int> evaluating_solution, int variable_pointer) {
    return -1;
}

vector<int> solve(ALSP_representation ALSP, vector<vector<int>> solutions, vector<int> evaluating_solution = DEFAULT_VECTOR, int variable_pointer = 0) {
    /*
        Recibe una matriz de dominio y una de la separacion minima entre los aviones.
        Las filas representan los valores del avion i
        En el caso de los dominios, las columnas son los dominios
        En el caso de la matrix de separacion las columnas es que tan separado debe estar de
        el avion j(columna).

        Realiza back tracking para resolver el problema ALSP.
    */
    if (reject(ALSP, evaluating_solution)) return vector<int>(ALSP.planes_qty);
    if (accept(ALSP, evaluating_solution)) {
        solutions.push_back(evaluating_solution);
        evaluating_solution.clear();
    }
    int s = 0;
    while (s != -1) {
        solve(ALSP, solutions, evaluating_solution, 0);
        s = get(ALSP, evaluating_solution, s);
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
    solve(ALSP,solutions);
    // limpiar heap
    for (int i = 0; i < planes_qty; i++)
        delete [] planes_separation_matrix[i];
    delete [] planes_separation_matrix;
    return 0;
}