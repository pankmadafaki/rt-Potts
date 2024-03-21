
#include "../include/raylib-cpp.hpp"
#include <cmath>
// #include <cstddef>
// #include <cstdlib>
#include <iostream>
#include <ostream>
#include <random>
#include <sciplot/Plot2D.hpp>
#include <sciplot/Vec.hpp>
#include <sciplot/sciplot.hpp>
#include <vector>
#define RAYGUI_IMPLEMENTATION
#include "../include/raylib.h"
#include "../include/raygui.h"
#define RECS_WIDTH 3
#define RECS_HEIGHT 3
#define N_LEVELS_DEFAULT 1
#define MAX_RECS_X 1000 / RECS_WIDTH
#define MAX_RECS_Y 1000 / RECS_HEIGHT
#define PLAY_TIME_IN_FRAMES 240 // At 60 fps = 4 seconds
#define LOG_ITERATIONS 1000000
#define MEAN_ITERATIONS 500000
#define MAX_TEMP 6.0

int arrMean(int arr[MEAN_ITERATIONS]) {
  int sum = 0;
  for (int i = 0; i < MEAN_ITERATIONS; i++) {
    sum += arr[i];
    std::cout << arr[i];
  }
  return sum / MEAN_ITERATIONS;
}

double calculateMean(int arr[], int size) {
  double sum = 0;
  for (int i = 0; i < size; ++i) {
    sum += arr[i];
  }
  return sum / size;
}

double calculateStdDev(int arr[], int size) {
  double mean = calculateMean(arr, size);
  double sumSquaredDiff = 0;

  for (int i = 0; i < size; ++i) {
    sumSquaredDiff += pow(arr[i] - mean, 2);
  }
  return sqrt(sumSquaredDiff / size);
}

void print_int(std::vector<int> const &input) {
  for (int i = 0; i < input.size(); i++) {
    std::cout << input.at(i) << ' ';
  }
}
void print_float(std::vector<float> const &input) {
  for (int i = 0; i < input.size(); i++) {
    std::cout << input.at(i) << ' ';
  }
}

template <typename T, size_t N, size_t M> void printArray(T (&theArray)[N][M]) {
  for (int n = 0; n < N; n++) {
    for (int m = 0; m < M; m++) {
      std::cout << theArray[m][n] << " ";
    }
    std::cout << "\n";
  }
}

int magField(int spin, int field_q, int field_strength) {
  if (spin == field_q) {
    return field_strength;
  } else {
    return 0;
  }
}

int magnetization(int arr[MAX_RECS_X][MAX_RECS_Y]) {
  int mag = 0;
  for (int n = 0; n < MAX_RECS_X; n++) {
    for (int m = 0; m < MAX_RECS_Y; m++) {
      mag += arr[n][m];
    }
  }
  return mag;
}

raylib::Color get_color(signed int q) {
  raylib::Color c;
  if (q == 0) {
    return raylib::Color(255, 255, 255);
  } else if (q == 1) {
    return raylib::Color(94, 129, 172);
  } else if (q == 2) {
    return raylib::Color(228, 0, 102);
  } else if (q == 3) {
    return raylib::Color(3, 206, 164);
  } else if (q == 4) {
    return raylib::Color(251, 77, 61);
  } else if (q == 5) {
    return raylib::Color(136, 74, 178);
  } else if (q == 6) {
    return raylib::Color(255, 102, 255);
  } else if (q == 7) {
    return raylib::Color(0, 0, 0);
  } else if (q == 8) {
    return raylib::Color(234, 196, 53);
  }

  return c;
}

int get_energy(int arr[MAX_RECS_X][MAX_RECS_Y]) {
  int E = 0;
  for (int x = 0; x != MAX_RECS_X; ++x) {
    for (int y = 0; y != MAX_RECS_Y; ++y) {
      int spin = arr[x][y];
      if (x > 0) {
        E += (arr[x][y - 1] == spin) * 2 - 1;
      }
      if (x < MAX_RECS_X - 1) {
        E += (arr[x][y + 1] == spin) * 2 - 1;
      }
      if (y > 0) {
        E += (arr[x - 1][y] == spin) * 2 - 1;
      }
      if (y < MAX_RECS_Y - 1) {
        E += (arr[x + 1][y] == spin) * 2 - 1;
      }
    }
  }
  return E;
}

float mean(std::vector<int> vec) {
  float average = std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
  return average;
}

struct energy_result {
  int energy;
  int spin_initial;
  int spin_final;
};

energy_result nm_energy(int arr[MAX_RECS_X][MAX_RECS_Y], int x, int y,
                        int spin_f, bool includeDiag, int hStrength,
                        int hDirection) {
  int E_i = 0;
  int E_f = 0;
  int spin_i = arr[x][y];
  if (x > 0) {
    if (y > 0 && includeDiag) {
      E_i += (arr[x - 1][y - 1] == spin_i) * 2 - 1 +
             magField(arr[x - 1][y - 1], hDirection, hStrength);
      E_f += (arr[x - 1][y - 1] == spin_f) * 2 - 1 +
             magField(spin_f, hDirection, hStrength);
    }
    E_i += (arr[x - 1][y] == spin_i) * 2 - 1 +
           magField(arr[x - 1][y], hDirection, hStrength);
    E_f += (arr[x - 1][y] == spin_f) * 2 - 1 +
           magField(spin_f, hDirection, hStrength);
  }
  if (x < MAX_RECS_X - 1) {
    if (y < MAX_RECS_Y - 1 && includeDiag) {
      E_i += (arr[x + 1][y + 1] == spin_i) * 2 - 1 +
             magField(arr[x + 1][y + 1], hDirection, hStrength);
      E_f += (arr[x + 1][y + 1] == spin_f) * 2 - 1 +
             magField(spin_f, hDirection, hStrength);
    }
    E_i += (arr[x + 1][y] == spin_i) * 2 - 1 +
           magField(arr[x + 1][y], hDirection, hStrength);
    E_f += (arr[x + 1][y] == spin_f) * 2 - 1 +
           magField(spin_f, hDirection, hStrength);
  }
  if (y > 0) {
    if (x < MAX_RECS_X - 1 && includeDiag) {
      E_i += (arr[x + 1][y - 1] == spin_i) * 2 - 1 +
             magField(arr[x + 1][y - 1], hDirection, hStrength);
      E_f += (arr[x + 1][y - 1] == spin_f) * 2 - 1 +
             magField(spin_f, hDirection, hStrength);
    }

    E_i += (arr[x][y - 1] == spin_i) * 2 - 1 +
           magField(arr[x][y - 1], hDirection, hStrength);
    E_f += (arr[x][y - 1] == spin_f) * 2 - 1 +
           magField(spin_f, hDirection, hStrength);
  }
  if (y < MAX_RECS_Y - 1) {
    if (x > 0 && includeDiag) {
      E_i += (arr[x - 1][y + 1] == spin_i) * 2 - 1 +
             magField(arr[x - 1][y + 1], hDirection, hStrength);
      E_f += (arr[x - 1][y + 1] == spin_f) * 2 - 1 +
             magField(spin_f, hDirection, hStrength);
    }
    E_i += (arr[x][y + 1] == spin_i) * 2 - 1 +
           magField(arr[x][y + 1], hDirection, hStrength);
    E_f += (arr[x][y + 1] == spin_f) * 2 - 1 +
           magField(spin_f, hDirection, hStrength);
  }
  int dE = E_i - E_f;
  return energy_result{dE, spin_i, spin_f};
}

int main() {

  int test = 1500;

  // Initialization
  int screenWidth = 1000 + 400;
  int screenHeight = 1000;
  float n_levels = N_LEVELS_DEFAULT;
  std::random_device rd;
  std::mt19937 gen(rd()); // Standard mersenne_twister_engine seeded with rd()
  std::uniform_int_distribution<> uniform_color(
      0, 255); // For random color picking
  std::uniform_real_distribution<> uniform_float(
      0.0, 1.0); // For Metropolis algorithm
  std::uniform_int_distribution<> uniform_int(
      0, MAX_RECS_X - 1); // For lattice position selection
  std::uniform_int_distribution<> uniform_q(0, n_levels); // For spin selection
  raylib::Window w(screenWidth, screenHeight, "Ising model");
  GuiLoadStyle(
      "include/dark.rgs"); //
  Font fontTtf = LoadFontEx(
      "include/PixelOperator.ttf",
      32, 0, 250); //    Load
  // dark
  // style
  

  raylib::Rectangle recs[MAX_RECS_X * MAX_RECS_Y] = {0};
  // Lattice Initialization

  int lattice_p[MAX_RECS_X][MAX_RECS_Y];
  for (int i = 0; i < MAX_RECS_X; i++) {
    for (int j = 0; j < MAX_RECS_Y; j++) {
      lattice_p[i][j] = uniform_q(gen);
    }
  }
  std::cout << "\n\n\n";
  // Raylib square size specifications
  for (int y = 0; y < MAX_RECS_Y; y++) {
    for (int x = 0; x < MAX_RECS_X; x++) {
      recs[y * MAX_RECS_X + x].x = RECS_WIDTH / 2.0f + RECS_WIDTH * x;
      recs[y * MAX_RECS_X + x].y = RECS_HEIGHT / 2.0f + RECS_HEIGHT * y;
      recs[y * MAX_RECS_X + x].width = RECS_WIDTH;
      recs[y * MAX_RECS_X + x].height = RECS_HEIGHT;
    }
  }

  float BJ = MAX_TEMP; // Temperature
  std::vector<double> e_list;
  std::vector<float> t_list;
  std::vector<double> c_list;
  std::vector<float> m_list;
  std::vector<float> s_list;
  int framesCounter = 0;
  int moves = 0;
  int totalEnergy = get_energy(lattice_p);
  int totalMagnetization = magnetization(lattice_p);
  float fieldStrength = 0;
  float fieldDirection = 0;
  bool sweepTemp = false;
  bool diagonalElem = false;
  int sweepCounter = 0;
  double energies = totalEnergy;
  double magnetizations = totalMagnetization;
  int energiesArr[MEAN_ITERATIONS];
  int magnetizationsArr[MEAN_ITERATIONS];
  int area = MAX_RECS_X * MAX_RECS_Y;
  // Main simulation loop
  while (!w.ShouldClose()) // Detect window close button or ESC key
  {
    // Choose random stuff
    //----------------------------------------------------------------------------------
    moves++;

    std::uniform_int_distribution<> uniform_q(0,
                                              n_levels); // For spin selection
    framesCounter++;
    sweepCounter++;
    int x = uniform_int(gen);
    int y = uniform_int(gen);
    int q = uniform_q(gen);
    // Perform flip and get energy
    //----------------------------------------------------------------------------------
    auto [dE, spin_i, spin_f] =
        nm_energy(lattice_p, x, y, q, diagonalElem, int(fieldStrength),
                  int(fieldDirection)); // Return energy and spin to be set
    if (dE > 0.0 && uniform_float(gen) < exp(-dE / BJ)) {
      lattice_p[x][y] = spin_f;
      // If energy change is unfavourable then maybe change spin
      totalEnergy += dE;
      totalMagnetization -= spin_i;
      totalMagnetization += spin_f;
    } else if (dE <= 0.0) {
      lattice_p[x][y] =
          spin_f; // If energy change is favourable then definitely change spin
      totalEnergy += dE;
      totalMagnetization -= spin_i;
      totalMagnetization += spin_f;
    }
    // Draw every x steps
    //----------------------------------------------------------------------------------
    if (framesCounter > 100000) {
      BeginDrawing();
      ClearBackground(RAYWHITE);

      for (int i = 0; i < MAX_RECS_X; i++) {
        for (int j = 0; j < MAX_RECS_Y; j++) {
          DrawRectanglePro(recs[j * MAX_RECS_X + i],
                           (Vector2){recs[j * MAX_RECS_X + i].width / 2,
                                     recs[j * MAX_RECS_X + i].height / 2},
                           0.0f, get_color(lattice_p[i][j]));
        }
      }
      DrawRectangle(1000, 0, GetScreenWidth() - 400, GetScreenHeight(),
                    Fade(BLACK, 0.8f));
      GuiSliderBar((Rectangle){1090, 40, 220, 20}, "Temp", NULL, &BJ, 0.0,
                   MAX_TEMP);
      // DrawText(TextFormat("%f", BJ), 1330, 40, 15, WHITE);
      DrawTextEx(fontTtf, TextFormat("%.2f", BJ), {1320, 40}, 20, 0.2, WHITE);

      GuiSliderBar((Rectangle){1090, 80, 220, 20}, "q-states", NULL, &n_levels,
                   1, 8);
      DrawTextEx(fontTtf, TextFormat("%i", int(n_levels)), {1320, 80}, 20, 0.1,
                 WHITE);

      GuiCheckBox((Rectangle){1090, 120, 20, 20}, "Start Temp Sweep",
                  &sweepTemp);
      GuiCheckBox((Rectangle){1090, 160, 20, 20}, "Include Diagonal elements",
                  &diagonalElem);

      GuiSliderBar((Rectangle){1090, 200, 220, 20}, "H Magnitude", NULL,
                   &fieldStrength, 0.0, 2.0);
      // DrawText(TextFormat("%f", BJ), 1330, 40, 15, WHITE);
      DrawTextEx(fontTtf, TextFormat("%i", int(fieldStrength)), {1320, 200}, 20,
                 0.2, WHITE);
      GuiSliderBar((Rectangle){1090, 240, 220, 20}, "H direction", NULL,
                   &fieldDirection, 0, float(int(n_levels)));
      DrawTextEx(fontTtf, TextFormat("%i", int(fieldDirection)), {1320, 240},
                 20, 0.1, WHITE);
      DrawFPS(10, 10);
      EndDrawing();
      framesCounter = 0;
    }

    // Temperature sweep
    //----------------------------------------------------------------------------------
    if (sweepTemp) {
      if (sweepCounter > LOG_ITERATIONS - MEAN_ITERATIONS &&
          sweepCounter < LOG_ITERATIONS + 1) {

        energiesArr[sweepCounter - LOG_ITERATIONS + MEAN_ITERATIONS - 1] =
            totalEnergy;

        magnetizationsArr[sweepCounter - LOG_ITERATIONS + MEAN_ITERATIONS - 1] =
            totalMagnetization;

        energies += float(totalEnergy) / area;
        magnetizations += float(totalMagnetization) / area;
      } else if (sweepCounter > LOG_ITERATIONS && BJ > 0.2) {
        sweepCounter = 0;
        if (BJ != MAX_TEMP) {
          double stdDev = calculateStdDev(energiesArr, MEAN_ITERATIONS);
          c_list.push_back(stdDev * stdDev * pow(1 / BJ, 2));
          stdDev = calculateStdDev(magnetizationsArr, MEAN_ITERATIONS);
          s_list.push_back(stdDev * stdDev * (1 / BJ));
          std::cout << pow(1 / BJ, 2) << " ";
          e_list.push_back(energies / MEAN_ITERATIONS);
          t_list.push_back(BJ);
          m_list.push_back(magnetizations / MEAN_ITERATIONS);
        }
        energies = 0;
        magnetizations = 0;
        BJ -= 0.05;
      } else if (sweepCounter > LOG_ITERATIONS && BJ < 0.2) {
        BJ = MAX_TEMP;
        sweepTemp = false;
        sciplot::Plot2D plot_energy;
        sciplot::Plot2D plot_capacity;
        sciplot::Plot2D plot_magnetization;
        sciplot::Plot2D plot_susceptibility;
        plot_energy.xlabel("Temperature");
        plot_energy.ylabel("Energy");
        plot_energy.legend()
            .atOutsideBottom()
            .displayHorizontal()
            .displayExpandWidthBy(2);
        plot_capacity.xlabel("Temperature");
        plot_capacity.ylabel("Heat Capacity");
        plot_capacity.legend()
            .atOutsideBottom()
            .displayHorizontal()
            .displayExpandWidthBy(2);
        plot_magnetization.xlabel("Temperature");
        plot_magnetization.ylabel("Magnetization");
        plot_magnetization.legend()
            .atOutsideBottom()
            .displayHorizontal()
            .displayExpandWidthBy(2);
        plot_susceptibility.xlabel("Temperature");
        plot_susceptibility.ylabel("Susceptibility");
        plot_susceptibility.legend()
            .atOutsideBottom()
            .displayHorizontal()
            .displayExpandWidthBy(2);
        plot_capacity.drawPoints(t_list, c_list)
            .label("Heat Capacity")
            .lineColor("red");
        plot_energy.drawCurve(t_list, e_list)
            .lineColor("red")
            .label("Temperature Energy dependence");
        plot_magnetization.drawPoints(t_list, m_list)
            .label("Magnetization Energy dependence")
            .lineColor("blue");
        plot_susceptibility.drawPoints(t_list, s_list)
            .label("Susceptibility")
            .lineColor("blue");
        sciplot::Figure fig = {{plot_energy, plot_magnetization},
                               {plot_capacity, plot_susceptibility}};
        fig.title(
            TextFormat("Potts model simulation for q = %i", int(n_levels + 1)));

        // Create canvas to hold figure
        sciplot::Canvas canvas = {{fig}};
        canvas.size(1100, 1100);
        canvas.show();
        canvas.cleanup();
        print_float(s_list);
        e_list.clear();
        t_list.clear();
        m_list.clear();
      }
    }
    // Logging
    //----------------------------------------------------------------------------------
    if (moves % LOG_ITERATIONS == 0) {
      std::cout << moves << " " << BJ << "\n";
    }
  }
  return 0;
}
