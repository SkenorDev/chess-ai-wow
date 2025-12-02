#include "Chess.h"
#include "MagicBitboards.h"
#include "BitBoard.h"
#include <limits>
#include <cmath>

Chess::Chess()
{
    _grid = new Grid(8, 8);
}

Chess::~Chess()
{
    delete _grid;
}

char Chess::pieceNotation(int x, int y) const
{
    const char *wpieces = { "0PNBRQK" };
    const char *bpieces = { "0pnbrqk" };
    Bit *bit = _grid->getSquare(x, y)->bit();
    char notation = '0';
    if (bit) {
        notation = bit->gameTag() < 128 ? wpieces[bit->gameTag()] : bpieces[bit->gameTag()-128];
    }
    return notation;
}

Bit* Chess::PieceForPlayer(const int playerNumber, ChessPiece piece)
{
    const char* pieces[] = { "pawn.png", "knight.png", "bishop.png", "rook.png", "queen.png", "king.png" };

    Bit* bit = new Bit();
    // should possibly be cached from player class?
    const char* pieceName = pieces[piece - 1];
    std::string spritePath = std::string("") + (playerNumber == 0 ? "w_" : "b_") + pieceName;
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setOwner(getPlayerAt(playerNumber));
    bit->setSize(pieceSize, pieceSize);

    return bit;
}

void Chess::setUpBoard()
{
    setNumberOfPlayers(2);
    _gameOptions.rowX = 8;
    _gameOptions.rowY = 8;
   // _moves = generateAllMoves(stateString(),)
    _grid->initializeChessSquares(pieceSize, "boardsquare.png");
    FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
    //FENtoBoard("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
   if(gameHasAI()){
    setAIPlayer(1);
   }
    startGame();
}

void Chess::FENtoBoard(const std::string& fen) {
    int boardIndex=0;
    for (int i=0;i<fen.length();i++){
        char current = fen[i];
        if(boardIndex<64){
        if(current=='/'){
            continue;
        }
        //convert into corelating int
        int skip = current-'0';
        if(skip>0&&skip<9){
            boardIndex+=skip;
            continue;
        }
        
        placePiece(boardIndex,current);
        boardIndex++;
        }
     }
}
void Chess::placePiece(int pos, char input) {
    int player = 0;
    if (input >= 'a') {
        player = 1;
        }
     Bit *bit = new Bit();
     int tag = 0;
     switch (tolower(input)) {
        case 'p': tag = 1; break;
        case 'n': tag = 2; break;
        case 'b': tag = 3; break;
        case 'r': tag = 4; break;
        case 'q': tag = 5; break;
        case 'k': tag = 6; break;
    }

    if (player == 1)
        tag += 128;

    bit->setGameTag(tag);
     std::string spritePath;
    if(input=='b'||input=='B') spritePath = std::string("") + (player == 0 ? "w_" : "b_") + "bishop.png";
    if(input=='p'||input=='P') spritePath = std::string("") + (player == 0 ? "w_" : "b_") + "pawn.png";
    if(input=='r'||input=='R') spritePath = std::string("") + (player == 0 ? "w_" : "b_") + "rook.png";
     if(input=='n'||input=='N') spritePath = std::string("") + (player == 0 ? "w_" : "b_") + "knight.png";
    if(input=='q'||input=='Q') spritePath = std::string("") + (player == 0 ? "w_" : "b_") + "queen.png";
    if(input=='k'||input=='K') spritePath = std::string("") + (player == 0 ? "w_" : "b_") + "king.png";
    bit->LoadTextureFromFile(spritePath.c_str());
    bit->setPosition(_grid->getSquare(pos%8,pos/8)->getPosition());
    bit->setOwner(getPlayerAt(player));
    bit->setSize(pieceSize, pieceSize);
    // printf("%d",bit->getEntityType());
    _grid->getSquare(pos%8,pos/8)->setBit(bit);

}
 


bool Chess::actionForEmptyHolder(BitHolder &holder)
{
    return false;
}

bool Chess::canBitMoveFrom(Bit &bit, BitHolder &src)
{
    // need to implement friendly/unfriendly in bit so for now this hack
    int currentPlayer = getCurrentPlayer()->playerNumber() * 128;
    int pieceColor = bit.gameTag() & 128;
    int tag = bit.gameTag();
    int pieceType = tag & 0x7F;
    
     return true;
}




/*
vector<BitMove> getAllMoves(Bit &bit){

read fenNotation and generate bitboard

for all your pieces, getfriendlypieces(), generate moves depenind what it is and if its valid
read bitboard and generate all moves for that piece
}

*/





void Chess::stopGame()
{
    _grid->forEachSquare([](ChessSquare* square, int x, int y) {
        square->destroyBit();
    });
}

Player* Chess::ownerAt(int x, int y) const
{
    if (x < 0 || x >= 8 || y < 0 || y >= 8) {
        return nullptr;
    }

    auto square = _grid->getSquare(x, y);
    if (!square || !square->bit()) {
        return nullptr;
    }
    return square->bit()->getOwner();
}

Player* Chess::checkForWinner()
{
    return nullptr;
}

bool Chess::checkForDraw()
{
    return false;
}

std::string Chess::initialStateString()
{
    return stateString();
}

std::string Chess::stateString()
{
    std::string s;
    s.reserve(64);
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            s += pieceNotation( x, y );
        }
    );
    return s;}

void Chess::setStateString(const std::string &s)
{
    _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
        int index = y * 8 + x;
        char playerNumber = s[index] - '0';
        if (playerNumber) {
            square->setBit(PieceForPlayer(playerNumber - 1, Pawn));
        } else {
            square->setBit(nullptr);
        }
    });
}
// Generate actual move objects from a bitboard
void Chess::generateKnightMoves(std::vector<BitMove>& moves, BitBoard knightBoard, uint64_t occupancy) {
    knightBoard.forEachBit([&](int fromSquare) {
        BitBoard moveBitboard = BitBoard(KnightAttacks[fromSquare] & occupancy);
        // Efficiently iterate through only the set bits
        moveBitboard.forEachBit([&](int toSquare) {
            moves.emplace_back(fromSquare, toSquare, Knight);
        });
    });
}
void Chess::generateKingMoves(std::vector<BitMove>& moves, BitBoard kingBoard, uint64_t occupancy) {
    kingBoard.forEachBit([&](int fromSquare) {
        BitBoard moveBitboard = BitBoard(KingAttacks[fromSquare] & occupancy);
        // Efficiently iterate through only the set bits
        moveBitboard.forEachBit([&](int toSquare) {
            moves.emplace_back(fromSquare, toSquare, King);
        });
    });
}





// 21 mins in nov 21 for explanation of generate all moves needs
void Chess::updateAI() 
{
    //int currentPlayer = getCurrentPlayer()->playerNumber();
    auto state= stateString();
    auto _moves = generateAllMoves(state,2);
    int bestVal = negInfinite;
    BitMove bestMove;


    for(auto move : _moves) {
        int srcSquare = move.from;
        int dstSquare = move.to;
    
        char oldDst=state[dstSquare];
        char srcPce = state[srcSquare];
        state[dstSquare] = srcPce;
        state[srcSquare] = '0';
            int moveVal = -negamax(state, 3, negInfinite,posInfinite ,HUMAN_PLAYER);
            // Undo the move
            state[dstSquare] = oldDst;
            state[srcSquare] = srcPce;
            // If the value of the current move is more than the best value, update best
            if (moveVal > bestVal) {
                bestMove = move;
                bestVal = moveVal;
            }
        }
        if(bestVal != negInfinite){
            int srcSquare=bestMove.from;
            int dstSquare = bestMove.to;
             BitHolder& src =getHolderAt(srcSquare&7,srcSquare/8);
             BitHolder& dst =getHolderAt(dstSquare&7, dstSquare/8);
            Bit* bit =src.bit();
            
            dst.dropBitAtPoint(bit, ImVec2(0,0));
            src.setBit(nullptr);
            bitMovedFromTo(*bit,src,dst);
        }
        else{
           // printf("uh oh \n");
        }
    };
int Chess::negamax(std::string& state, int depth, int alpha, int beta, int playerColor) {
    if (depth == 0){
        return evaluateBoard(state) * playerColor;
    }
    
    auto newMoves= generateAllMoves(state, playerColor);
    // Check if AI wins, human wins, or draw
  int bestVal =negInfinite; // Min value
    for(auto move : newMoves) {
        int srcSquare = move.from;
        int dstSquare = move.to;
    
        char oldDst=state[dstSquare];
        char srcPce = state[srcSquare];
        state[dstSquare] = srcPce;
        state[srcSquare] = '0';
            int moveVal = -negamax(state, depth-1, -beta,-alpha ,-playerColor);
            // Undo the move
            state[dstSquare] = oldDst;
            state[srcSquare] = srcPce;
            // If the value of the current move is more than the best value, update best
            if (moveVal > bestVal) {
                bestVal = moveVal;
            }
        }
    return bestVal;
}
int Chess::evaluateBoard(std::string state){
    int values[128];
    values['P']=100;
values['N']=300;
values['B']=400;
values['R']=500;
values['Q']=900;
values['K']=2000;

    values['-']=100;
values['n']=-300;
values['b']=-400;
values['r']=-500;
values['q']=-900;
values['k']=-2000;
values['0']=0;
int score =0;
for(char ch:state){
 score+= values[ch];

}
return score;
}
std::array<char, 64> Chess::fenToArray64(const std::string& fen) {
    std::array<char, 64> board{};
    board.fill('\0'); // empty squares = null char

    int idx = 0;

    for (char c : fen) {
        if (idx >= 64) break;

        if (c == '/') {
            continue; // next rank
        } 
        else if (std::isdigit(c)) {
            // number = that many empty squares
            int empty = c - '0';
            for (int i = 0; i < empty && idx < 64; ++i)
                board[idx++] = '\0';
        }
        else if (std::isalpha(c)) {
            // piece: white => lowercase, black => UPPERCASE
            bool isWhite = std::isupper(c);
            char piece = std::tolower(c);

            if (!isWhite) {
                piece = std::toupper(piece);
            }

            board[idx++] = piece;
        }
        else {
            // reached space → end of board portion
            break;
        }
    }

    return board;
}
   inline bool whiteCheck(char c) { return c >= 'a' && c <= 'z'; }
inline bool blackCheck(char c) { return c >= 'A' && c <= 'Z'; }
inline bool sameSide(char a, char b) {
    return (whiteCheck(a) && whiteCheck(b)) || (blackCheck(a) && blackCheck(b));
}

inline int fileOf(int idx) { return idx % 8; }
inline int rankOf(int idx) { return idx / 8; }
std::vector<BitMove> Chess::generateAllMoves(std::string& state, int turn) {
    std::vector<BitMove> moves;
    moves.reserve(256);
    auto board = state;
    bool whiteTurn = (turn == 1);
    for (int sq = 0; sq < 64; ++sq) {
        char p = board[sq];
        if (p == '0') continue;

        if (!whiteTurn) {
            if (p == 'p') {
                int f = fileOf(sq);

                // single push (sq + 8)
                if (sq + 8 < 64 && board[sq + 8] == '0') {
                    moves.emplace_back(sq, sq + 8, Pawn);
                }

                // double push (black starts on rank 1 = squares 8–15)
                if (sq >= 8 && sq <= 15) {
                    if (board[sq + 8] == '0' && board[sq + 16] == '0') {
                        moves.emplace_back(sq, sq + 16, Pawn);
                    }
                }

                // capture left (sq + 7)
                if (f > 0 && sq + 7 < 64) {
                    if (board[sq + 7] != '0' && !sameSide(p, board[sq + 7])) {
                        moves.emplace_back(sq, sq + 7, Pawn);
                    }
                }

                // capture right (sq + 9)
                if (f < 7 && sq + 9 < 64) {
                    if (board[sq + 9] != '0' && !sameSide(p, board[sq + 9])) {
                        moves.emplace_back(sq, sq + 9, Pawn);
                    }
                }
                continue;
            }
            if (p == 'n') {
                static const int knightMoves[8] = {
                    17, 15, 10, 6, -6, -10, -15, -17
                };
                for (int d : knightMoves) {
                    int to = sq + d;
                    if (to < 0 || to >= 64) continue;

                    int f1 = fileOf(sq);
                    int f2 = fileOf(to);
                    if (abs(f1 - f2) > 2) continue;

                    if (board[to] == '0' || !sameSide(p, board[to])) {
                        moves.emplace_back(sq, to, Knight);
                    }
                }
                continue;
            }
            if (p == 'k') {
                static const int kingMoves[8] = {
                    1, -1, 8, -8, 9, -9, 7, -7
                };
                for (int d : kingMoves) {
                    int to = sq + d;
                    if (to < 0 || to >= 64) continue;

                    int f1 = fileOf(sq);
                    int f2 = fileOf(to);
                    if (abs(f1 - f2) > 2) continue;

                    if (board[to] == '0' || !sameSide(p, board[to])) {
                        moves.emplace_back(sq, to, King);
                        continue;
                    }
                }
            }
            if (p == 'B') {
                static const int dirs[4] = { 9, -9, 7, -7 };
                for (int d : dirs) {
                    int to = sq;
                    while (true) {
                        int f1 = fileOf(to);
                        to += d;
                        if (to < 0 || to >= 64) break;

                        int f2 = fileOf(to);
                        if (abs(f1 - f2) != 1) break;  // wrap-around check

                        if (board[to] == '0') {
                            moves.emplace_back(sq, to, Bishop);
                        } else {
                            // Stop sliding after encountering any piece:
                            if (!sameSide(p, board[to])) {
                                moves.emplace_back(sq, to, Bishop);
                            }
                            break;  // crucial break here to stop moving beyond this piece
                        }
                    }
                }
            }

            // Rook
            if (p == 'R') {
                static const int dirs[4] = { 8, -8, 1, -1 };
                for (int d : dirs) {
                    int to = sq;
                    while (true) {
                        int f1 = fileOf(to);
                        to += d;
                        if (to < 0 || to >= 64) break;

                        int f2 = fileOf(to);
                        if ((d == 1 || d == -1) && f1 != f2) break;  // horizontal wrap check

                        if (board[to] == '0') {
                            moves.emplace_back(sq, to, Rook);
                        } else {
                            if (!sameSide(p, board[to])) {
                                moves.emplace_back(sq, to, Rook);
                            }
                            break;  // must break here instead of continue
                        }
                    }
                }
            }

            // Queen
            if (p == 'Q') {
                static const int dirs[8] = { 8, -8, 1, -1, 9, -9, 7, -7 };
                for (int d : dirs) {
                    int to = sq;
                    while (true) {
                        int f1 = fileOf(to);
                        to += d;
                        if (to < 0 || to >= 64) break;

                        int f2 = fileOf(to);

                        // Horizontal wrap check
                        if ((d == 1 || d == -1) && f1 != f2) break;

                        // Diagonal wrap check
                        if ((d == 9 || d == -9 || d == 7 || d == -7) && abs(f1 - f2) != 1) break;

                        if (board[to] == '0') {
                            moves.emplace_back(sq, to, Queen);
                        } else {
                            if (!sameSide(p, board[to])) {
                                moves.emplace_back(sq, to, Queen);
                            }
                            break;
                        }
                    }
                }
            }
        }
        // White Turn
        else {
            if (p == 'P') {
                int f = fileOf(sq);

                // single push (sq - 8)
                if (sq - 8 >= 0 && board[sq - 8] == '0') {
                    moves.emplace_back(sq, sq - 8, Pawn);
                }

                // double push (white starts on rank 6 = squares 48–55)
                if (sq >= 48 && sq <= 55) {
                    if (board[sq - 8] == '0' && board[sq - 16] == '0') {
                        moves.emplace_back(sq, sq - 16, Pawn);
                    }
                }

                // capture left (sq - 9)
                if (f > 0 && sq - 9 >= 0) {
                    if (board[sq - 9] != '0' && !sameSide(p, board[sq - 9])) {
                        moves.emplace_back(sq, sq - 9, Pawn);
                    }
                }

                // capture right (sq - 7)
                if (f < 7 && sq - 7 >= 0) {
                    if (board[sq - 7] != '0' && !sameSide(p, board[sq - 7])) {
                        moves.emplace_back(sq, sq - 7, Pawn);
                    }
                }
                continue;
            }

            if (p == 'N') {
                static const int knightMoves[8] = {
                    17, 15, 10, 6, -6, -10, -15, -17
                };
                for (int d : knightMoves) {
                    int to = sq + d;
                    if (to < 0 || to >= 64) continue;

                    int f1 = fileOf(sq);
                    int f2 = fileOf(to);
                    if (abs(f1 - f2) > 2) continue;

                    if (board[to] == '0' || !sameSide(p, board[to])) {
                        moves.emplace_back(sq, to, Knight);
                    }
                }
                continue;
            }

            if (p == 'K') {
                static const int kingMoves[8] = {
                    1, -1, 8, -8, 9, -9, 7, -7
                };
                for (int d : kingMoves) {
                    int to = sq + d;
                    if (to < 0 || to >= 64) continue;

                    int f1 = fileOf(sq);
                    int f2 = fileOf(to);
                    if (abs(f1 - f2) > 2) continue;

                    if (board[to] == '0' || !sameSide(p, board[to])) {
                        moves.emplace_back(sq, to, King);
                    }
                }
            }

            // Bishop
            if (p == 'B') {
                static const int dirs[4] = { 9, -9, 7, -7 };
                for (int d : dirs) {
                    int to = sq;
                    while (true) {
                        int f1 = fileOf(to);
                        to += d;
                        if (to < 0 || to >= 64) break;

                        int f2 = fileOf(to);
                        if (abs(f1 - f2) != 1) break;  // wrap-around check

                        if (board[to] == '0') {
                            moves.emplace_back(sq, to, Bishop);
                        } else {
                            // Stop sliding after encountering any piece:
                            if (!sameSide(p, board[to])) {
                                moves.emplace_back(sq, to, Bishop);
                            }
                            break;  // crucial break here to stop moving beyond this piece
                        }
                    }
                }
            }

            // Rook
            if (p == 'R') {
                static const int dirs[4] = { 8, -8, 1, -1 };
                for (int d : dirs) {
                    int to = sq;
                    while (true) {
                        int f1 = fileOf(to);
                        to += d;
                        if (to < 0 || to >= 64) break;

                        int f2 = fileOf(to);
                        if ((d == 1 || d == -1) && f1 != f2) break;  // horizontal wrap check

                        if (board[to] == '0') {
                            moves.emplace_back(sq, to, Rook);
                        } else {
                            if (!sameSide(p, board[to])) {
                                moves.emplace_back(sq, to, Rook);
                            }
                            break;  // must break here instead of continue
                        }
                    }
                }
            }

            // Queen
            if (p == 'Q') {
                static const int dirs[8] = { 8, -8, 1, -1, 9, -9, 7, -7 };
                for (int d : dirs) {
                    int to = sq;
                    while (true) {
                        int f1 = fileOf(to);
                        to += d;
                        if (to < 0 || to >= 64) break;

                        int f2 = fileOf(to);

                        // Horizontal wrap check
                        if ((d == 1 || d == -1) && f1 != f2) break;

                        // Diagonal wrap check
                        if ((d == 9 || d == -9 || d == 7 || d == -7) && abs(f1 - f2) != 1) break;

                        if (board[to] == '0') {
                            moves.emplace_back(sq, to, Queen);
                        } else {
                            if (!sameSide(p, board[to])) {
                                moves.emplace_back(sq, to, Queen);
                            }
                            break;
                        }
                    }
                }
            }
        }

    }

    return moves;
}

bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    
    ChessSquare* srcSquare = dynamic_cast<ChessSquare*>(&src);
    ChessSquare* dstSquare = dynamic_cast<ChessSquare*>(&dst);

    int pieceType = bit.gameTag() & 0x7F;
    int piecePlayer = (bit.gameTag() & 128) ? 1 : 0;
     int currentPlayer = getCurrentPlayer()->playerNumber();
            if (piecePlayer != currentPlayer) {
                    return false;
                }
    if (pieceType == Knight){
       
        int currentPlayer = getCurrentPlayer()->playerNumber();
        uint64_t friendlyPieces = 0ULL; // Bitboard of friendly pieces 
        if (piecePlayer != currentPlayer) {
    return false;
}

        //get all friendly pieces
        _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            Bit *b = square->bit();
            if (b)
            {
                int tag = b->gameTag();
                int piecePlayer = (tag & 128) ? 1 : 0; 
                if (piecePlayer == currentPlayer) {
                    friendlyPieces |= (1ULL << square->getSquareIndex());
                }
            }

        });

        int fromSquare = srcSquare->getSquareIndex();
        BitBoard knightBoard(1ULL << fromSquare); //where whatever piece is located
        uint64_t validTargets = ~friendlyPieces; 
        std::vector<BitMove> knightmoves;
        generateKnightMoves(knightmoves, knightBoard, validTargets);

        int toSquare = dstSquare->getSquareIndex();
        for (const BitMove& move : knightmoves) {
            if (move.to == toSquare) {
                return true;
            }
        }
        return false;
    }
    if(pieceType==King){
        int currentPlayer = getCurrentPlayer()->playerNumber();
        uint64_t friendlyPieces = 0ULL; // Bitboard of friendly pieces 
        if (piecePlayer != currentPlayer) {
    return false;
}
         _grid->forEachSquare([&](ChessSquare* square, int x, int y) {
            Bit *b = square->bit();
            if (b)
            {
                int tag = b->gameTag();
                int piecePlayer = (tag & 128) ? 1 : 0; 
                if (piecePlayer == currentPlayer) {
                    friendlyPieces |= (1ULL << square->getSquareIndex());
                }
            }

        });

        int fromSquare = srcSquare->getSquareIndex();
        BitBoard kingBoard(1ULL << fromSquare); //where whatever piece is located
        uint64_t validTargets = ~friendlyPieces; 

        std::vector<BitMove> knightmoves;
        generateKingMoves(knightmoves, kingBoard, validTargets);

        int toSquare = dstSquare->getSquareIndex();
        for (const BitMove& move : knightmoves) {
            if (move.to == toSquare) {
                return true;
            }
        }
        return false;


    }

    ChessSquare* srcxy = static_cast<ChessSquare*>(&src);
    ChessSquare* dstxy = static_cast<ChessSquare*>(&dst);
     int srcx = srcxy->getColumn();
    int srcy = srcxy->getRow();
         int dstx =dstxy->getColumn();
    int dsty = dstxy->getRow();
    int player = getCurrentPlayer()->playerNumber();
    //White pawn
    if(bit.gameTag()==1&&player==0){
        if (dstx==srcx){
             if (srcy==6&&dsty==4&& getHolderAt(srcx,srcy-2).bit()==NULL){
                return true;
            }
            if (srcy-1==dsty&& getHolderAt(srcx,srcy-1).bit()==NULL){
                return true;
            }      
    }
             if (dstx == srcx - 1 && srcx != 0&& srcy -1 ==dsty) {
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() > 8) { 
            return true;
        }
            }
            if (dstx == srcx + 1 && srcx != 7&& srcy -1 ==dsty) {
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() > 8) {
            return true;
        }
    }
    }
    //black pawn
    if(bit.gameTag()==129&&player==1){
        if (dstx==srcx){
             if (srcy==1&&dsty==3&& getHolderAt(srcx,srcy+2).bit()==NULL){
                return true;
            }
            if (srcy+1==dsty&& getHolderAt(srcx,srcy+1).bit()==NULL){
                return true;
            }      
    }
           if (dstx == srcx - 1 && srcx != 0&& srcy +1 ==dsty) {
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() < 8) { 
            return true;
        }
            }
            if (dstx == srcx + 1 && srcx != 7&& srcy +1 ==dsty) {
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() < 8) {
            return true;
        }
    }
            
    }
    //white Bishop
    if(pieceType==Bishop && player ==0){
int xoffset=0;
int yoffset=0;
while(srcx+xoffset<7&&srcy+yoffset<7){
    xoffset++;
    yoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=9;
        yoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset<9 && yoffset<9){
        return true;
    }
}
 xoffset=0;
 yoffset=0;
 while(srcx+xoffset>0&&srcy+yoffset<7){
    xoffset--;
    yoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=-9;
        yoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset>-9 && yoffset<9){
        return true;
    }
}
 xoffset=0;
 yoffset=0;
while(srcx+xoffset<7&&srcy+yoffset>0){
    xoffset++;
    yoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=9;
        yoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset<9 && yoffset>-9){
        return true;
    }
}
xoffset=0;
 yoffset=0;
while(srcx+xoffset>0&&srcy+yoffset>0){
    xoffset--;
    yoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=-9;
        yoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset>-9 && yoffset>-9){
        return true;
    }
}
}
             //black Bishop
if(pieceType==Bishop && player ==1){
int xoffset=0;
int yoffset=0;
while(srcx+xoffset<7&&srcy+yoffset<7){
    xoffset++;
    yoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=9;
        yoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset<9 && yoffset<9){
        return true;
    }
}
 xoffset=0;
 yoffset=0;
 while(srcx+xoffset>0&&srcy+yoffset<7){
    xoffset--;
    yoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=-9;
        yoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset>-9 && yoffset<9){
        return true;
    }
}
 xoffset=0;
 yoffset=0;
while(srcx+xoffset<7&&srcy+yoffset>0){
    xoffset++;
    yoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=9;
        yoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset<9 && yoffset>-9){
        return true;
    }
}
xoffset=0;
 yoffset=0;
while(srcx+xoffset>0&&srcy+yoffset>0){
    xoffset--;
    yoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=-9;
        yoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset>-9 && yoffset>-9){
        return true;
    }
}
}
//black rook
if(pieceType==Rook && player==1){
    int xoffset=0;

while(srcx+xoffset<7){
    xoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy && xoffset<9){
        return true;
    }
}
xoffset=0;
while(srcx+xoffset>0){
    xoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy && xoffset>-9){
        return true;
    }
}
int yoffset=0;
while(srcy+yoffset>0){
    yoffset--;
    Bit* target = getHolderAt(srcx, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        yoffset=-9;
    }
    if (dstx==srcx&&dsty==srcy+yoffset && yoffset>-9){
        return true;
    }
}
    yoffset=0;

while(srcy+yoffset<7){
    yoffset++;
    Bit* target = getHolderAt(srcx, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        yoffset=9;
    }
    if (dstx==srcx&&dsty==srcy +yoffset&& yoffset<9){
        return true;
    }
}
}
//White Rook
if(pieceType==Rook && player==0){
    int xoffset=0;

while(srcx+xoffset<7){
    xoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy).bit();
    if (target != NULL && target->gameTag() <100){
        xoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy && xoffset<9){
        return true;
    }
}
xoffset=0;
while(srcx+xoffset>0){
    xoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy).bit();
    if (target != NULL && target->gameTag() <100){
        xoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy && xoffset>-9){
        return true;
    }
}
int yoffset=0;
while(srcy+yoffset>0){
    yoffset--;
    Bit* target = getHolderAt(srcx, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        yoffset=-9;
    }
    if (dstx==srcx&&dsty==srcy+yoffset && yoffset>-9){
        return true;
    }
}
    yoffset=0;

while(srcy+yoffset<7){
    yoffset++;
    Bit* target = getHolderAt(srcx, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        yoffset=9;
    }
    if (dstx==srcx&&dsty==srcy +yoffset&& yoffset<9){
        return true;
    }
}
}
if(pieceType==Queen && player==0){
     int xoffset=0;

while(srcx+xoffset<7){
    xoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy && xoffset<9){
        return true;
    }
}
xoffset=0;
while(srcx+xoffset>0){
    xoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy && xoffset>-9){
        return true;
    }
}
int yoffset=0;
while(srcy+yoffset>0){
    yoffset--;
    Bit* target = getHolderAt(srcx, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        yoffset=-9;
    }
    if (dstx==srcx&&dsty==srcy+yoffset && yoffset>-9){
        return true;
    }
}
    yoffset=0;

while(srcy+yoffset<7){
    yoffset++;
    Bit* target = getHolderAt(srcx, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        yoffset=9;
    }
    if (dstx==srcx&&dsty==srcy +yoffset&& yoffset<9){
        return true;
    }
}
xoffset=0;
yoffset=0;
while(srcx+xoffset<7&&srcy+yoffset<7){
    xoffset++;
    yoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=9;
        yoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset<9 && yoffset<9){
        return true;
    }
}
 xoffset=0;
 yoffset=0;
 while(srcx+xoffset>0&&srcy+yoffset<7){
    xoffset--;
    yoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=-9;
        yoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset>-9 && yoffset<9){
        return true;
    }
}
 xoffset=0;
 yoffset=0;
while(srcx+xoffset<7&&srcy+yoffset>0){
    xoffset++;
    yoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=9;
        yoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset<9 && yoffset>-9){
        return true;
    }
}
xoffset=0;
 yoffset=0;
while(srcx+xoffset>0&&srcy+yoffset>0){
    xoffset--;
    yoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() <127){
        xoffset=-9;
        yoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset>-9 && yoffset>-9){
        return true;
    }
}
}
if(pieceType==Queen && player==1){
     int xoffset=0;

while(srcx+xoffset<7){
    xoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy && xoffset<9){
        return true;
    }
}
xoffset=0;
while(srcx+xoffset>0){
    xoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy && xoffset>-9){
        return true;
    }
}
int yoffset=0;
while(srcy+yoffset>0){
    yoffset--;
    Bit* target = getHolderAt(srcx, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        yoffset=-9;
    }
    if (dstx==srcx&&dsty==srcy+yoffset && yoffset>-9){
        return true;
    }
}
    yoffset=0;

while(srcy+yoffset<7){
    yoffset++;
    Bit* target = getHolderAt(srcx, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        yoffset=9;
    }
    if (dstx==srcx&&dsty==srcy +yoffset&& yoffset<9){
        return true;
    }
}
xoffset=0;
yoffset=0;
while(srcx+xoffset<7&&srcy+yoffset<7){
    xoffset++;
    yoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=9;
        yoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset<9 && yoffset<9){
        return true;
    }
}
 xoffset=0;
 yoffset=0;
 while(srcx+xoffset>0&&srcy+yoffset<7){
    xoffset--;
    yoffset++;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=-9;
        yoffset=9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset>-9 && yoffset<9){
        return true;
    }
}
 xoffset=0;
 yoffset=0;
while(srcx+xoffset<7&&srcy+yoffset>0){
    xoffset++;
    yoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=9;
        yoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset<9 && yoffset>-9){
        return true;
    }
}
xoffset=0;
 yoffset=0;
while(srcx+xoffset>0&&srcy+yoffset>0){
    xoffset--;
    yoffset--;
    Bit* target = getHolderAt(srcx+xoffset, srcy+yoffset).bit();
    if (target != NULL && target->gameTag() >127){
        xoffset=-9;
        yoffset=-9;
    }
    if (dstx==srcx+xoffset&&dsty==srcy+yoffset && xoffset>-9 && yoffset>-9){
        return true;
    }
}
}
    return false;
}
