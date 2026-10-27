#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <iomanip>

struct Rectangle
{
    int ni, pi, qi;
    int amplada() const { return std::max(pi, qi); }
    int alçada() const { return std::min(pi, qi); }
    int area() const { return pi * qi; }
};

bool comparaRectangles(const Rectangle &a, const Rectangle &b)
{
    return a.area() > b.area();
}

struct RectangleColocat
{
    int x1, y1, x2, y2;
};

bool cap(const std::vector<RectangleColocat> &rectanglesColocats, int ampladaRotlle, int alçadaRotlle, int x, int y, int amplada, int alçada)
{
    if (x + amplada > ampladaRotlle || y + alçada > alçadaRotlle)
        return false;
    for (const auto &rect : rectanglesColocats)
    {
        if (x < rect.x2 + 1 && x + amplada > rect.x1 && y < rect.y2 + 1 && y + alçada > rect.y1)
        {
            return false;
        }
    }
    return true;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Ús: " << argv[0] << " <fitxer_entrada> <fitxer_sortida>" << std::endl;
        return 1;
    }

    std::ifstream entrada(argv[1]);
    if (!entrada)
    {
        std::cerr << "Error obrint fitxer: " << argv[1] << std::endl;
        return 1;
    }

    std::ofstream sortida(argv[2]);
    if (!sortida)
    {
        std::cerr << "Error obrint fitxer: " << argv[2] << std::endl;
        return 1;
    }

    int ampladaRotlle, N;
    entrada >> ampladaRotlle >> N;

    std::vector<Rectangle> rectangles(N);
    for (int i = 0; i < N; ++i)
    {
        entrada >> rectangles[i].ni >> rectangles[i].pi >> rectangles[i].qi;
    }

    auto inici = std::chrono::high_resolution_clock::now();

    // Ordenem els rectangles per àrea
    std::sort(rectangles.begin(), rectangles.end(), comparaRectangles);

    int alçadaRotlle = 0;

    std::vector<RectangleColocat> rectanglesColocats;

    for (const auto &rect : rectangles)
    {
        for (int comptador = 0; comptador < rect.ni; ++comptador)
        {
            int ampladaRect = rect.amplada();
            int alçadaRect = rect.alçada();

            bool col·locat = false;

            // Provem de col·locar el rectangle en l'orientació que menys espai ocupi
            for (int y = 0; y <= alçadaRotlle - 1; ++y)
            {
                for (int x = 0; x <= ampladaRotlle - 1; ++x)
                {
                    if (cap(rectanglesColocats, ampladaRotlle, alçadaRotlle, x, y, ampladaRect, alçadaRect))
                    {
                        rectanglesColocats.push_back({x, y, x + ampladaRect - 1, y + alçadaRect - 1});
                        col·locat = true;
                        break;
                    }
                    else if (cap(rectanglesColocats, ampladaRotlle, alçadaRotlle, x, y, alçadaRect, ampladaRect))
                    {
                        std::swap(ampladaRect, alçadaRect);
                        rectanglesColocats.push_back({x, y, x + ampladaRect - 1, y + alçadaRect - 1});
                        col·locat = true;
                        break;
                    }
                }
                if (col·locat)
                    break;
            }

            if (!col·locat)
            {
                // Movem el rectangle a la següent fila, en l'orientació que ocupi menys espai
                if (alçadaRect > ampladaRect)
                {
                    if (alçadaRect < ampladaRotlle)
                    {
                        alçadaRotlle += ampladaRect;
                        rectanglesColocats.push_back({0, alçadaRotlle - ampladaRect, alçadaRect - 1, alçadaRotlle - 1});
                        col·locat = true;
                    }
                    else
                    {
                        alçadaRotlle += alçadaRect;
                        rectanglesColocats.push_back({0, alçadaRotlle - alçadaRect, ampladaRect - 1, alçadaRotlle - 1});
                        col·locat = true;
                    }
                }
                else if (ampladaRect >= alçadaRect)
                {
                    if (ampladaRect < ampladaRotlle)
                    {
                        alçadaRotlle += alçadaRect;
                        rectanglesColocats.push_back({0, alçadaRotlle - alçadaRect, ampladaRect - 1, alçadaRotlle - 1});
                        col·locat = true;
                    }
                    else
                    {
                        alçadaRotlle += ampladaRect;
                        rectanglesColocats.push_back({0, alçadaRotlle - ampladaRect, alçadaRect - 1, alçadaRotlle - 1});
                        col·locat = true;
                    }
                }
            }
        }
    }

    auto final = std::chrono::high_resolution_clock::now();
    double temps = std::chrono::duration<double>(final - inici).count();

    sortida << std::fixed << std::setprecision(1) << temps << std::endl;

    sortida << alçadaRotlle << std::endl;

    for (const auto &rectColocat : rectanglesColocats)
    {
        sortida << rectColocat.x1 << " " << rectColocat.y1 << "    "
                << rectColocat.x2 << " " << rectColocat.y2 << std::endl;
    }

    entrada.close();
    sortida.close();

    return 0;
}