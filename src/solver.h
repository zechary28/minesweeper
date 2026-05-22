class Solver
{
public:
    Solver(Minesweeper& game);
    void Step();

private:
    Minesweeper& game;
};