#include <iostream>
#include <vector>
#include <ctime>
#include <climits>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <csignal>

using namespace std;


struct Rectangle
{
    int ample, alt; 
};

struct Colocacio
{
    int x1, y1, x2, y2; // Coordenades del rectangle (superior-esquerra i inferior-dreta)
};

// Variables globals
vector<Rectangle> llista_rectangles; 
int amplada_rotlle, millor_alcada;   
vector<Colocacio> millor_configuracio; 
clock_t temps_inici_programa;       
string nom_fitxer_sortida;          


void escriure_resultat(const vector<Colocacio> &configuracio, int alzada)
{
    ofstream fitxer_sortida(nom_fitxer_sortida, ios::out | ios::trunc);

    if (!fitxer_sortida.is_open())
    {
        cerr << "Error obrint el fitxer de sortida: " << nom_fitxer_sortida << endl;
        return;
    }

    double temps_transcorregut = double(clock() - temps_inici_programa) / CLOCKS_PER_SEC;
    fitxer_sortida << fixed << setprecision(1) << temps_transcorregut << endl;

    fitxer_sortida << alzada << endl;
    for (const auto &c : configuracio)
    {
        fitxer_sortida << c.x1 << " " << c.y1 << " " << c.x2 << " " << c.y2 << endl;
    }

    fitxer_sortida.close();
}


bool pot_colocar(const vector<vector<bool>> &graella, int x, int y, int ample, int alt)
{
    if (x + ample > amplada_rotlle || y + alt > (int)graella[0].size())
        return false;

    for (int i = x; i < x + ample; ++i)
    {
        for (int j = y; j < y + alt; ++j)
        {
            if (graella[i][j])
                return false;
        }
    }
    return true;
}


void marcar_graella(vector<vector<bool>> &graella, int x, int y, int ample, int alt, bool estat)
{
    for (int i = x; i < x + ample; ++i)
    {
        for (int j = y; j < y + alt; ++j)
        {
            graella[i][j] = estat;
        }
    }
}

// Troba la y més baixa per col·locar un rectangle
int obtenir_y_minima(const vector<vector<bool>> &graella, int x, int ample, int alt, int alzada_actual)
{
    for (int y = 0; y <= alzada_actual; ++y)
    {
        if (pot_colocar(graella, x, y, ample, alt))
            return y;
    }
    return -1;
}


void processar_input(const string &nom_fitxer)
{
    ifstream entrada(nom_fitxer);
    if (!entrada)
    {
        cerr << "Error obrint el fitxer: " << nom_fitxer << endl;
        exit(1);
    }

    millor_alcada = INT_MAX;

    int num_rectangles;
    entrada >> amplada_rotlle >> num_rectangles;

    string linia;
    getline(entrada, linia);
    while (getline(entrada, linia))
    {
        istringstream iss(linia);
        int quantitat, ample, alt;
        if (iss >> quantitat >> ample >> alt)
        {
            for (int i = 0; i < quantitat; ++i)
            {
                llista_rectangles.push_back({ample, alt});
            }
        }
        else
        {
            cerr << "Error analitzant la línia: " << linia << endl;
        }
    }

    entrada.close();

    sort(llista_rectangles.begin(), llista_rectangles.end(),
         [](const Rectangle &a, const Rectangle &b)
         {
             return (a.ample * a.alt) > (b.ample * b.alt);
         });
}


void buscar_solucio(int index, int alzada_actual, vector<vector<bool>> &graella, vector<Colocacio> &configuracio)
{
    if (alzada_actual >= millor_alcada)
        return;

    if (index == (int)llista_rectangles.size())
    {
        millor_alcada = alzada_actual;
        millor_configuracio = configuracio;

        // Escrivim la millor solució trobada fins ara
        escriure_resultat(millor_configuracio, millor_alcada);
        return;
    }

    Rectangle rectangle = llista_rectangles[index];
    for (int x = 0; x < amplada_rotlle; ++x)
    {
        int y = obtenir_y_minima(graella, x, rectangle.ample, rectangle.alt, alzada_actual);
        if (y != -1)
        {
            marcar_graella(graella, x, y, rectangle.ample, rectangle.alt, true);
            configuracio.push_back({x, y, x + rectangle.ample - 1, y + rectangle.alt - 1});
            int nova_alzada = max(alzada_actual, y + rectangle.alt);

            buscar_solucio(index + 1, nova_alzada, graella, configuracio);

            marcar_graella(graella, x, y, rectangle.ample, rectangle.alt, false);
            configuracio.pop_back();
        }

        if (rectangle.ample != rectangle.alt)
        {
            int y_rotada = obtenir_y_minima(graella, x, rectangle.alt, rectangle.ample, alzada_actual);
            if (y_rotada != -1)
            {
                marcar_graella(graella, x, y_rotada, rectangle.alt, rectangle.ample, true);
                configuracio.push_back({x, y_rotada, x + rectangle.alt - 1, y_rotada + rectangle.ample - 1});
                int nova_alzada = max(alzada_actual, y_rotada + rectangle.ample);

                buscar_solucio(index + 1, nova_alzada, graella, configuracio);

                marcar_graella(graella, x, y_rotada, rectangle.alt, rectangle.ample, false);
                configuracio.pop_back();
            }
        }
    }
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Ús: " << argv[0] << " <fitxer_entrada> <fitxer_sortida>" << endl;
        return 1;
    }

    nom_fitxer_sortida = argv[2];
    temps_inici_programa = clock();

    processar_input(argv[1]);

    int max_alzada = 0;
    for (auto &r : llista_rectangles)
        max_alzada += max(r.ample, r.alt);

    vector<vector<bool>> graella(amplada_rotlle, vector<bool>(max_alzada, false));
    vector<Colocacio> configuracio_actual;

    buscar_solucio(0, 0, graella, configuracio_actual);

    return 0;
}
