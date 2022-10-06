/*
 * TODO:
 *   - pseudo-legal move generation
 *   - move ordering
 *   - tree search (negamax)
 *   - alpha-beta pruning (aspirarion windows)
 *   - quiescence search
 *   - iterative deepening
 *   - transposition table
 */

#include <iostream>
#include <string>
#include <iomanip>

#include "drefys.hpp"

int main(int argc, char *argv[]){
	Board board = Board();
	Drefys::fillStartingPos(board);
	std::string str;
	Move m = Move(0, 0, 0);

    while(1){
        Drefys::displayBoard(board);

        std::cout << "Make a move: ";
        std::cin >> str;

        if(str[0] == 'q'){ break;}

        Drefys::doMoveUser(board, str);
    }


	return 0;
}
