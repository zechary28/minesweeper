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
    bool reveallable();
};