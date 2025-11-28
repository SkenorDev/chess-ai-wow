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
