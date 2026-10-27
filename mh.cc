#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <fstream>
#include <chrono>

using namespace std;
using namespace chrono;

struct Rectangle {
    int amplada, alcada, quantitat, area;

    Rectangle(int a, int al, int q) : amplada(a), alcada(al), quantitat(q), area(a * al) {}

    bool operator<(const Rectangle& altre) const {
        return area > altre.area; // Ordre descendent per àrea
    }
};

struct Posicionament {
    int x1, y1, x2, y2;
};

struct Solucio {
    int llargadaRollo;
    vector<Posicionament> posicionaments;
};

double calculaCost(const Solucio& sol, int A) {
    double cost = sol.llargadaRollo;

    for (size_t i = 0; i < sol.posicionaments.size(); ++i) {
        const auto& p1 = sol.posicionaments[i];

        // Comprovar superposicions
        for (size_t j = i + 1; j < sol.posicionaments.size(); ++j) {
            const auto& p2 = sol.posicionaments[j];
            if (!(p1.x2 < p2.x1 || p2.x2 < p1.x1 || p1.y2 < p2.y1 || p2.y2 < p1.y1)) {
                cost += 1000; // Penalització alta per superposició
            }
        }

        if (p1.x1 < 0 || p1.y1 < 0 || p1.x2 >= A || p1.y2 >= sol.llargadaRollo) {
            cost += 1000; // Penalització alta per fora de límits
        }
    }

    return cost;
}

bool esSolucioValida(const Solucio& sol, int A, int alcadaMax, const vector<Rectangle>& rectangles) {
    vector<vector<bool>> graella(A, vector<bool>(alcadaMax, false));

    for (const auto& posicionament : sol.posicionaments) {
        if (posicionament.x1 < 0 || posicionament.y1 < 0 || posicionament.x2 >= A || posicionament.y2 >= alcadaMax) {
            return false;
        }

        for (int x = posicionament.x1; x <= posicionament.x2; ++x) {
            for (int y = posicionament.y1; y <= posicionament.y2; ++y) {
                if (graella[x][y]) {
                    return false; 
                }
                graella[x][y] = true;
            }
        }
    }

    map<pair<int, int>, int> mapaQuantitats;
    for (const auto& posicionament : sol.posicionaments) {
        int amplada = posicionament.x2 - posicionament.x1 + 1;
        int alcada = posicionament.y2 - posicionament.y1 + 1;
        mapaQuantitats[{amplada, alcada}]++;
    }

    for (const auto& rect : rectangles) {
        if (mapaQuantitats[{rect.amplada, rect.alcada}] != rect.quantitat) {
            return false;
        }
    }

    return true;
}

Solucio generaSolucioInicial(int A, const vector<Rectangle>& rectangles, int alcadaMax) {
    Solucio sol;
    sol.llargadaRollo = 0;

    vector<int> alcades(A, 0);

    for (const auto& rect : rectangles) {
        for (int i = 0; i < rect.quantitat; ++i) {
            bool col·locat = false;

            for (int x = 0; x <= A - rect.amplada && !col·locat; ++x) {
                int alcadaMaxBaix = 0;
                for (int dx = 0; dx < rect.amplada; ++dx) {
                    alcadaMaxBaix = max(alcadaMaxBaix, alcades[x + dx]);
                }

                if (alcadaMaxBaix + rect.alcada <= alcadaMax) {
                    for (int dx = 0; dx < rect.amplada; ++dx) {
                        alcades[x + dx] = alcadaMaxBaix + rect.alcada;
                    }

                    sol.posicionaments.push_back({x, alcadaMaxBaix, x + rect.amplada - 1, alcadaMaxBaix + rect.alcada - 1});
                    sol.llargadaRollo = max(sol.llargadaRollo, alcadaMaxBaix + rect.alcada);
                    col·locat = true;
                }
            }

            if (!col·locat) {
                cerr << "Error: No es pot col·locar el rectangle a la solució inicial." << endl;
                exit(1);
            }
        }
    }

    return sol;
}

Solucio generaVei(const Solucio& actual, int A, int alcadaMax) {
    Solucio vei = actual;

    int numPerturbacions = 1 + rand() % 5;
    for (int p = 0; p < numPerturbacions; ++p) {
        int idx = rand() % vei.posicionaments.size();

        auto& posicionament = vei.posicionaments[idx];
        int ampladaRect = posicionament.x2 - posicionament.x1 + 1;
        int alcadaRect = posicionament.y2 - posicionament.y1 + 1;

        bool col·locat = false;
        while (!col·locat) {
            posicionament.x1 = rand() % A;
            posicionament.y1 = rand() % alcadaMax;
            posicionament.x2 = posicionament.x1 + ampladaRect - 1;
            posicionament.y2 = posicionament.y1 + alcadaRect - 1;

            if (posicionament.x2 >= A || posicionament.y2 >= alcadaMax) continue;

            bool valid = true;
            for (size_t i = 0; i < vei.posicionaments.size(); ++i) {
                if (i == idx) continue;
                const auto& altre = vei.posicionaments[i];
                if (!(posicionament.x2 < altre.x1 || altre.x2 < posicionament.x1 || posicionament.y2 < altre.y1 || altre.y2 < posicionament.y1)) {
                    valid = false;
                    break;
                }
            }

            if (valid) {
                col·locat = true;
            }
        }
    }

    vei.llargadaRollo = 0;
    for (const auto& posicionament : vei.posicionaments) {
        vei.llargadaRollo = max(vei.llargadaRollo, posicionament.y2 + 1);
    }

    return vei;
}

void escriuMillorSolucioAFitxer(const Solucio& millor, const string& nomFitxerSortida, double tempsExecucio) {
    ofstream fitxerSortida(nomFitxerSortida);
    if (!fitxerSortida.is_open()) {
        cerr << "Error: No s'ha pogut obrir el fitxer de sortida per escriure la millor solució." << endl;
        return;
    }

    fitxerSortida << tempsExecucio << endl;

    fitxerSortida << millor.llargadaRollo << endl;
    for (const auto& posicionament : millor.posicionaments) {
        fitxerSortida << posicionament.x1 << " " << posicionament.y1 << " " << posicionament.x2 << " " << posicionament.y2 << endl;
    }

    fitxerSortida.close();
}

Solucio recuitSimulat(int A, const vector<Rectangle>& rectangles, double temperaturaInicial, double taxaRefredament, int maxIteracions, int maxVeins, int alcadaMax, const string& nomFitxerSortida) {
    auto tempsInici = high_resolution_clock::now();

    Solucio actual = generaSolucioInicial(A, rectangles, alcadaMax);
    Solucio millor = actual;

    // Escriure la solució inicial
    escriuMillorSolucioAFitxer(millor, nomFitxerSortida, 0.0);

    double temperatura = temperaturaInicial;

    for (int iter = 0; iter < maxIteracions; ++iter) {
        for (int veins = 0; veins < maxVeins; ++veins) {
            Solucio vei = generaVei(actual, A, alcadaMax);

            if (!esSolucioValida(vei, A, alcadaMax, rectangles)) continue;

            double delta = calculaCost(vei, A) - calculaCost(actual, A);

            if (delta < 0 || exp(-delta / temperatura) > ((double)rand() / RAND_MAX)) {
                actual = vei;
            }

            if (calculaCost(actual, A) < calculaCost(millor, A)) {
                millor = actual;

                auto tempsActual = high_resolution_clock::now();
                duration<double> tempsExec = tempsActual - tempsInici;
                escriuMillorSolucioAFitxer(millor, nomFitxerSortida, tempsExec.count());
            }
        }

        temperatura *= taxaRefredament;
        if (temperatura < 1e-3) break;
    }

    auto tempsFinal = high_resolution_clock::now();
    duration<double> tempsExec = tempsFinal - tempsInici;

    escriuMillorSolucioAFitxer(millor, nomFitxerSortida, tempsExec.count());
    return millor;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cerr << "Ús: " << argv[0] << " <fitxer_entrada> <fitxer_sortida>" << endl;
        return 1;
    }

    auto tempsInici = high_resolution_clock::now();

    ifstream fitxerEntrada(argv[1]);
    if (!fitxerEntrada.is_open()) {
        cerr << "Error: No s'ha pogut obrir el fitxer d'entrada." << endl;
        return 1;
    }

    int A, N;
    fitxerEntrada >> A >> N;

    map<pair<int, int>, int> comptadorRectangles;
    for (int i = 0; i < N;) {
        int ni, pi, qi;
        fitxerEntrada >> ni >> pi >> qi;
        comptadorRectangles[{pi, qi}] += ni;
        i += ni;
    }

    vector<Rectangle> rectangles;
    int totalRectangles = 0;
    int alcadaMax = 0;

    for (const auto& entrada : comptadorRectangles) {
        int pi = entrada.first.first;
        int qi = entrada.first.second;
        int quantitat = entrada.second;
        if (qi > pi && qi <= A) {
            swap(pi, qi);
        }
        rectangles.emplace_back(pi, qi, quantitat);
        alcadaMax += qi * quantitat;
        totalRectangles += quantitat;
    }

    sort(rectangles.begin(), rectangles.end());

    fitxerEntrada.close();

    srand(time(0));

    double temperaturaInicial = 1000.0;
    double taxaRefredament = 0.99;
    int maxIteracions = 10000;
    int maxVeins = 500;

    recuitSimulat(A, rectangles, temperaturaInicial, taxaRefredament, maxIteracions, maxVeins, alcadaMax, argv[2]);

    auto tempsFinal = high_resolution_clock::now();
    duration<double> tempsExec = tempsFinal - tempsInici;

    return 0;
}
