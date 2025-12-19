// chess.cpp 
// g++ -std=c++17 cpp/chess.cpp -o cpp/chess && ./cpp/chess
// шахматы
// todo: check/checkmate, stalemate, draw conditions, AI opponent
#include <iostream>
#include <vector>
int fifty_move_rule_counter = 0;
struct Piece {
    int ptype; // 0: pawn, 1: rook, 2: knight, 3: bishop, 4: queen, 5: king
    int x, y;
    bool isWhite;
    bool hasMoved = false;
    bool enPassantable = false;
    Piece() : ptype(-1), x(0), y(0), isWhite(false) {}
    Piece(int ptype, int x, int y, bool isWhite) : ptype(ptype), x(x), y(y), isWhite(isWhite) {}
};

struct Board {
    Piece* grid[8][8];
    Board() : grid() {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                grid[i][j] = nullptr;
            }
        }
    }
};
int move_piece (Piece &piece, Board &board, std::vector<Board> &board_history, int target_x, int target_y);

void printBoard(const Board& board) {
    char type_to_char[] = {'P', 'R', 'N', 'B', 'Q', 'K'};
    for (int j = 7; j >= 0; --j) {
        for (int i = 0; i < 8; ++i) {
            if (board.grid[i][j] != nullptr) {
                std::cout << (char)(type_to_char[board.grid[i][j]->ptype]+32*board.grid[i][j]->isWhite) << ' ';
            } else {
                std::cout << ". ";
            }
        }
        std::cout << '\n';
    }
};
int is_threatened(int x, int y, const Board &board, bool turn, std::vector<Board> &board_history) {
    //check for threats from all piece types
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* piece = board.grid[i][j];
            
            if (piece != nullptr) {
                Board temp_board = board;
                if (move_piece(*piece, temp_board, board_history, x, y) == 0) {
                    //std::cout << "Square (" << x << ", " << y << ") is threatened by piece at (" << i << ", " << j << ")\n";
                    return 1; // Square is threatened
                }
            }
        }
    }
    return 0; // Square is safe
}
int move_piece (Piece &piece, Board &board, std::vector<Board> &board_history, int target_x, int target_y) {
    auto move=[&board, &piece, target_x, target_y, &board_history]() {
        //handle check and pins
        Board temp_board = board;
        // Verify the move doesn't leave the king in check
        Piece* captured = board.grid[target_x][target_y];
        Piece* source = board.grid[piece.x][piece.y];

        // Make the move on temp board
        temp_board.grid[piece.x][piece.y] = nullptr;
        temp_board.grid[target_x][target_y] = &piece;

        // Find the king position after the move
        int king_x = piece.x, king_y = piece.y;
        if (piece.ptype == 5) { // If moving piece is the king
            king_x = target_x;
            king_y = target_y;
        } else {
            // Find friendly king
            for (int i = 0; i < 8; ++i) {
                for (int j = 0; j < 8; ++j) {
                    if (temp_board.grid[i][j] != nullptr &&
                        temp_board.grid[i][j]->ptype == 5 &&
                        temp_board.grid[i][j]->isWhite == piece.isWhite) {
                        king_x = i;
                        king_y = j;
                    }
                }
            }
        }

        // Check if king is in check after move
        if (is_threatened(king_x, king_y, temp_board, !piece.isWhite, board_history)) {
            return -1; // Move leaves king in check, invalid
        }

        // Move is valid, apply to real board
        board.grid[piece.x][piece.y] = nullptr;
        if (captured != nullptr) fifty_move_rule_counter=0; // Remove captured piece
        else if (piece.ptype == 0) fifty_move_rule_counter=0; // Reset fifty move rule counter on pawn move
        else {
            fifty_move_rule_counter++; // No capture, increment fifty move rule counter
        }
        board.grid[target_x][target_y] = &piece;
        piece.x = target_x;
        piece.y = target_y;
        piece.hasMoved = true;
        return 0;
    };
    auto check_path_clear=[&board, &piece, target_x, target_y]() {
        int dx = (target_x - piece.x) == 0 ? 0 : (target_x - piece.x) / abs(target_x - piece.x);
        int dy = (target_y - piece.y) == 0 ? 0 : (target_y - piece.y) / abs(target_y - piece.y);
        int x = piece.x + dx;
        int y = piece.y + dy;
        while (x != target_x || y != target_y) {
            if (board.grid[x][y] != nullptr) {
                return false; // Path is blocked
            }
            x += dx;
            y += dy;
        }
        return true; // Path is clear
    };
    
    
    if (target_x == piece.x && target_y == piece.y) {
        return -1; // No movement
    };
    if (target_x < 0 || target_x > 7 || target_y < 0 || target_y > 7) {
        return -1; // Out of bounds
    };
    if (board.grid[target_x][target_y] != nullptr &&
        board.grid[target_x][target_y]->isWhite == piece.isWhite) {
        return -1; // Cannot capture own piece
    };
    switch (piece.ptype)
    {
    case 0: // Pawn
        if (target_y == piece.y + (2*piece.isWhite)-1 && target_x == piece.x &&
            board.grid[target_x][target_y] == nullptr) {
            piece.enPassantable = false;
            move(); // Single-square move
        } else if (target_y == piece.y + (2*piece.isWhite)-1 && 
                   (target_x == piece.x + 1 || target_x == piece.x - 1) &&
                   board.grid[target_x][target_y] != nullptr &&
                   board.grid[target_x][target_y]->isWhite != piece.isWhite) {
        piece.enPassantable = false; 
        move();
        } else if ((piece.isWhite && piece.y == 1 && target_y == 3 && target_x == piece.x) ||
                   (!piece.isWhite && piece.y == 6 && target_y == 4 && target_x == piece.x) &&
                    board.grid[target_x][target_y+2*(piece.isWhite-1)] == nullptr) {
            piece.enPassantable = true;
            move(); // Two-square initial move
        } else if ((target_y == piece.y + (2*piece.isWhite)-1) &&
                    (target_x == piece.x + 1 || target_x == piece.x - 1) &&
                    board.grid[target_x][target_y] == nullptr &&
                    board.grid[target_x][target_y-((2*piece.isWhite)-1)] != nullptr &&
                    board.grid[target_x][target_y-((2*piece.isWhite)-1)]->ptype == 0 &&
                    board.grid[target_x][target_y-((2*piece.isWhite)-1)]->isWhite != piece.isWhite &&
                    board.grid[target_x][target_y-((2*piece.isWhite)-1)]->enPassantable &&
                    board_history[board_history.size()-2].grid[target_x][target_y-((2*piece.isWhite)-1)] == nullptr) {
            board.grid[target_x][target_y-((2*piece.isWhite)-1)] = nullptr; // Capture the pawn
            piece.enPassantable = false;
            move(); // En passant
        } else {
            return -1; // Invalid move
        };
        return 0;
    case 1: // Rook
        if (target_x != piece.x && target_y != piece.y) {
            return -1; // Rook moves only in straight lines
        };
        if (!check_path_clear()) {
            return -1; // Path is blocked
        };
        move();
        return 0;
    case 2: // Knight
        if (!((abs(target_x - piece.x) == 2 && abs(target_y - piece.y) == 1) ||
              (abs(target_x - piece.x) == 1 && abs(target_y - piece.y) == 2))) {
            return -1; // Invalid knight move
        }
        move();
        return 0;
    case 3: // Bishop
        if (abs(target_x - piece.x) != abs(target_y - piece.y)) {
            return -1; // Bishop moves only diagonally
        };
        if (!check_path_clear()) {
            return -1; // Path is blocked
        };
        move();
        return 0;
    case 4: // Queen
        if (target_x != piece.x && target_y != piece.y &&
            abs(target_x - piece.x) != abs(target_y - piece.y)) {
            return -1; // Queen moves only in straight lines or diagonally
        };
        if (!check_path_clear()) {
            return -1; // Path is blocked
        };
        move();
        return 0;
    case 5: // King
        if (!(abs(target_x - piece.x) > 1 || abs(target_y - piece.y) > 1)) {
            move();
            return 0; // King moves only one square in any direction
        } else if (!piece.hasMoved && target_y == piece.y &&
                   (target_x == piece.x + 2 || target_x == piece.x - 2)) {
            // Castling
            int rook_x = (target_x == piece.x + 2) ? 7 : 0;
            int step = (target_x == piece.x + 2) ? 1 : -1;
            if (board.grid[rook_x][piece.y] != nullptr &&
                board.grid[rook_x][piece.y]->ptype == 1 &&
                !board.grid[rook_x][piece.y]->hasMoved) {
                // Check path clear
                if (!check_path_clear()) {
                    return -1; // Path is blocked
                }
                for (int x = piece.x; x != target_x + step; x += step) {
                    if (is_threatened(x, piece.y, board, !piece.isWhite, board_history)) {
                        return -1; // Cannot castle through or into check
                    }
                }
                // Move king
                move();
                // Move rook
                Piece* rook = board.grid[rook_x][piece.y];
                board.grid[rook_x][piece.y] = nullptr;
                board.grid[piece.x - step][piece.y] = rook;
                rook->x = piece.x - step;
                rook->hasMoved = true;
                return 0;
            } else {
                return -1; // Invalid castling
            }
        }
        else {
            return -1; // Invalid king move
        }
    default: return -1; // Invalid piece type
    }
    return -1; // Invalid piece type
};

int input_move(Board &board, std::vector<Board> &board_history, bool turn) {
    int target_x, target_y, pos_x, pos_y;
    char move_input[5];
    std::cout << (turn ? "White's turn.\n" : "Black's turn.\n");
    std::cout << "Enter move (e.g., e2e4): ";
    std::cin >> move_input;
    if (move_input[0] == 'q') {
        std::cout << "Exiting game.\n";
        return 1; // Exit command
    } else if (sizeof(move_input) != 5) {
        return -1; // Invalid input length
    }
    pos_x = move_input[0] - 'a';
    pos_y = move_input[1] - '1';
    target_x = move_input[2] - 'a';
    target_y = move_input[3] - '1';
    
    if (target_x < 0 || target_x > 7 || target_y < 0 || target_y > 7 || 
        pos_x < 0 || pos_x > 7 || pos_y < 0 || pos_y > 7) {
        return -1; // Out of bounds
    };
    if (board.grid[pos_x][pos_y] == nullptr) {
        return -1; // No piece at source position
    } else if (board.grid[pos_x][pos_y]->isWhite != turn) {
        return -1; // Not the player's turn
    };
    return move_piece(*board.grid[pos_x][pos_y], board, board_history, target_x, target_y);
};

int game_loop(Board &board, std::vector<Board> &board_history, int result=0, bool turn=true) {
    auto promote = [](Piece &piece, Board &board) {

    char promotion_choices[] = {'Q', 'R', 'B', 'N'};
    char choice;
    std::cout << "Promote pawn at " << char('a' + piece.x) << (piece.y + 1) << " to (Q, R, B, N): ";
    std::cin >> choice;
    if ((piece.isWhite && piece.y == 7) || (!piece.isWhite && piece.y == 0)) {
        switch (choice) { 
            case 'Q':
                piece.ptype = 4;
                break;
            case 'R':
                piece.ptype = 1;
                break;
            case 'B':
                piece.ptype = 3;
                break;
            case 'N':
                piece.ptype = 2;
                break;
            default:
                piece.ptype = 4; // Default to queen
                break;
        }
    };
    };
    result = input_move(board, board_history, turn);
    if (result == -1) {
        std::cout << "Invalid move, try again.\n";
        printBoard(board);
        return game_loop(board, board_history, result, turn);
    } else if (result == 1)  return 0;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; j+=7) {
            if (board.grid[i][j] != nullptr){
            if (board.grid[i][j]->ptype == 0) {
                promote(*board.grid[i][j], board);
            }}
        }
    };
    board_history.push_back(board);
    printBoard(board);
    //check for checkmate and stalemate
    int king_x, king_y;
    for (int i=0; i<8; i++) {
        for (int j=0; j<8; ++j) {
            if (board.grid[j][i] != nullptr) {
                if (board.grid[j][i]->ptype == 5 && board.grid[j][i]->isWhite == turn) {
                    int king_x = j;
                    int king_y = i;
                    goto found_king;
                }
            }
        }
    }
    found_king:
    bool legal_move_exists = false;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            if (board.grid[i][j] != nullptr && board.grid[i][j]->isWhite != turn) {
                if (move_piece(*board.grid[i][j], board, board_history, king_x, king_y) == 0) {
                    legal_move_exists = true;
                    return 0;
                }
            }
        }
    }
    if (!legal_move_exists) {
        if (is_threatened(king_x, king_y, board, turn, board_history)) {
            std::cout << (turn ? "White is in checkmate.\n" : "Black is in checkmate.\n");
        } else {
            std::cout << "Stalemate.\n";
        }
        return 0;
    }
    //check for draw conditions
    if (fifty_move_rule_counter >= 50) {
        std::cout << "Draw by fifty-move rule.\n";
        return 0;
    }
    //threefold repetition rule
    int repetition_count = 0;
    for (auto &history_board : board_history) {
        for (int i = 0; i < 8; ++i) {
            for (int j = 0; j < 8; ++j) {
                if ((board.grid[i][j] == nullptr && history_board.grid[i][j] != nullptr) ||
                    (board.grid[i][j] != nullptr && history_board.grid[i][j] == nullptr) ||
                    (board.grid[i][j] != nullptr && history_board.grid[i][j] != nullptr &&
                     board.grid[i][j]->ptype != history_board.grid[i][j]->ptype)) {
                    goto not_repeated;
                }
            }
        }
        repetition_count++;
        not_repeated:;
    }
    if (repetition_count >= 3) {
        std::cout << "Draw by threefold repetition.\n";
    }

    return game_loop(board, board_history, result, !turn);
}
int main() {
    std::vector<Board> board_history;
    Board board;
    auto setupBoard = [&board, &board_history]() {
        int j = 0;
        for (int i = 0; i < 8; ++i) {
            board.grid[i][1] = new Piece(0, i, 1, true);
            board.grid[i][6] = new Piece(0, i, 6, false);
        };
        int back_rank[]={1, 2, 3, 4, 5, 3, 2, 1};
        for (int i = 0; i < 8; ++i) {
            board.grid[i][0] = new Piece(back_rank[i], i, 0, true);
            board.grid[i][7] = new Piece(back_rank[i], i, 7, false);
        }
        
    };
    
    setupBoard();
    printBoard(board);
    board_history.push_back(board);
    return game_loop(board, board_history);
}
