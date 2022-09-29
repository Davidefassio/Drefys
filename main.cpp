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

int main(int argc, char *argv[]){/*
    PlutoCE* engine = new PlutoCE();
	Board* board = new Board();
	engine->fillStartingPos(board);

    char str[5];

    while(1){
        engine->displayBoard(board);

        int eval = engine->evaluate(board);
        std::cout << "Eval: " << eval << std::endl << std::endl;

        bool check = engine->isCheck(board, true);
        std::cout << ((check)?"Check":"No checks") << std::endl;

        std::cout << "Make a move: ";
        std::cin >> str;

        if(str[0] == 'q'){ break;}

        engine->makeMove(board, str);
    }

    delete board;
    delete engine;

	return 0;*/


    constexpr int pst[120] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,80,70,70,80,80,70,70,80,0,0,90,90,90,90,90,90,90,90,0,0,70,60,60,70,70,60,60,70,0,0,60,50,50,60,60,50,50,60,0,0,50,40,40,50,50,40,40,50,0,0,40,30,30,40,40,30,30,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    for(int i = 0; i < 12; ++i){
        for(int j = 0; j < 10; ++j){
            std::cout << std::setw(3) << pst[i*10 + j] << " ";
        }
        std::cout << std::endl;
    }
}
