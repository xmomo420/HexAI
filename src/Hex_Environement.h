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

using namespace std;

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



  /***************************************************************************
   ******************************** Ajouts ***********************************
   ***************************************************************************/


    /*static double uct(const Noeud &noeudFils) {
        return (noeudFils.nombreVictoireSimulation / noeudFils.nombreSimulations)
               + sqrt(2) * sqrt(log(noeudFils.parent->nombreSimulations)
                                / noeudFils.nombreSimulations);
    }

    tuple<int, int> bestMove(const char &joueurIA) {
        // TODO : Générer tous les noeuds fils qui ne sont pas explorés
        genererPlateauxFils(joueurIA, noeudCourant);
        bool tousExplore = false;
        // TODO : Exlporer tous les noeuds fils à partir du noeud courant
        //  Si nbSimulation == 0, le noeud n'a pas été exploré
        while (!tousExplore) {
            // TODO : Simulation, dès que la partie se termine
            for (auto &noeud : noeudCourant->enfants)
                if (noeud->nombreSimulations == 0) {
                    noeudCourant->nombreSimulations++;
                    simuler(noeud);
                }
            tousExplore = true;
        }
        // TODO : Choisir l'action qui mène au noeud fils qui maximise la fonction UCT
        shared_ptr<Noeud> meilleurEnfant = meilleurNoeud(noeudCourant->enfants);
        return determinerAction(noeudCourant->etatPlateau,meilleurEnfant->etatPlateau);
    }

    void simuler( shared_ptr<Noeud> &fils) {
        simulation = true;
        tuple<int, int> action = determinerAction(noeudCourant->etatPlateau,
                                                  fils->etatPlateau);
        playMove(std::get<0>(action), std::get<1>(action));
        int compteur = 1;
        while (!isGameOver()) {
            playSimulation();
            compteur++;
        }
        if (getWinner() == getIa())
            noeudCourant->nombreVictoireSimulation++;
        for (int i = 0; i < compteur; i++) // Retourner à l'état original
            undo();
        simulation = false;
    }


    static tuple<int, int> determinerAction(const vector<vector<char>> &original,
                                            const vector<vector<char>> &resultat) {
        tuple<int, int> coup = {-1, -1};
        for (int i = 0; i < original.size(); ++i)
            for (int j = 0; j < original[i].size(); ++j)
                if (original[i][j] != resultat[i][j])
                    coup = {i, j};
        return coup;
    }

    static shared_ptr<Noeud> choisirAction(const vector<shared_ptr<Noeud>> &enfants) {
        shared_ptr<Noeud> meilleur = nullptr;
        double scoreUCT = -1.0;
        for (auto &enfant : enfants)
            if (uct(*enfant) > scoreUCT) {
                meilleur = enfant;
                scoreUCT = uct(*meilleur);
            }
        return meilleur;
    }


    static bool memePlateaux(const vector<vector<char>> &plateau,
                             const vector<vector<char>> &autrePlateau) {
        bool aucuneDifference = true;
        for (int i = 0; aucuneDifference && i < plateau.size(); i++)
            for (int j = 0; aucuneDifference && j < plateau[i].size(); j++)
                aucuneDifference = plateau[i][j] == autrePlateau[i][j];
        return aucuneDifference;
    }

    const vector<std::vector<char>> &getBoard() const;


    static int ajouterNoeudsFils(const vector<vector<char>> &plateau, shared_ptr<Noeud> &noeudParent) {
        bool noeudPresent = false;
        int i;
        for (i = 0; !noeudPresent && i < noeudParent->enfants.size(); ++i)
            noeudPresent = memePlateaux(plateau,noeudParent->enfants[i]->etatPlateau);
        if (!noeudPresent) {
            noeudParent->enfants.push_back(make_shared<Noeud>(plateau,noeudParent));
            return -1;
        } else
            return i;

    }


    static tuple<int, int> genererCoupValide(const vector<vector<char>> &plateau) {
        tuple<int, int> coup = {-1, -1};
        for (int i = 0; i < plateau.size(); ++i)
            for (int j = 0; j < plateau[i].size(); ++j)
                if (plateau[i][j] == '-')
                    coup = {i, j};
        return coup;
    }


    static int calculerNbCasesVides(const vector<vector<char>> &plateau) {
        int compteur = 0;
        for (auto &rangee : plateau)
            for (char _case : rangee)
                if (_case == '-')
                    ++compteur;
        return compteur;
    }


    void genererPlateauxFils(const char &joueurIA, shared_ptr<Noeud> &noeudParent) {
        vector<vector<char>> nouveauPlateau = vector<vector<char>>(noeudParent->etatPlateau);
        vector<vector<char>> plateauTmp = vector<vector<char>>(nouveauPlateau);
        // Calculer le nombre de cases vides, boucler autant de fois que ce
        // nombre, stocker les coups aléatoires dans plateauTmp
        int nombreCasesVides = calculerNbCasesVides(plateauTmp);
        for (int i = 0; i < nombreCasesVides; ++i) {
            tuple<int, int> coup = genererCoupValide(plateauTmp);
            nouveauPlateau[std::get<0>(coup)][std::get<1>(coup)] = joueurIA;
            plateauTmp[std::get<0>(coup)][std::get<1>(coup)] = joueurIA;
            ajouterNoeudsFils(nouveauPlateau, noeudParent);
            nouveauPlateau = vector<vector<char>>(noeudParent->etatPlateau);
        }
    }*/

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

    const vector<std::vector<char>> & getBoard() const {
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
