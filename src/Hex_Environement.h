#ifndef Hex_Environement__H
#define Hex_Environement__H

#include <vector>
#include <tuple>
#include <set>
#include <cassert>
#include <ncurses.h>
#include <string>
#include <memory>
#include <iostream>
#include <map>
#include <cmath>


class Hex_Environement;

struct Player_Interface {
    // Méthode appelée lorsque l'autre joueur joue un coup
    virtual void otherPlayerMove(int row, int col) { };

    // Méthode appelée pour demander à l'IA de jouer un coup
    virtual std::tuple<int, int> getMove(Hex_Environement& hex) = 0;
};

class Humain : public Player_Interface {
public:
    std::tuple<int, int> getMove(Hex_Environement& hex) override;
    virtual void otherPlayerMove(int row, int col) { std::cerr << "L'autre joueur a joué le coup " << row << col << std::endl; };
};


class Hex_Environement {
    std::vector< std::tuple<unsigned int, unsigned int> > historique_coups;
    std::vector<std::vector<char>> board;
    char _player = 'X';

    std::unique_ptr<Player_Interface> playerX;
    std::unique_ptr<Player_Interface> playerO;

    bool _withGUI;
    bool _gameOver = false;


public:

    unsigned int getSize() const {
        return board.size();
    }

    void setPlayerX(std::unique_ptr<Player_Interface> && player) {
        playerX = std::move(player);
    }

    void setPlayerO(std::unique_ptr<Player_Interface> && player) {
        playerO = std::move(player);
    }

    Hex_Environement(bool withGUI=true, unsigned int taille=10) : _withGUI(withGUI) {
        board.resize(taille, std::vector<char>(taille, '-'));
        playerX = std::make_unique<Humain>();
        playerO = std::make_unique<Humain>();

        if(withGUI) {
            initscr();
            cbreak();
            noecho();
            keypad(stdscr, TRUE);
            mousemask(ALL_MOUSE_EVENTS, NULL);
            curs_set(FALSE);

            start_color(); 
            init_pair('-', COLOR_WHITE, COLOR_BLACK);
            init_pair('X', COLOR_WHITE, COLOR_RED);
            init_pair('O', COLOR_WHITE, COLOR_BLUE);
        }
    }

    ~Hex_Environement() {
        if(_withGUI) {
            endwin();
        }
    }

    // Réafficher le plateau de jeu
    void printBoard() const {
        if(_withGUI) {
            clear();
            printw("   ");
            attron(COLOR_PAIR('O'));
            for(unsigned int j=0; j<board.size(); j++) {
                if(j < 10)
                    printw(" %d  ", j);
                else
                    printw(" %d ", j);
            }
            printw(" ");
            attroff(COLOR_PAIR('O'));
            
            printw("\n");
            unsigned int i = 0;
            for(auto &line: board) {

                for(unsigned int j=0; j<i; j++) {
                    printw("  ");
                }

                attron(COLOR_PAIR('X'));
                if(i < 10)
                    printw(" %d  ", i);
                else
                    printw(" %d ", i);
                attroff(COLOR_PAIR('X'));

                for(auto c: line) {
                    attron(COLOR_PAIR(c));
                        printw(" %c  ", c);
                    attroff(COLOR_PAIR(c));
                }

                attron(COLOR_PAIR('X'));
                printw(" %d  ", i);
                attroff(COLOR_PAIR('X'));


                printw("\n");
                i++;
            }
            
            for(unsigned int j=0; j<=i; j++) {
                printw("  ");
            }
            attron(COLOR_PAIR('O'));
            for(unsigned int j=0; j<board.size(); j++) {
                if(j < 10)
                    printw("  %d ", j);
                else
                    printw("  %d", j);
            }
            attroff(COLOR_PAIR('O'));
            printw("\n");

            printw("Player: %c\n", _player);
            refresh();
        } else {
            std::cerr << "Au joueur " << _player << " de jouer : " << std::flush;
        }
    }

    // Vérifier si le coup est valide
    bool isValidMove(int x, int y) const {
        if (x < 0 || x >= (int)(board.size()) || y < 0 || y >= (int)(board[x].size())) return false;  // Vérifier si la case est dans les limites du plateau de jeu
        if (board[x][y] != '-') return false;  // Vérifier si la case est vide

        return true;
    }

    // Annuler le dernier coup
    void undo() {
        if(historique_coups.size() == 0) return;
        auto [x, y] = historique_coups.back();
        historique_coups.pop_back();
        board[x][y] = '-';
        _player = (_player == 'X') ? 'O' : 'X';  // Changer de joueur
    }

    // Jouer le coup (x, y) et retourner true si c'est la fin de la partie apres le move
    bool playMove(int x, int y) {
        assert( isValidMove(x, y) );

        board[x][y] = _player;  // Placer la pièce du joueur actuel sur la case choisie
        historique_coups.push_back({x, y});

        // Vérifier si le joueur actuel a gagné
        std::set<std::pair<int, int>> visited = {{x, y}};
        std::vector<std::pair<int, int>> toVisit = {{x, y}};
        bool side1 = false;
        bool side2 = false;
        while( toVisit.size() ) {
            auto [i, j] = toVisit.back();
            toVisit.pop_back();

            if(_player == 'X') {
                if(j == 0) {
                    side1 = true;
                }
                if(j == (int)board.size()-1) {
                    side2 = true;
                }
            } else {
                assert(_player == 'O');
                if(i == 0) {
                    side1 = true;
                }
                if(i == (int)board.size()-1) {
                    side2 = true;
                }
            }

            if(side1 && side2) {
                _player = (_player == 'X') ? 'O' : 'X';  // Changer de joueur
                return true;
            }

            for(int dx=-1; dx<=1; dx++) {
                for(int dy=-1; dy<=1; dy++) {
                    if(dx == 0 && dy == 0) continue;
                    if(dx == 1 && dy == 1) continue;
                    if(dx == -1 && dy == -1) continue;
                    if(i+dx < 0 || i+dx >= (int)board.size() || j+dy < 0 || j+dy >= (int)board.size()) continue;

                    if(board[i+dx][j+dy] == _player) {
                        if(visited.count({i+dx, j+dy}) == 0) {
                            visited.insert({i+dx, j+dy});
                            toVisit.push_back({i+dx, j+dy});
                        }
                    }
                }
            }    
        }

        _player = (_player == 'X') ? 'O' : 'X';  // Changer de joueur
        return false;
    }

    void play() {
        std::tuple<int, int> move;
        if(getPlayer() == 'O') {
            move = playerO->getMove(*this);
        } else {
            assert(getPlayer() == 'X');
            move = playerX->getMove(*this);
        }

        if(isValidMove(std::get<0>(move), std::get<1>(move))) {
            _gameOver = playMove(std::get<0>(move), std::get<1>(move));
        } else {
            printw("Invalid move\n");
            _gameOver = true;
        }

        // Envoi du coup à l'autre joueur
        if(_player == 'X') {
            playerX->otherPlayerMove(std::get<0>(move), std::get<1>(move));
        } else {
            assert(_player == 'O');
            playerO->otherPlayerMove(std::get<0>(move), std::get<1>(move));
        }
    }

    char getWinner() const {
        if(!_gameOver) return '?';
        return ((_player == 'X') ? 'O' : 'X');
    }

    // Afficher le gagnant
    void afficherFin() const {
        assert(isGameOver());
        printBoard();
        printw("Player %c a gagner !!\n", getWinner());
    }

    // Retourner le joueur actuel
    char getPlayer() const {
        return _player;
    }

    // Retourner le contenu de la case (row, col)
    char get(int row, int col) const {
        return board[row][col];
    }
    
    // Retourner le contenu de la case ij
    char get(const std::tuple<int, int> &ij) const {
        return board[std::get<0>(ij)][std::get<1>(ij)];
    }

    bool isGameOver() const {
        return _gameOver;
    }


    bool isWithGUI() const {
        return _withGUI;
    }

    const std::vector<std::vector<char>> & getBoard() const {
        return board;
    }

};

inline std::tuple<int, int>  Humain::getMove(Hex_Environement& hex) {
    int row, col;
    while(true) {
        if(hex.isWithGUI()) {
            int c = getch();
            if (c == KEY_MOUSE) {
                MEVENT event;
                getmouse(&event);
                row = event.y-1;
                col = ( (event.x - 2*row) / 4 ) - 1;
                if (hex.isValidMove(row, col)) {
                    break;
                }
            }
        } else {
            std::string coups;
            std::cin >> coups;
            if(coups.size() != 2) {
                std::cerr << "Invalid move" << std::endl;
                continue;
            }
            row = coups[0]-'0';
            col = coups[1]-'0';
            if(hex.isValidMove(row, col)) {
                break;
            } else {
                std::cerr << "Invalid move" << std::endl;
            }
        }
    }
    return {row, col};
}

#endif
