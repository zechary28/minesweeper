#include "cell.h"
//#include <raylib.h>

Cell::Cell(bool hasMine)
    : hasMine(hasMine), revealed(false), flagged(false), visited(false)
{

}

void Cell::Mark()
{
    revealed = true;
}

void Cell::Flag()
{
    flagged = !flagged;
}

bool Cell::floodable()
{
    return !visited && !flagged && !hasMine;
}
