/*
Camping game solver.

The game input is:
    A number N.
    A column requirements vector of size N
    A row requirements vector of size N
    An NxN matrix of initial camp status where every tyle is Tree or Grass.

Final state requires:
    Every column i has as many tents as required in column requirements i.
    Every row i has as many tents as required in row requirements i.
    Every tent is adyacent to a tree.
    No two tents are adyacent or diagonally next to each other.
    Every tree covers at most one tent. A tree may have more than one adyacent tent if that one is covered by another tree.


Algorithm:
    1) Map tyles grass tyles with an adyacent tree to potencial tents.
    2) If a row/column has as many required tents as potential tents, place tents in them.
    3) Find row/col with lower  (potentials - req) difference. Place one of the posible tents. Try to solve from step 2. if contradiction, try other posible tent an repeat.

    Where place a tent means:
    - Change Tyle to tent
    - Change surrounding spaces to grass, unless tree.
    - if col/row req full, change other potential tents in col/row to grass.
 */

#include <iostream>
#include <fstream>
#include <stack> 
using namespace std;

enum class Tyle {Grass, Tree, Tent, PotentialTent};

uint read_initial_state(Tyle ***tented_camp, uint **column_reqs, uint **row_reqs) {
    ifstream file_input;
    uint size;
    uint *_column_reqs, *_row_reqs;
    Tyle **camp;
    file_input.open("camp.txt");
    file_input >> size;
    _column_reqs = new uint[size];
    _row_reqs = new uint[size];
    camp = new Tyle*[size];
    for(uint i = 0; i < size; i++)
        camp[i] = new Tyle[size];

    for(uint i = 0; i < size; i++) {
        file_input >> _column_reqs[i];
    }
    for(uint i = 0; i < size; i++) {
        file_input >> _row_reqs[i];
        cout << _row_reqs[i] << "\n";
        for(uint j = 0; j < size; j++) {
            char t;
            file_input >> t;
            switch (t)
            {
            case 'g':
                camp[i][j] = Tyle::Grass;
                break;
            case 't':
                camp[i][j] = Tyle::Tree;
                break;
            default:
                throw "Non recognized tile";
                break;
            }
        }
    }
    *tented_camp = camp;
    *column_reqs = _column_reqs;
    *row_reqs = _row_reqs;
    return size;
}

bool potential_camp_site(Tyle **empty_camp, uint size, uint x, uint y) {
    if(empty_camp[x][y] == Tyle::Tree)
        return false;
    if((x > 0 && empty_camp[x - 1][y] == Tyle::Tree)
        || (x < size - 1 && empty_camp[x + 1][y] == Tyle::Tree)
        || (y > 0 && empty_camp[x][y - 1] == Tyle::Tree)
        || (y < size - 1 && empty_camp[x][y + 1] == Tyle::Tree))
        return true;
    else
        return false;
}

void init_tented_camp(Tyle ***tented_camp, Tyle **empty_camp, uint size) {

    Tyle** camp = new Tyle*[size];
    for(uint i = 0; i < size; i++)
        camp[i] = new Tyle[size];

    for(uint i = 0; i < size; i++) {
        for(uint j = 0; j < size; j++) {
            if(potential_camp_site(empty_camp, size, i, j))
                camp[i][j] = Tyle::PotentialTent;
            else
                camp[i][j] = empty_camp[i][j];
        }
    }
    *tented_camp = camp;
}

struct Pos {
    uint x;
    uint y;
};

bool find_next_tyle_to_place_tent(Tyle **tented_camp, uint *column_reqs, uint *row_reqs, uint size, Pos *next) {
    uint counter;
    Pos pos;
    
    // Find an obvios row
    for(uint i = 0; i < size; i++) {
        counter = 0;
        for(uint j = 0; j < size; j++) {
            if(tented_camp[i][j] == Tyle::PotentialTent) {
                pos = Pos{i, j};
                counter++;
            }
        }
        if(row_reqs[i] > 0 && counter == row_reqs[i]) {
            *next = pos;
            row_reqs[i]--;
            return true;
        }
    }

    // Find an obvios column
    for(uint i = 0; i < size; i++) {
        counter = 0;
        for(uint j = 0; j < size; j++) {
            if(tented_camp[j][i] == Tyle::PotentialTent) {
                pos = Pos{j, i};
                counter++;
            }
        }
        if(column_reqs[i] > 0 && counter == column_reqs[i]) {
            *next = pos;
            column_reqs[i]--;
            return true;
        }
    }
    return false;
}

void place_tent(Tyle **camp, uint size, Pos pos) {
    uint x = pos.x, y = pos.y;
    camp[x][y] = Tyle::Tent;
    // Adyacents
    if(x > 0 && camp[x - 1][y] != Tyle::Tree)
        camp[x - 1][y] = Tyle::Grass;
    if(x < size - 1 && camp[x + 1][y] != Tyle::Tree)
        camp[x + 1][y] = Tyle::Grass;
    if(y > 0 && camp[x][y - 1] != Tyle::Tree)
        camp[x][y - 1] = Tyle::Grass;
    if(y < size - 1 && camp[x][y + 1] != Tyle::Tree)
        camp[x][y + 1] = Tyle::Grass;
    
    // Diagonals
    if(x > 0 && y > 0 && camp[x - 1][y - 1] != Tyle::Tree)
        camp[x - 1][y - 1] = Tyle::Grass;
    if(x < size - 1  && y > 0 && camp[x + 1][y - 1] != Tyle::Tree)
        camp[x + 1][y - 1] = Tyle::Grass;
    if(x > 0 && y < size - 1 && camp[x - 1][y + 1] != Tyle::Tree)
        camp[x - 1][y + 1] = Tyle::Grass;
    if(x < size - 1 && y < size - 1 && camp[x + 1][y + 1] != Tyle::Tree)
        camp[x + 1][y + 1] = Tyle::Grass;

    // if req full, grass all potential on col/row.
}

char tyle_to_char(Tyle t) {
    switch (t)
    {
    case Tyle::Grass:
        return 'G';
        break;
    case Tyle::Tree:
        return 'T';
        break;
    case Tyle::Tent:
        return 'A';
        break;
    // Shouldn't happen
    case Tyle::PotentialTent:
        return 'P';
        break;
    
    default:
        throw "Unexpected tyle type";
        break;
    }
}

void print_camp(Tyle **camp, uint size) {
    for(uint i = 0; i < size; i++) {
        for(uint j = 0; j < size; j++) {
            cout << tyle_to_char(camp[i][j]) << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

Tyle** clone_camp(Tyle** camp, uint size) {
    Tyle** new_camp = new Tyle*[size];
    for(uint i = 0; i < size; i++) {
        new_camp[i] = new Tyle[size];
        for(uint j = 0; j < size; j++) {
            new_camp[i][j] = camp[i][j];
        }
    }
    return new_camp;
}

uint* clone_req(uint* req, uint size) {
    uint* new_req = new uint[size];
    for(uint i = 0; i < size; i++) {
        new_req[i] = req[i];
    }
    return new_req;
}

bool solver(Tyle **camp, uint *column_reqs, uint *row_reqs, uint size) {
    Pos next;
    // Solve all posible on this state
    while(find_next_tyle_to_place_tent(camp, column_reqs, row_reqs, size, &next)) {
        place_tent(camp, size, next);
    }
    // Check for contradiction, return false


    //Supose a tent and try to solve
    Tyle **posible_camp = clone_camp(camp, size);
    uint *posible_col_req = clone_req(column_reqs, size), *posible_row_req = clone_req(row_reqs, size);
    //Find a row or column with 1 req and 2 potential tents. Probably new function.

    // Call solver on new camp
    // true -> Delete old camp, poin camp to this suposed camp.
    // false -> Try the other option
}

int main() {
    uint size;
    uint *column_reqs, *row_reqs;
    Tyle **empty_camp, **tented_camp;
    size = read_initial_state(&empty_camp, &column_reqs, &row_reqs);
    print_camp(empty_camp, size);
    init_tented_camp(&tented_camp, empty_camp, size);
    print_camp(tented_camp, size);
    solver(tented_camp, column_reqs, row_reqs, size);
    print_camp(tented_camp, size);

    delete[] column_reqs;
    delete[] row_reqs;
    for(uint i; i < size; i++) {
        delete[] empty_camp[i];
        delete[] tented_camp[i];
    }
    delete[] empty_camp;
    delete[] tented_camp;
}
