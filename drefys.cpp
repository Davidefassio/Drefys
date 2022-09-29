#include "drefys.hpp"
/*
// Check operating system
// 0: not defined
// 1: Mac OS
// 2: Windows
// 3: Linux
#if defined(__APPLE__)
    #define OS 1
#elif defined(_WIN32)
    #define OS 2
#elif defined(__linux__)
    #define OS 3
#else
    #define OS 0
#endif

#define INB(x) (((x) & 0xFFFFFFF8) == 0 )  // Macro for in bound check

#define EMPTY       0
#define WHITE_PAWN  1
#define WHITE_BISH  2
#define WHITE_KNIG  3
#define WHITE_ROOK  4
#define WHITE_QUEE  5
#define WHITE_KING  6
#define BLACK_PAWN  8
#define BLACK_BISH  9
#define BLACK_KNIG 10
#define BLACK_ROOK 11
#define BLACK_QUEE 12
#define BLACK_KING 13


Board::Board(){
    return;
}

Board::Board(const Board &b){
    memcpy(this->position, b.position, 64);
    this->flags = b.flags;
    this->enpassant = b.enpassant;
    this->lastzeroing = b.lastzeroing;
    this->movevalue = b.movevalue;
}

bool Board::operator<(const Board &b) const{
    return this->movevalue < b.movevalue;
}

// Setup the board in the starting position
void PlutoCE::fillFEN(Board* b, std::string fen){
    int r = 0, f = 0, i;
    // Board position
    for(i = 0; fen[i] != ' '; ++i){
        if(fen[i] == '/'){
            r += 1;
            f = 0;
            continue;
        }
        else if(fen[i] > 47 && fen[i] < 58){ // Number
            for(int j = 0; j < fen[i] - 48; ++j, ++f)
                b->position[r][f] = EMPTY;
        }
        else{  // Character
            switch(fen[i]){
                case 'P': b->position[r][f] = WHITE_PAWN; break;
				case 'B': b->position[r][f] = WHITE_BISH; break;
				case 'N': b->position[r][f] = WHITE_KNIG; break;
				case 'R': b->position[r][f] = WHITE_ROOK; break;
				case 'Q': b->position[r][f] = WHITE_QUEE; break;
				case 'K': b->position[r][f] = WHITE_KING; break;
				case 'p': b->position[r][f] = BLACK_PAWN; break;
				case 'b': b->position[r][f] = BLACK_BISH; break;
				case 'n': b->position[r][f] = BLACK_KNIG; break;
				case 'r': b->position[r][f] = BLACK_ROOK; break;
				case 'q': b->position[r][f] = BLACK_QUEE; break;
				case 'k': b->position[r][f] = BLACK_KING; break;

				default: // Wrong piece code
					std::cout << "Wrong piece in the FEN!" << std::endl;
					exit(1);
					break;
            }
            ++f;
        }
    }
    if(r != 7 || f != 8){
        std::cout << "Board not fully specified!" << std::endl;
        exit(1);
    }

    // Player to move
    ++i;
    if(fen[i] == 'w')
        b->flags = 0x80;
    else if(fen[i] == 'b')
        b->flags = 0x00;
    else{
        std::cout << "Wrong move char in the FEN!" << std::endl;
        exit(1);
    }

    // Castling rights
    i += 2;
    if(fen[i] != '-'){
        for(; fen[i] != ' '; ++i){
            switch(fen[i]){
                case 'K':
                    if(b->position[7][4] == WHITE_KING && b->position[7][7] == WHITE_ROOK)
                        b->flags |= 0x01;
                    break;
                case 'Q':
                    if(b->position[7][4] == WHITE_KING && b->position[7][0] == WHITE_ROOK)
                        b->flags |= 0x02;
                    break;
                case 'k':
                    if(b->position[0][4] == BLACK_KING && b->position[0][7] == BLACK_ROOK)
                        b->flags |= 0x04;
                    break;
                case 'q':
                    if(b->position[0][4] == BLACK_KING && b->position[0][0] == BLACK_ROOK)
                        b->flags |= 0x08;
                    break;

                default: // Wrong castling code
					std::cout << "Wrong castling in the FEN!" << std::endl;
					exit(1);
					break;
            }
        }
    }
    else{
        ++i;
    }

    // Enpassant
    ++i;
    if(fen[i] == '-'){
        b->enpassant = -1;
    }
    else{
        f = fen[i] - 97;
        r = 56 - fen[++i];

        if((r & 0x7) != r || (f & 0x7) != f){
            std::cout << "Indexes of enpassant out of range!" << std::endl;
            exit(1);
        }

        b->enpassant = ((r << 4) | f) & 0x000000FF;
    }

    // Distance in ply to last zeroing move
    std::string ply;
    for(i += 2; fen[i] != ' '; ++i)
        ply.push_back(fen[i]);

    b->lastzeroing = std::stol(ply, NULL);
    b->movevalue = 0;
}

// Setup the board in the starting position
void PlutoCE::fillStartingPos(Board* b){
	this->fillFEN(b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

// Setup an empty board.
// The rights to castle are also set.
void PlutoCE::fillEmpty(Board* b){
	for(int i = 0; i < 8; ++i)
        for(int j = 0; j < 8; ++j)
            b->position[i][j] = EMPTY;

	b->flags = 0x8F;  // Set all rights to castle (F) and white to move (8)
	b->lastzeroing = 0;
	b->enpassant = -1;
	b->movevalue = 0;
}

// Save the game state in a FEN string
std::string PlutoCE::exportFEN(Board* b){
    std::string fen = "";

    // Board position
    int cnt= 0;
    for(int r = 0; r < 8; ++r){
        for(int f = 0; f < 8; ++f){
            if(b->position[r][f] == EMPTY){
                ++cnt;
                continue;
            }
            if(cnt > 0){
                fen.push_back(cnt + 48);
                cnt = 0;
            }
            switch(b->position[r][f]){
                case WHITE_PAWN: fen.push_back('P'); break;
				case WHITE_BISH: fen.push_back('B'); break;
				case WHITE_KNIG: fen.push_back('N'); break;
				case WHITE_ROOK: fen.push_back('R'); break;
				case WHITE_QUEE: fen.push_back('Q'); break;
				case WHITE_KING: fen.push_back('K'); break;
				case BLACK_PAWN: fen.push_back('p'); break;
				case BLACK_BISH: fen.push_back('b'); break;
				case BLACK_KNIG: fen.push_back('n'); break;
				case BLACK_ROOK: fen.push_back('r'); break;
				case BLACK_QUEE: fen.push_back('q'); break;
				case BLACK_KING: fen.push_back('k'); break;
            }
        }
        if(cnt > 0){
            fen.push_back(cnt + 48);
            cnt = 0;
        }
        fen.push_back('/');
    }
    fen.pop_back();
    fen.push_back(' ');

    // Turn to move
    fen += ((b->flags >> 7) ? "w " : "b ");

    // Castling rights
    if((b->flags << 1) == 0){
        fen.push_back('-');
    }
    else{
        if( b->flags & 0x01) fen.push_back('K');
        if((b->flags & 0x02)>>1) fen.push_back('Q');
        if((b->flags & 0x04)>>2) fen.push_back('k');
        if((b->flags & 0x08)>>3) fen.push_back('q');
    }
    fen.push_back(' ');

    // Enpassant
    if((b->enpassant & 0x80) != 0 || (b->enpassant & 0x08) != 0)
        fen.push_back('-');
    else{
        fen.push_back(97 + (b->enpassant & 0x0F));
        fen.push_back(56 - (b->enpassant >> 4));
    }
    fen.push_back(' ');

    // Last zeroing and move counter
    fen += std::to_string(b->lastzeroing);
    fen += " 1";  // Always to 1 because who cares?

    return fen;
}

// Display the board to stdout
void PlutoCE::displayBoard(Board* b){
	for(int i = 0; i < 8; ++i){
		for(int j = 0; j < 8; ++j){
			switch(b->position[i][j]){
				case WHITE_PAWN: std::cout << "P "; break;
				case WHITE_BISH: std::cout << "B "; break;
				case WHITE_KNIG: std::cout << "N "; break;
				case WHITE_ROOK: std::cout << "R "; break;
				case WHITE_QUEE: std::cout << "Q "; break;
				case WHITE_KING: std::cout << "K "; break;
				case BLACK_PAWN: std::cout << "p "; break;
				case BLACK_BISH: std::cout << "b "; break;
				case BLACK_KNIG: std::cout << "n "; break;
				case BLACK_ROOK: std::cout << "r "; break;
				case BLACK_QUEE: std::cout << "q "; break;
				case BLACK_KING: std::cout << "k "; break;
				case EMPTY:      std::cout << ". "; break;

				default: // Wrong piece code
					std::cout << std::endl << "Error at: " << i << ", " << j << std::endl;
					exit(1);
					break;
			}
		}
		std::cout << std::endl;
	}
}

// Display the board + some debug info
void PlutoCE::debugBoard(Board* b){
    this->displayBoard(b);

    std::cout << ((b->flags >> 7) ? "White to move" : "Black to move") << std::endl;
    std::cout << "White castling: " << ((b->flags & 0x02)>>1) << " - " << (b->flags & 0x01) << std::endl;
    std::cout << "Black castling: " << ((b->flags & 0x08)>>3) << " - " << ((b->flags & 0x04)>>2) << std::endl;

    std::cout << "Enpassant: ";
    if((b->enpassant & 0x80) != 0 || (b->enpassant & 0x08) != 0)
        std::cout << "-" << std::endl;
    else
        std::cout << (char) (97 + (b->enpassant & 0x0F)) << (8 - (b->enpassant >> 4)) << std::endl;

    std::cout << "Moves since last zeroing: " << (int) b->lastzeroing << std::endl;
}

// Control if there are checks on the board
bool PlutoCE::isCheck(Board* b, bool checkOnWhite){
    int r = -1, f = -1, knig, bish, rook, quee;
    bool flag = true;
    if(checkOnWhite){
        // Find white king
        for(r = 7; flag && r >= 0; --r)
            for(f = 7; flag && f >= 0; --f)
                if(b->position[r][f] == WHITE_KING)
                    flag = false;
        if(flag){
            std::cout << "Error king not found!" << std::endl;
            exit(1);
        }

        // Check from pawns
        if(INB(r-1) && INB(f-1) && b->position[r-1][f-1] == BLACK_PAWN)
            return true;
        if(INB(r-1) && INB(f+1) && b->position[r-1][f+1] == BLACK_PAWN)
            return true;

        knig = BLACK_KNIG;
        bish = BLACK_BISH;
        rook = BLACK_ROOK;
        quee = BLACK_QUEE;
    }
    else{
        // Find black king
        for(r = 0; flag && r < 8; ++r)
            for(f = 7; flag && f >= 0; --f)
                if(b->position[r][f] == BLACK_KING)
                    flag = false;
        if(flag){
            std::cout << "Error king not found!" << std::endl;
            exit(1);
        }

        // Check from pawns
        if(INB(r+1) && INB(f-1) && b->position[r+1][f-1] == WHITE_PAWN)
            return true;
        if(INB(r+1) && INB(f+1) && b->position[r+1][f+1] == WHITE_PAWN)
            return true;

        knig = WHITE_KNIG;
        bish = WHITE_BISH;
        rook = WHITE_ROOK;
        quee = WHITE_QUEE;
    }

    // Check from knights
    if(INB(r-1) && INB(f-2) && b->position[r-1][f-2] == knig)
        return true;
    if(INB(r-2) && INB(f-1) && b->position[r-2][f-1] == knig)
        return true;
    if(INB(r-2) && INB(f+1) && b->position[r-2][f+1] == knig)
        return true;
    if(INB(r-1) && INB(f+2) && b->position[r-1][f+2] == knig)
        return true;
    if(INB(r+1) && INB(f+2) && b->position[r+1][f+2] == knig)
        return true;
    if(INB(r+2) && INB(f+1) && b->position[r+2][f+1] == knig)
        return true;
    if(INB(r+2) && INB(f-1) && b->position[r+2][f-1] == knig)
        return true;
    if(INB(r+1) && INB(f-2) && b->position[r+1][f-2] == knig)
        return true;

    // Check from sliding pieces
    flag = true;
    bool notBlocked[8] = {true, true, true, true, true, true, true, true};
    for(int i = 1; flag; ++i){
        flag = false;
        if(notBlocked[0] && INB(r-i) && (flag = true) && b->position[r-i][f] != 0){
            if(b->position[r-i][f] == rook || b->position[r-i][f] == quee) return true;
            notBlocked[0] = false;
        }
        if(notBlocked[1] && INB(r-i) && INB(f-i) && (flag = true) && b->position[r-i][f-i] != 0){
            if(b->position[r-i][f-i] == bish || b->position[r-i][f-i] == quee) return true;
            notBlocked[1] = false;
        }
        if(notBlocked[2] && INB(r-i) && INB(f+i) && (flag = true) && b->position[r-i][f+i] != 0){
            if(b->position[r-i][f+i] == bish || b->position[r-i][f+i] == quee) return true;
            notBlocked[2] = false;
        }
        if(notBlocked[3] && INB(f-i) && (flag = true) && b->position[r][f-i] != 0){
            if(b->position[r][f-i] == rook || b->position[r][f-i] == quee) return true;
            notBlocked[3] = false;
        }
        if(notBlocked[4] && INB(f+i) && (flag = true) && b->position[r][f+i] != 0){
            if(b->position[r][f+i] == rook || b->position[r][f+i] == quee) return true;
            notBlocked[4] = false;
        }
        if(notBlocked[5] && INB(r+i) && INB(f-i) && (flag = true) && b->position[r+i][f-i] != 0){
            if(b->position[r+i][f-i] == bish || b->position[r+i][f-i] == quee) return true;
            notBlocked[5] = false;
        }
        if(notBlocked[6] && INB(r+i) && INB(f+i) && (flag = true) && b->position[r+i][f+i] != 0){
            if(b->position[r+i][f+i] == bish || b->position[r+i][f+i] == quee) return true;
            notBlocked[6] = false;
        }
        if(notBlocked[7] && INB(r+i) && (flag = true) && b->position[r+i][f] != 0){
            if(b->position[r+i][f] == rook || b->position[r+i][f] == quee) return true;
            notBlocked[7] = false;
        }
    }
    return false;  // No checks
}

// Try to make the user move on the board.
// If the move is illegal it returns -1, otherwise 0.
int PlutoCE::makeMove(Board* b, std::string str){
    if(str.length() < 4){
        std::cout << "Input move too short!" << std::endl;
        return -1;
    }

    int sr = 56 - str[1];
    int sf = str[0] - 97;
    int er = 56 - str[3];
    int ef = str[2] - 97;

    // Check if the values are in bound
    if(!(INB(sr) && INB(sf) && INB(er) && INB(ef))){
        std::cout << "Indexes out of range!" << std::endl;
        return -1;
    }

    b->position[er][ef] = b->position[sr][sf];
    b->position[sr][sf] = EMPTY;

    return 0;
}

std::priority_queue<Board> generateMove(Board* b){
    std::priority_queue<Board> pq;

    if((b->flags & 0x80) != 0){ // White move
        for(int r = 0; r < 8; ++r){
            for(int f = 0; f < 8; ++f){
                if(b->position[r][f] > 0 && b->position[r][f] < 7){
                    switch(b->position[r][f]){
                        case WHITE_PAWN:{
                            break;}
                        case WHITE_BISH:{
                            break;}
                        case WHITE_KNIG:{
                            break;}
                        case WHITE_ROOK:{
                            break;}
                        case WHITE_QUEE:{
                            break;}
                        case WHITE_KING:{
                            break;}
                    }
                }
            }
        }
    }
    else{  // Black move

    }

    return pq;
}

// Evaluate a given board.
// Parameter taken in consideration:
//   - material count
//   - position of the pieces
//   - passed pawns
//   - number of pawn islands
//   - bishop pair
//   - rook on open file
//   - doubled rooks
//   - king position in middle and end game
int PlutoCE::evaluate(Board* b){
    int evaluation = 0;  // Output value

    int files[5][8] = {0};  // Pawn (w/b) - Rook (w/b) - Open

    int pieceValueCount = 0;      // To know when it go in the endgame
    int whiteKing_r = -1, whiteKing_f = -1; // Index of kings
    int blackKing_r = -1, blackKing_f = -1;
    int whiteNumBish = 0;  // Number of bishop
    int blackNumBish = 0;

    int i, j, k;
    for(i = 0; i < 8; ++i){
        for(j = 0; j < 8; ++j){
            if(b->position[i][j] == EMPTY) continue;
            ++files[4][j];
            switch(b->position[i][j]){
                case WHITE_PAWN: {
                    ++files[0][j];
                    pieceValueCount += 100;
                    evaluation += 100;
                    if(j == 0){
                        for(k = i - 1; k > 0; --k){
                            if(b->position[k][j] == BLACK_PAWN || b->position[i][j+1] == BLACK_PAWN){
                                evaluation += this->ps_pawn[i][j];
                                break;
                            }
                        }
                    }
                    else if(j == 7){
                        for(k = i - 1; k > 0; --k){
                            if(b->position[k][j] == BLACK_PAWN || b->position[i][j-1] == BLACK_PAWN){
                                evaluation += this->ps_pawn[i][j];
                                break;
                            }
                        }
                    }
                    else{
                        for(k = i - 1; k > 0; --k){
                            if(b->position[k][j] == BLACK_PAWN || b->position[i][j-1] == BLACK_PAWN || b->position[i][j+1] == BLACK_PAWN){
                                evaluation += this->ps_pawn[i][j];
                                break;
                            }
                        }
                    }
                    if(k == 0) // Passed pawn
                        evaluation += this->ps_pawn_pass[i][j];
                break; }

				case WHITE_BISH: {
				    pieceValueCount += 300;
				    evaluation += 310 + this->ps_bishop[i][j];
				    if(whiteNumBish == 1)
                        evaluation += 10; // Bishop pair
                    ++whiteNumBish;
                break; }

				case WHITE_KNIG: {
				    pieceValueCount += 300;
				    evaluation += 305 + this->ps_knight[i][j];
                break; }

				case WHITE_ROOK: {
				    ++files[2][j];
				    pieceValueCount += 500;
				    evaluation += 500 + this->ps_rook[i][j];
                break; }

				case WHITE_QUEE: {
				    pieceValueCount += 900;
				    evaluation += 900 + this->ps_queen[i][j];
                break; }

				case WHITE_KING: {
				    whiteKing_r = i;
				    whiteKing_f = j;
                break; }

				case BLACK_PAWN: {
				    ++files[1][j];
				    pieceValueCount += 100;
                    evaluation -= 100;
                    if(j == 0){
                        for(k = i + 1; k < 7; ++k){
                            if(b->position[k][j] == WHITE_PAWN || b->position[i][j+1] == WHITE_PAWN){
                                evaluation -= this->ps_pawn[7-i][j];
                                break;
                            }
                        }
                    }
                    else if(j == 7){
                        for(k = i + 1; k < 7; ++k){
                            if(b->position[k][j] == WHITE_PAWN || b->position[i][j-1] == WHITE_PAWN){
                                evaluation -= this->ps_pawn[7-i][j];
                                break;
                            }
                        }
                    }
                    else{
                        for(k = i + 1; k < 7; ++k){
                            if(b->position[k][j] == WHITE_PAWN || b->position[i][j-1] == WHITE_PAWN || b->position[i][j+1] == WHITE_PAWN){
                                evaluation -= this->ps_pawn[7-i][j];
                                break;
                            }
                        }
                    }
                    if(k == 7) // Passed pawn
                        evaluation -= this->ps_pawn_pass[7-i][j];
                break; }

				case BLACK_BISH: {
				    pieceValueCount += 300;
				    evaluation -= 310 + this->ps_bishop[7-i][j];
				    if(blackNumBish == 1)
                        evaluation -= 10;  // Bishop pair
                    ++blackNumBish;
                break; }

				case BLACK_KNIG: {
				    pieceValueCount += 300;
				    evaluation -= 305 + this->ps_knight[7-i][j];
                break; }

				case BLACK_ROOK: {
				    ++files[3][j];
				    pieceValueCount += 500;
				    evaluation -= 500 + this->ps_rook[7-i][j];
                break; }

				case BLACK_QUEE: {
				    pieceValueCount += 900;
				    evaluation -= 900 + this->ps_queen[7-i][j];
                break; }

				case BLACK_KING: {
				    blackKing_r = i;
				    blackKing_f = j;
                break; }

                default: {
                    std::cout << "Error in evaluating the position. Wrong piece!" << std::endl;
                    exit(1);
                break; }
            }
        }
    }

    // Check if there are king on the board
    if(whiteKing_r == -1 || whiteKing_f == -1){
        if(blackKing_r == -1 || blackKing_f == -1){
            std::cout << "Error in evaluating the position. No kings found!" << std::endl;
            exit(1);
        }
        return INT_MIN;
    }
    else if(blackKing_r == -1 || blackKing_f == -1){
        return INT_MAX;
    }

    // Evaluate the pawn islands
    if(files[0][0] > 0)
        evaluation -= 15;
    if(files[1][0] > 0)
        evaluation += 15;
    for(i = 1; i < 8; i++){
        if(files[0][i] > 0){  // White pawns
            if(files[0][i-1] == 0)
                evaluation -= 15;   // Pawn island
            if(files[0][i] > 1)
                evaluation -= 30;   // Double or triple pawn
        }
        if(files[1][i] > 0){  // Black pawns
            if(files[1][i-1] == 0)
                evaluation += 15;
            if(files[1][i] > 1)
                evaluation += 30;
        }
    }

    // Rook on open files and doubled
    for(i = 0; i < 8; ++i){
        if(files[2][i] > 0){      // White rooks
            if(files[4][i] == 0)  // Open file
                evaluation += 10;
            if(files[2][i] > 1)   // Doubled rook
                evaluation += 20;
        }
        if(files[3][i] > 0){      // Black rooks
            if(files[4][i] == 0)
                evaluation -= 10;
            if(files[3][i] > 1)
                evaluation -= 20;
        }
    }

    // Check if we are in the endgame
    // 2600 is exactly:
    // 1 King + 1 Queen + 1 Minor + 1 Pawn each or equivalent
    if(pieceValueCount < 2600)
        evaluation += this->ps_king_late[whiteKing_r][whiteKing_f] - this->ps_king_late[7 - blackKing_r][blackKing_f];
    else
        evaluation += this->ps_king[whiteKing_r][whiteKing_f] - this->ps_king[7 - blackKing_r][blackKing_f];

    return evaluation;
}*/
