#include "drefys.hpp"

constexpr int8_t WHITE_PAWN =  0;
constexpr int8_t BLACK_PAWN =  1;
constexpr int8_t WHITE_KNIG =  2;
constexpr int8_t BLACK_KNIG =  3;
constexpr int8_t WHITE_BISH =  4;
constexpr int8_t BLACK_BISH =  5;
constexpr int8_t WHITE_ROOK =  6;
constexpr int8_t BLACK_ROOK =  7;
constexpr int8_t WHITE_QUEE =  8;
constexpr int8_t BLACK_QUEE =  9;
constexpr int8_t WHITE_KING = 10;
constexpr int8_t BLACK_KING = 11;
constexpr int8_t EMPTY      = 12;
constexpr int8_t OOB        = -1;  // Out of bounds


Move::Move(){
    return;
}

Move::Move(uint8_t sp, uint8_t ep, int8_t c){
    this->startPos = sp;
    this->endPos = ep;
    this->captured = c;
    this->promoted = -1;
    return;
}

Move::Move(uint8_t sp, uint8_t ep, int8_t c, int8_t p){
    this->startPos = sp;
    this->endPos = ep;
    this->captured = c;
    this->promoted = p;
    return;
}

Board::Board(){
    return;
}

Board::Board(const Board& b){
    memcpy(this->position, b.position, 120);
    this->white_to_move = b.white_to_move;
    memcpy(this->castling, b.castling, 4);
    this->enpassant = b.enpassant;
    this->lastzeroing = b.lastzeroing;
    this->evaluation = b.evaluation;
    return;
}

// Setup the board using the FEN
void Drefys::fillFEN(Board& b, std::string fen){
    int r = 2, f = 1, i, index;
    // Board position
    for(i = 0; fen[i] != ' '; ++i){
        if(fen[i] == '/'){
            r += 1;
            f = 1;
            continue;
        }
        else if(fen[i] > 47 && fen[i] < 58){ // Number
            index = 10 * r + f;
            for(int j = 0; j < fen[i] - 48; ++j, ++f)
                b.position[index++] = EMPTY;
        }
        else{  // Character
            index = 10 * r + f;
            switch(fen[i]){
                case 'P': b.position[index] = WHITE_PAWN; break;
                case 'p': b.position[index] = BLACK_PAWN; break;
				case 'N': b.position[index] = WHITE_KNIG; break;
				case 'n': b.position[index] = BLACK_KNIG; break;
				case 'B': b.position[index] = WHITE_BISH; break;
				case 'b': b.position[index] = BLACK_BISH; break;
				case 'R': b.position[index] = WHITE_ROOK; break;
				case 'r': b.position[index] = BLACK_ROOK; break;
				case 'Q': b.position[index] = WHITE_QUEE; break;
				case 'q': b.position[index] = BLACK_QUEE; break;
				case 'K': b.position[index] = WHITE_KING; break;
				case 'k': b.position[index] = BLACK_KING; break;

				default: // Wrong piece code
					std::cout << "Wrong piece in the FEN!" << std::endl;
					exit(1);
					break;
            }
            ++f;
        }
    }
    if(r != 9 || f != 9){
        std::cout << "Board not fully specified!" << std::endl;
        exit(1);
    }

    // Player to move
    ++i;
    if(fen[i] == 'w')
        b.white_to_move = true;
    else if(fen[i] == 'b')
        b.white_to_move = false;
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
                    if(b.position[95] == WHITE_KING && b.position[98] == WHITE_ROOK)
                        b.castling[0] = true;
                    break;
                case 'Q':
                    if(b.position[95] == WHITE_KING && b.position[91] == WHITE_ROOK)
                        b.castling[1] = true;
                    break;
                case 'k':
                    if(b.position[25] == BLACK_KING && b.position[28] == BLACK_ROOK)
                        b.castling[2] = true;
                    break;
                case 'q':
                    if(b.position[25] == BLACK_KING && b.position[21] == BLACK_ROOK)
                        b.castling[3] = true;
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
        b.enpassant = 0;
    }
    else{
        f = fen[i] - 97;
        r = 56 - fen[++i];

        if((r & 0x7) != r || (f & 0x7) != f){
            std::cout << "Indexes of enpassant out of range!" << std::endl;
            exit(1);
        }

        b.enpassant = 10 * (r + 2) + f + 1;
    }

    // Distance in ply to last zeroing move
    std::string ply;
    for(i += 2; fen[i] != ' '; ++i)
        ply.push_back(fen[i]);

    b.lastzeroing = std::stoi(ply, NULL);

    // Evaluate
    b.evaluation = 0;
}

// Setup the board in the starting position
void Drefys::fillStartingPos(Board& b){
	Drefys::fillFEN(b, "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
}

// Setup an empty board.
// The rights to castle are also set.
void Drefys::fillEmpty(Board& b){
	for(int i = 2; i < 10; ++i)
		for(int j = 1; j < 9; ++j)
            b.position[10 * i + j] = EMPTY;

    b.white_to_move = true;
	b.castling[0] = b.castling[1] = b.castling[2] = b.castling[3] = false;
	b.enpassant = 0;
	b.lastzeroing = 0;
	b.evaluation = 0;
}

// Save the game state in a FEN string
std::string Drefys::exportFEN(const Board& b){
    std::string fen = "";

    // Board position
    int cnt= 0;
    for(int r = 2; r < 10; ++r){
        for(int f = 1; f < 9; ++f){
            if(b.position[10 * r + f] == EMPTY){
                ++cnt;
                continue;
            }
            if(cnt > 0){
                fen.push_back(cnt + 48);
                cnt = 0;
            }
            switch(b.position[10 * r + f]){
                case WHITE_PAWN: fen.push_back('P'); break;
                case BLACK_PAWN: fen.push_back('p'); break;
				case WHITE_KNIG: fen.push_back('N'); break;
				case BLACK_KNIG: fen.push_back('n'); break;
				case WHITE_BISH: fen.push_back('B'); break;
				case BLACK_BISH: fen.push_back('b'); break;
				case WHITE_ROOK: fen.push_back('R'); break;
				case BLACK_ROOK: fen.push_back('r'); break;
				case WHITE_QUEE: fen.push_back('Q'); break;
				case BLACK_QUEE: fen.push_back('q'); break;
				case WHITE_KING: fen.push_back('K'); break;
				case BLACK_KING: fen.push_back('k'); break;

				default: // Wrong castling code
					std::cout << "Wrong piece in the FEN!" << std::endl;
					exit(1);
					break;

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
    fen += ((b.white_to_move) ? "w " : "b ");

    // Castling rights
    if(b.castling[0]) fen.push_back('K');
    if(b.castling[1]) fen.push_back('Q');
    if(b.castling[2]) fen.push_back('k');
    if(b.castling[3]) fen.push_back('q');
    if(!(b.castling[0] || b.castling[1] || b.castling[2] || b.castling[3])) fen.push_back('-');
    fen.push_back(' ');

    // Enpassant
    if(b.enpassant == 0)
        fen.push_back('-');
    else{
        fen.push_back(96 + b.enpassant % 10);
        fen.push_back(58 - b.enpassant / 10);
    }
    fen.push_back(' ');

    // Last zeroing and move counter
    fen += std::to_string(b.lastzeroing);
    fen += " 1";  // Always to 1 because who cares?

    return fen;
}

// Display the board to stdout
void Drefys::displayBoard(const Board& b){
	for(int i = 2; i < 10; ++i){
		for(int j = 1; j < 9; ++j){
			switch(b.position[10 * i + j]){
				case WHITE_PAWN: std::cout << "P "; break;
				case BLACK_PAWN: std::cout << "p "; break;
				case WHITE_KNIG: std::cout << "N "; break;
				case BLACK_KNIG: std::cout << "n "; break;
				case WHITE_BISH: std::cout << "B "; break;
				case BLACK_BISH: std::cout << "b "; break;
				case WHITE_ROOK: std::cout << "R "; break;
				case BLACK_ROOK: std::cout << "r "; break;
				case WHITE_QUEE: std::cout << "Q "; break;
				case BLACK_QUEE: std::cout << "q "; break;
				case WHITE_KING: std::cout << "K "; break;
				case BLACK_KING: std::cout << "k "; break;
				case EMPTY:      std::cout << ". "; break;

				default: // Wrong piece code
					std::cout << std::endl << "Error at: " << (10 * i + j) << std::endl;
					exit(1);
					break;
			}
		}
		std::cout << std::endl;
	}
}

// Display the board + some debug info
void Drefys::debugBoard(const Board& b){
    Drefys::displayBoard(b);

    std::cout << ((b.white_to_move) ? "White to move" : "Black to move") << std::endl;
    std::cout << "White castling: " << b.castling[0] << " - " << b.castling[1] << std::endl;
    std::cout << "Black castling: " << b.castling[2] << " - " << b.castling[3] << std::endl;

    std::cout << "Enpassant: ";
    if(b.enpassant == 0)
        std::cout << "-" << std::endl;
    else{
        const int32_t *coord = Drefys::coordFrom64[Drefys::index120To64[b.enpassant]];
        std::cout << Drefys::fileIntToChar(coord[0]) << coord[1] + 1 << std::endl;
    }
    std::cout << "Moves since last zeroing: " << b.lastzeroing << std::endl;

    std::cout << "Evaluation: " << b.evaluation << std::endl;
}

void Drefys::doMoveUser(Board& b, std::string str){
    if(str.size() < 4 || str.size() > 6)
        return;

    Move m = Move();

    if(str.size() == 5){
        switch(str[4]){
            case 'N': m.promoted = WHITE_KNIG; break;
            case 'B': m.promoted = WHITE_BISH; break;
            case 'R': m.promoted = WHITE_ROOK; break;
            case 'Q': m.promoted = WHITE_QUEE; break;
            case 'n': m.promoted = BLACK_KNIG; break;
            case 'b': m.promoted = BLACK_BISH; break;
            case 'r': m.promoted = BLACK_ROOK; break;
            case 'q': m.promoted = BLACK_QUEE; break;

            default:
                return;
        }
    }

    int sf = Drefys::fileCharToInt(str[0]);
    int sr = str[1] - 49;
    int ef = Drefys::fileCharToInt(str[2]);
    int er = str[3] - 49;
    int sp64 = Drefys::coordTo64[sf][sr];
    int ep64 = Drefys::coordTo64[ef][er];

    m.startPos = Drefys::index64To120[sp64];
    m.endPos = Drefys::index64To120[ep64];
    m.captured = b.position[m.endPos];

    Drefys::doMove(b, m);
}

inline void Drefys::doMove(Board& b, Move m){
    b.position[m.endPos] = b.position[m.startPos];
    b.position[m.startPos] = EMPTY;
    return;
}

inline void Drefys::undoMove(Board& b, Move m){
    return;
}

inline char Drefys::fileIntToChar(int32_t f){
    return f + 97;
}

inline int32_t Drefys::fileCharToInt(char f){
    return f - 97;
}
