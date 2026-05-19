#include <vector>
#include <cstdint>

class GameOfLife
{
public:
    GameOfLife(int width, int height, int cellSize);

    void Update();
    void Draw() const;
    void Randomize(float probability);
    void ToggleCell(int x, int y);

private:
    int width;
    int height;
    int cellSize;

    std::vector<uint8_t> grid;
    std::vector<uint8_t> next;

    int index(int x, int y) const;
    int aliveNeighbours(int x, int y) const;
};
