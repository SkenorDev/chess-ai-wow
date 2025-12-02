bool Chess::canBitMoveFromTo(Bit &bit, BitHolder &src, BitHolder &dst)
{
    ChessSquare* srcxy = static_cast<ChessSquare*>(&src);
    ChessSquare* dstxy = static_cast<ChessSquare*>(&dst);
     int srcx = srcxy->getColumn();
    int srcy = srcxy->getRow();
         int dstx =dstxy->getColumn();
    int dsty = dstxy->getRow();
    int player = getCurrentPlayer()->playerNumber();
    //White pawn
    if(bit.gameTag()==1&&player==0){
        printf("%d %d \n",dstx, dsty);
        if (dstx==srcx){
             if (srcy==6&&dsty==4&& getHolderAt(srcx,srcy-2).bit()==NULL){
                return true;
            }
            if (srcy-1==dsty&& getHolderAt(srcx,srcy-1).bit()==NULL){
                return true;
            }      
    }
             if (dstx == srcx - 1 && srcx != 0) {
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() > 8) { 
            return true;
        }
            }
            if (dstx == srcx + 1 && srcx != 7) {
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() > 8) {
            return true;
        }
    }
    }
    //Black Pawn
    if(bit.gameTag()==129&&player==1){
        if (dstx==srcx){
             if (srcy==1&&dsty==3&& getHolderAt(srcx,srcy+2).bit()==NULL){
                return true;
            }
            if (srcy+1==dsty&& getHolderAt(srcx,srcy+1).bit()==NULL){
                return true;
            }      
    }
           if (dstx == srcx - 1 && srcx != 0) {
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() < 8) { 
            return true;
        }
            }
            if (dstx == srcx + 1 && srcx != 7) {
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() < 8) {
            return true;
        }
    }
            
    }
    //White Knight
    if(bit.gameTag()==2&&player==0){
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() < 9) {
            return false;
        }
    if(srcx + 1 == dstx && srcy + 2 == dsty) return true;
if(srcx - 1 == dstx && srcy + 2 == dsty) return true;
if(srcx + 2 == dstx && srcy + 1 == dsty) return true;
if(srcx + 2 == dstx && srcy - 1 == dsty) return true;
if(srcx - 2 == dstx && srcy + 1 == dsty) return true;
if(srcx - 2 == dstx && srcy - 1 == dsty) return true;
if(srcx + 1 == dstx && srcy - 2 == dsty) return true;
if(srcx - 1 == dstx && srcy - 2 == dsty) return true;
    }
      //Black Knight
    if(bit.gameTag()==130&&player==1){
        Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() > 9) {
            return false;
        }
if(srcx + 1 == dstx && srcy + 2 == dsty) return true;
if(srcx - 1 == dstx && srcy + 2 == dsty) return true;
if(srcx + 2 == dstx && srcy + 1 == dsty) return true;
if(srcx + 2 == dstx && srcy - 1 == dsty) return true;
if(srcx - 2 == dstx && srcy + 1 == dsty) return true;
if(srcx - 2 == dstx && srcy - 1 == dsty) return true;
if(srcx + 1 == dstx && srcy - 2 == dsty) return true;
if(srcx - 1 == dstx && srcy - 2 == dsty) return true;
    }
    if(bit.gameTag()==6&&player==0){
                Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() < 9) {
            return false;
        }
        if(srcx+1==dstx) return true;
         if(srcx-1==dstx) return true;
          if(srcy+1==dsty) return true;
           if(srcy-1==dsty) return true;
            if(srcx+1==dstx&&srcy+1==dsty) return true;
             if(srcx+1==dstx&&srcy-1==dsty) return true;
            if(srcx-1==dstx&&srcy+1==dsty) return true;
             if(srcx-1==dstx&&srcy-1==dsty) return true;
    }
        if(bit.gameTag()==134&&player==1){
                Bit* target = getHolderAt(dstx, dsty).bit();
        if (target != NULL && target->gameTag() > 9) {
            return false;
        }
                if(srcx+1==dstx) return true;
         if(srcx-1==dstx) return true;
          if(srcy+1==dsty) return true;
           if(srcy-1==dsty) return true;
            if(srcx+1==dstx&&srcy+1==dsty) return true;
             if(srcx+1==dstx&&srcy-1==dsty) return true;
            if(srcx-1==dstx&&srcy+1==dsty) return true;
             if(srcx-1==dstx&&srcy-1==dsty) return true;
    }
    return false;
}
void Chess::addPawnBitboardMovesToList(std::vector<BitMove>& moves, const BitBoard pawns){
    if(bitboard.getData()==0)
    return;
    bitboard.forEachBit([&](int toSquare){
        int fromSquare = toSquare - shift;
        moves.emplace_back(fromSquare, toSquare,Pawn);
    });
}
#define WHITE 1
#define BLACK -1
void Chess::generatePawnMoves(std::vector<BitMove>& moves, BitBoard pawnBoard, uint64_t occupancy) {
    if (pawns.getData()==0)
    return;

    constexpr uint64_t NotAFile = 0xFEFEFEFEFEFEFEFEULL;
    constexpr uint64_t NotHFile = 0x7F7F7F7F7F7F7F7FULL;
    constexpr uint64_t Rank3 = 0x0000000000FF0000ULL;
    constexpr uint64_t Rank6 = 0x0000FF0000000000ULL;
    BitBoard demoRight(NotAfile);
    BitBoard demoRight(NotHfile);

    BitBoard singleMoves = (color == WHITE) ? (pawns.getData()<< 8) & emptySquares.getData() : (pawns.getData()>>8) & emptySquares.getData();
    BitBoard doubleMoves = (color == WHITE) ? ((singleMoves.getData() & Rank3)<<8) & emptySquares.getData() : ((singleMoves.getData()& Rank6)>>8) & emptySquares.getData();

    BitBoard capturesLeft = (color == WHITE) ? ((pawns.getData() & NotHFile) << 7) & enemyPieces.getData() : ((pawns.getData() & NotHFile)>>9) & enemyPieces.getData();
    BitBoard capturesRight = (color == WHITE) ? ((pawns.getData() & NotHFile) << 9)& enemyPieces.getData() :((pawns.getData() & NotHFile)>>7) & enemyPieces.getData();
    int shiftForward = (color == WHITE) ? 8 :-8;
    int doubleShift = (color == WHITE) ? 16 :-16;
    int captureLeftShift = (color == WHITE) ? 7 :-9;
    int captureRightShift = (color == WHITE) ? 9 :-7;
    addPawnBitboardMovesToList(moves,singleMoves, shiftForward);
    addPawnBitboardMovesToList(moves,doubleMoves, doubleShift);
    addPawnBitboardMovesToList(moves,capturesLeft, captureLeftShift);
    addPawnBitboardMovesToList(moves,capturesRight, captureRightShift);
}
 // Make the best move


// std::vector<BitMove> Chess::generateAllMoves(){
// std::vector<BitMove> moves;
// moves.reserve(32);
// std::string state = stateString();
// for (int i=0; i<e_numBitboards;i++){
//     _bitboards[bitIndex] |= 1ULL << i;
// }
// _bitboards[WHITE_ALL_PIECES] = _bitboards[WHITE_PAWNS].getData() |
//  _bitboards[WHITE_KNIGHTS].getData() |
//   _bitboards[WHITE_BISHOPS].getData() |
//    _bitboards[WHITE_ROOKS].getData() |
//     _bitboards[WHITE_QUEENS].getData() |
//      _bitboards[WHITE_KING].getData();

//      _bitboards[BLACK_ALL_PIECES] = _bitboards[BLACK_PAWNS].getData() |
//  _bitboards[BLACK_KNIGHTS].getData() |
//   _bitboards[BLACK_BISHOPS].getData() |
//    _bitboards[BLACK_ROOKS].getData() |
//     _bitboards[BLACK_QUEENS].getData() |
//      _bitboards[BLACK_KING].getData();

//      _bitboards[OCCUPANCY] = _bitboards[WHITE_ALL_PIECES].getData() | _bitboards[BLACK_ALL_PIECES];

//      int bitIndex = _currentPlayer == WHITE ? WHITE_PAWNS : BLACK_PAWNS;
//     generateKnightMoves(moves, _bitboards[WHITE_KNIGHTS + bitIndex], ~_bitboards[OCCUPANCY].getData());
//     generatePawnMoves(moves, _bitboards[WHITE_PAWNS + bitIndex], ~_bitboards[OCCUPANCY].getData(), _bitboards[WHITE_ALL_PIECES + oppBitIndex].getData());
//     generateKingMoves(moves, _bitboards[WHITE_KING + bitIndex], ~_bitboards[OCCUPANCY].getData());
//     generateRookMoves(moves, _bitboards[WHITE_ROOKS + bitIndex], _bitboards[OCCUPANCY].getData(), _bitboards[WHITE_ALL_PIECES + BitIndex].getData());
//     generateBishopMoves(moves, _bitboards[WHITE_BISHOPS + bitIndex], _bitboards[OCCUPANCY].getData(), _bitboards[WHITE_ALL_PIECES + BitIndex].getData());
//     generateQueenMoves(moves, _bitboards[WHITE_QUEENS + bitIndex], _bitboards[OCCUPANCY].getData(), _bitboards[WHITE_ALL_PIECES + BitIndex].getData());
//      return moves;
// }
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

                if (board[to] == '\0' || !sameSide(p, board[to])) {
                    moves.emplace_back(sq, to, Knight);
                    
                }
            }
            continue;
        }
        if (p == 'k') {
 static const int kingMoves[8] = {
                1, -1, 8, -8, 9, -9, 7, -7
            };
            for (int d :  kingMoves) {
                int to = sq + d;
                if (to < 0 || to >= 64) continue;

            int f1 = fileOf(sq);
                int f2 = fileOf(to);
                if (abs(f1 - f2) > 2) continue;

                if (board[to] == '\0' || !sameSide(p, board[to])) {
                    moves.emplace_back(sq, to, King);
                    continue;
                }
            }
        }
         if (p == 'b') {
            static const int dirs[4] = { 9, -9, 7, -7 };
            for (int d : dirs) {
                int to = sq;
                while (true) {
                    int f1 = fileOf(to);
                    to += d;
                    if (to < 0 || to >= 64) break;

                    int f2 = fileOf(to);
                    if (abs(f1 - f2) != 1) break;  // wrapped

                    if (board[to] == '\0') {
                        moves.emplace_back(sq, to, Bishop);
                    } else {
    if (!sameSide(p, board[to])) {
        moves.emplace_back(sq, to, Bishop);
    }
    break;  // stop sliding in this direction
}
                }
            }
        }
        if (p == 'r') {
            static const int dirs[4] = { 8, -8, 1, -1 };
            for (int d : dirs) {
                int to = sq;
                while (true) {
                    int f1 = fileOf(to);
                    to += d;
                    if (to < 0 || to >= 64) break;

                    int f2 = fileOf(to);
                    if (d == 1 || d == -1) {
                        if (f1 == f2) {
                        } else {
                            break;
                        }
                    }

                    if (board[to] == '\0') {
                        moves.emplace_back(sq, to, Rook);
                    } else {
                        if (!sameSide(p, board[to])) {
                            moves.emplace_back(sq, to, Rook);
                        }
                       continue;
                    }
                }
            }
        }
         if (p == 'q') {
            static const int dirs[8] = { 
                8, -8, 1, -1, 9, -9, 7, -7 
            };
            for (int d : dirs) {
                int to = sq;
                while (true) {
                    int f1 = fileOf(to);
                    to += d;
                    if (to < 0 || to >= 64) break;

                    int f2 = fileOf(to);

                    // Horizontal wrap check
                    if (d == 1 || d == -1) {
                        if (f1 != f2) break;
                    }
                    // Diagonal wrap check
                    if (d == 9 || d == -9 || d == 7 || d == -7) {
                        if (abs(f1 - f2) != 1) break;
                    }

                    if (board[to] == '\0') {
                        moves.emplace_back(sq, to, Queen);
                    } else {
                        if (!sameSide(p, board[to])) {
                            moves.emplace_back(sq, to, Queen);
                        }
                       continue;
                    }
                }
            }
        }