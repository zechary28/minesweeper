#include <array>
#include <utility>

class Cell 
{
public:
    Cell(bool hasMine);
    bool hasMine;
    bool revealed;
    bool flagged;
    bool visited;
    int adjacentMines = 0;
    void Mark();
    void Flag();
    bool floodable();
};

struct NeighbourList {
    std::array<std::pair<int,int>, 8> cells;
    int count = 0;

    // lets you range-for over it naturally
    auto begin() { return cells.begin(); }
    auto end()   { return cells.begin() + count; }
    auto begin() const { return cells.begin(); }
    auto end()   const { return cells.begin() + count; }
};