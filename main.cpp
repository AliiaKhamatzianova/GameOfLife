#include <SDL2/SDL.h>
#include <vector>
#include <thread>
#include <chrono>
#include <cstdlib>
#include <string>

const int CELL_SIZE = 10;

class GameOfLife {
public:
    GameOfLife(int width, int height);
    void ToggleCell(int x, int y);
    void Update();
    void Draw(SDL_Renderer* renderer);
    void Clear();

private:
    int GetIndex(int x, int y) const;
    int CountNeighbors(int x, int y) const;

    int m_width, m_height;
    std::vector<bool> m_grid;
};

GameOfLife::GameOfLife(int width, int height)
    : m_width(width), m_height(height), m_grid(width * height, false) {}

int GameOfLife::GetIndex(int x, int y) const {
    return (y + m_height) % m_height * m_width + (x + m_width) % m_width;
}

int GameOfLife::CountNeighbors(int x, int y) const {
    int count = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (dx == 0 && dy == 0) continue;
            int nx = x + dx;
            int ny = y + dy;
            count += m_grid[GetIndex(nx, ny)];
        }
    }
    return count;
}

void GameOfLife::Update() {
    std::vector<bool> newGrid(m_width * m_height);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            int idx = GetIndex(x, y);
            int neighbors = CountNeighbors(x, y);
            bool alive = m_grid[idx];
            if (alive && (neighbors == 2 || neighbors == 3)) {
                newGrid[idx] = true;
            } else if (!alive && neighbors == 3) {
                newGrid[idx] = true;
            } else {
                newGrid[idx] = false;
            }
        }
    }
    m_grid = std::move(newGrid);
}

void GameOfLife::Draw(SDL_Renderer* renderer) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    for (int y = 0; y < m_height; ++y) {
        for (int x = 0; x < m_width; ++x) {
            if (m_grid[GetIndex(x, y)]) {
                SDL_Rect rect = {x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE};
                SDL_RenderFillRect(renderer, &rect);
            }
        }
    }
}

void GameOfLife::ToggleCell(int x, int y) {
    x /= CELL_SIZE;
    y /= CELL_SIZE;
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        m_grid[GetIndex(x, y)] = !m_grid[GetIndex(x, y)];
    }
}

void GameOfLife::Clear() {
    std::fill(m_grid.begin(), m_grid.end(), false);
}

int main(int argc, char* argv[]) {
    int gridWidth = 80;
    int gridHeight = 60;
    int fps = 10;

    // Чтение параметров командной строки
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "--width" && i + 1 < argc) {
            gridWidth = std::atoi(argv[++i]);
        } else if (std::string(argv[i]) == "--height" && i + 1 < argc) {
            gridHeight = std::atoi(argv[++i]);
        } else if (std::string(argv[i]) == "--speed" && i + 1 < argc) {
            fps = std::atoi(argv[++i]);
        }
    }

    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("Game of Life", SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          gridWidth * CELL_SIZE, gridHeight * CELL_SIZE,
                                          SDL_WINDOW_SHOWN);

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    GameOfLife game(gridWidth, gridHeight);
    bool running = true;
    bool paused = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                game.ToggleCell(x, y);
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_SPACE) {
                    paused = !paused;
                } else if (event.key.keysym.sym == SDLK_c) {
                    game.Clear();
                    paused = true;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderClear(renderer);

        if (!paused) {
            game.Update();
        }

        game.Draw(renderer);
        SDL_RenderPresent(renderer);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / fps));
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
