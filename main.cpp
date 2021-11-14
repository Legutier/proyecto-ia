#include <iostream>
#include <vector>

using namespace std;

int main () {
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
    for (int i = 0; i < planes_qty; i++)
        delete [] planes_separation_matrix[i];
    delete [] planes_separation_matrix;
    return 0;
}
