#ifndef IA_PLAYER__H
#define IA_PLAYER__H

#include <tuple>
#include <vector>
#include <memory>
#include <map>
#include <cassert>
#include <limits>
#include <cmath>
#include <string>

#include "Hex_Environement.h"

using namespace std;

class IA_Player : public Player_Interface {

private:
    char _player;
    char _otherPLayer;
    unsigned int _taille;

    // Nœud dans l'arbre
    struct Noeud {
        // Constructeurs
        Noeud(const vector<vector<char>> &board, const shared_ptr<Noeud> &parent, const bool &estRacine);
        Noeud();
        // Attributs
        vector<vector<char>> _board = vector<vector<char>>();
        double nbSimulations = 0.0;
        double nbVictoires = 0.0;
        shared_ptr<Noeud> _parent;
        vector<shared_ptr<Noeud>> enfants = vector<shared_ptr<Noeud>>();
    };

    shared_ptr<Noeud> noeudCourant;

public:
    IA_Player(char player, unsigned int taille=10) : _player(player), _taille(taille) {
        assert(player == 'X' || player == 'O');
        _otherPLayer =  _player == 'X' ? 'O' : 'X';
        shared_ptr<Noeud> parent = make_shared<Noeud>();
        vector<vector<char>> emptyBoard;
        emptyBoard.resize(taille, std::vector<char>(taille, '-'));
        noeudCourant = make_shared<Noeud>(emptyBoard, parent, true);
    }

    void otherPlayerMove(int row, int col) override {
        // l'autre joueur a joué (row, col)
        // Mettra à jour le noeud courant
        mettreNoeudAJour({row, col}, false, _otherPLayer);
    }

    tuple<int, int> getMove(Hex_Environement& hex) override {
        // TODO
        tuple<int, int> coup = choisirAction();
        mettreNoeudAJour(coup, false, _player);
        return coup;
    }

/**************************************************************************
 ************************ Déclarations de fonctions ***********************
 **************************************************************************/

/**
 *
 */
    static void afficherPlateau(const vector<vector<char>> &board) ;

/**
 *
 * @param original
 * @param resultat
 * @return
 */
static tuple<int, int> determinerAction(const vector<vector<char>> &original,
                                        const vector<vector<char>> &resultat);

/**
 *
 * @return
 */
tuple<int, int> choisirAction();

/**
 *
 * @param board
 * @return
 */
static int calculerNbCasesVides(const vector<vector<char>> &board);

/**
 *
 * @return
 */
static double calculerUCT(const shared_ptr<Noeud> &noeud);

/**
 *
 * @param enfants
 * @return
 */
tuple<int, int> meilleureAction(vector<shared_ptr<Noeud>> &enfants) const;

/**
 *
 * @param board
 * @return
 */
static tuple<int, int> coupAleatoire(const vector<vector<char>> &board);

/**
 *
 * @param board
 * @param row
 * @param col
 * @return
 */
static bool isValidMove(const vector<vector<char>> &board,
                        const int &row, const int &col);

/**
 *
 * @param action
 * @param simulation
 * @param joueur
 */
void mettreNoeudAJour(const tuple<int, int> &action,
                      const bool &simulation, const char &joueur);

/**
 *
 * @param plateauEnfant
 * @param noeudParent
 */
static shared_ptr<Noeud> & ajouterNoeudEnfant(const vector<vector<char>> &plateauEnfant,
                                              shared_ptr<Noeud> &noeudParent);
/**
 *
 * @param plateau
 * @param autre
 * @return
 */
static bool memePlateaux(const vector<vector<char>> &plateau, const vector<vector<char>> &autre);

/**
 *
 * @param action
 * @param noeudFils
 */
void simuler(shared_ptr<Noeud> &noeudFils);

/**
 *
 * @param noeudCourant
 * @param compteur
 */
static void revenir(shared_ptr<Noeud> &noeudCourant, const int &compteur);

/**
 *
 * @param action
 * @param joueur
 * @return
 */
    bool playSimulation(const tuple<int, int> &action, const char &joueur);

/**
 *
 * @return
 */
    void genererPlateauxFils();

/**
 *
 * @param board
 * @return
 */
static tuple<int, int> coupValide(const vector<vector<char>> &board);

};


/***************************************************************************
 *********************** Définition de fonctions ***************************
 ***************************************************************************/

IA_Player::Noeud::Noeud(const vector<vector<char>> &board, const shared_ptr<Noeud> &parent,
                        const bool &estRacine) : _board(board) {
    if (estRacine) {
        shared_ptr<Noeud> ptrParent = nullptr;
        _parent = ptrParent;
    } else
        _parent = parent;
}

IA_Player::Noeud::Noeud() = default;

tuple<int, int> IA_Player::coupAleatoire(const vector<vector<char>> &board) {
    int indiceCoup;
    vector<tuple<int, int>> coupsPossibles = vector<tuple<int, int>>();
    for (int i = 0; i < board.size(); ++i)
        for (int j = 0; j < board[i].size(); ++j)
            if (board[i][j] == '-') // Case valide
                coupsPossibles.emplace_back(i, j);
    indiceCoup = rand()%coupsPossibles.size();
    return coupsPossibles[indiceCoup];
}

bool IA_Player::isValidMove(const vector<vector<char>> &board,
                            const int &row, const int &col) {
    bool outOfBounds = row < 0 || col < 0 || row >= board.size() || col >= board[0].size();
    bool caseVide = board[row][col] == '-';
    return !outOfBounds && caseVide;
}

void IA_Player::mettreNoeudAJour(const tuple<int, int> &action,
                                 const bool &simulation, const char &joueur) {
    // Mettre à jour le plateau de jeu
    vector<vector<char>> plateauEnfant = noeudCourant->_board;
    plateauEnfant[get<0>(action)][get<1>(action)] = joueur;
    // Ajoute le nœud enfant, dans le vecteur d'enfant du nœud courant, s'il n'est pas déja dedans.
    shared_ptr<Noeud> nouveauNoeud = ajouterNoeudEnfant(plateauEnfant, noeudCourant);
    if (simulation) // Gestion simulation
        nouveauNoeud->nbSimulations++;
    // MAJ le nœud courant
    noeudCourant = nouveauNoeud;
}

shared_ptr<IA_Player::Noeud> &
        IA_Player::ajouterNoeudEnfant(const vector<vector<char>> &plateauEnfant,
                                      shared_ptr<Noeud> &noeudParent) {
    bool noeudPresent = false;
    int i;
    for (i = 0; !noeudPresent && i < noeudParent->enfants.size(); ++i)
        noeudPresent = memePlateaux(plateauEnfant, noeudParent->enfants[i]->_board);
    if (!noeudPresent) {
        shared_ptr<Noeud> nouveauNoeud = make_shared<Noeud>(plateauEnfant, noeudParent, false);
        noeudParent->enfants.push_back(nouveauNoeud);
        return noeudParent->enfants.back();
    } else
        return noeudParent->enfants[--i];

}

bool IA_Player::memePlateaux(const vector<vector<char>> &plateau,
                             const vector<vector<char>> &autre) {
    bool aucuneDifference = true;
    for (int i = 0; aucuneDifference && i < plateau.size(); i++)
        for (int j = 0; aucuneDifference && j < plateau[i].size(); j++)
            aucuneDifference = plateau[i][j] == autre[i][j];
    return aucuneDifference;
}

double IA_Player::calculerUCT(const shared_ptr<Noeud> &noeud) {
    if (noeud->nbSimulations == 0.0) {
        cerr << "UCT = 0\n";
        return 0.0;
    } else {
        double uct = (noeud->nbVictoires / noeud->nbSimulations)
                     + sqrt(2.0) * sqrt(log(noeud->_parent->nbSimulations)
                                        / noeud->nbSimulations);
        cerr << "UCT : " << uct << endl;
        return (noeud->nbVictoires / noeud->nbSimulations)
               + sqrt(2.0) * sqrt(log(noeud->_parent->nbSimulations)
                                  / noeud->nbSimulations);
    }

}

tuple<int, int> IA_Player::meilleureAction(vector<shared_ptr<Noeud>> &enfants) const {
    double scoreUCT = -1.0;
    double scoreUCTEnfant;
    tuple<int, int> action = {-1, -1};
    for (auto &noeud : enfants) {
        scoreUCTEnfant = calculerUCT(noeud);
        if (scoreUCTEnfant >= scoreUCT) { // Rajouter une chance de 50%
            if (scoreUCTEnfant > scoreUCT) {
                scoreUCT = scoreUCTEnfant;
                action = determinerAction(noeudCourant->_board, noeud->_board);
            } else if (rand()%2 == 0) {
                scoreUCT = scoreUCTEnfant;
                action = determinerAction(noeudCourant->_board, noeud->_board);
            }
        }
    }
    return action;
}

void IA_Player::simuler(shared_ptr<Noeud> &noeudFils) {
    noeudFils->nbSimulations++;
    noeudCourant->nbSimulations++;
    noeudCourant = noeudFils;
    int compteur = 1;
    bool gameOver = false;
    char joueur = _player == 'X' ? 'O' : 'X'; // Au tour de l'autre joueur
    while (!gameOver) {
        tuple<int, int> action = coupAleatoire(noeudCourant->_board);
        gameOver = playSimulation(action, joueur);
        if (!gameOver) // Si l'action n'a pas résulté en une fin de partie
            joueur = joueur == 'X' ? 'O' : 'X'; // Changer de joueur
        compteur++;
    }
    cerr << "Joueur " << joueur << " a gagné la simulation !\n";
    if (joueur == _player) // Si l'IA a gagné à la fin de la simulation
        noeudCourant->nbVictoires++;
    revenir(noeudCourant, compteur); // Revenir au noeud
}

bool
IA_Player::playSimulation(const tuple<int, int> &action, const char &joueur) {
    // Mettre à jour le nœud courant
    mettreNoeudAJour(action, true, joueur);

    // Vérifier si le joueur actuel a gagné
    std::set<std::pair<int, int>> visited = {{get<0>(action), get<1>(action)}};
    std::vector<std::pair<int, int>> toVisit = {{get<0>(action), get<1>(action)}};
    bool side1 = false;
    bool side2 = false;
    while( toVisit.size() ) {
        auto [i, j] = toVisit.back();
        toVisit.pop_back();

        if(joueur == 'X') {
            if(j == 0) {
                side1 = true;
            }
            if(j == (int)_taille-1) {
                side2 = true;
            }
        } else {
            assert(joueur == 'O');
            if(i == 0) {
                side1 = true;
            }
            if(i == (int)_taille-1) {
                side2 = true;
            }
        }

        if(side1 && side2)
            return true;

        for(int dx=-1; dx<=1; dx++) {
            for(int dy=-1; dy<=1; dy++) {
                if(dx == 0 && dy == 0) continue;
                if(dx == 1 && dy == 1) continue;
                if(dx == -1 && dy == -1) continue;
                if(i+dx < 0 || i+dx >= (int)_taille || j+dy < 0 || j+dy >= (int)_taille) continue;

                if(noeudCourant->_board[i+dx][j+dy] == joueur) {
                    if(visited.count({i+dx, j+dy}) == 0) {
                        visited.insert({i+dx, j+dy});
                        toVisit.push_back({i+dx, j+dy});
                    }
                }
            }
        }
    }
    return false;
}

void IA_Player::revenir(shared_ptr<Noeud> &noeudCourant, const int &compteur) {
    for (int i = 0; i < compteur; ++i)
        noeudCourant = noeudCourant->_parent;
}

tuple<int, int> IA_Player::coupValide(const vector<vector<char>> &board) {
    int row, col = -1;
    for (int i = 0; i < board.size(); ++i) {
        for (int j = 0; j < board[i].size(); ++j) {
            if (board[i][j] == '-') {
                row = i;
                col = j;
            }
        }
    }
    return {row, col};
}

void IA_Player::genererPlateauxFils() {
    vector<vector<char>> nouveauPlateau = vector<vector<char>>(noeudCourant->_board);
    vector<vector<char>> plateauTmp = vector<vector<char>>(nouveauPlateau);
    // Calculer le nombre de cases vides, boucler autant de fois que ce
    // nombre, stocker les coups aléatoires dans plateauTmp
    int nombreCasesVides = calculerNbCasesVides(plateauTmp);
    for (int i = 0; i < nombreCasesVides; ++i) {
        tuple<int, int> coup = coupValide(plateauTmp);
        nouveauPlateau[get<0>(coup)][get<1>(coup)] = _player;
        plateauTmp[get<0>(coup)][get<1>(coup)] = _player;
        ajouterNoeudEnfant(nouveauPlateau, noeudCourant);
        nouveauPlateau = vector<vector<char>>(noeudCourant->_board);
    }
}

int IA_Player::calculerNbCasesVides(const vector<vector<char>> &board) {
    int compteur = 0;
    for (auto &rangee : board)
        for (char _case : rangee)
            if (_case == '-')
                ++compteur;
    return compteur;
}

tuple<int, int> IA_Player::choisirAction() {
    // Générer tous les plateaux possibles depuis le nœud courant
    genererPlateauxFils(); // Tous les nœuds fils possibles ont été ajoutés au nœud courant
    // Simuler depuis le premier fils non-exploré trouvé → (nbSimulations == 0)
    for (auto &noeud : noeudCourant->enfants)
        if (noeud->nbSimulations == 0.0)  // Nœud non-exploré
            simuler(noeud);
    // Simulation finie, le nœud courant est redevenu celui qu'il était originalement
    // Choisir le meilleur nœud enfant, maintenant qu'ils sont tous explorés
    tuple<int, int> meilleure = meilleureAction(noeudCourant->enfants);
    return meilleure;
}

tuple<int, int>
IA_Player::determinerAction(const vector<vector<char>> &original,
                            const vector<vector<char>> &resultat) {
    tuple<int, int> coup;
    for (int i = 0; i < original.size(); ++i)
        for (int j = 0; j < original[i].size(); ++j)
            if (original[i][j] != resultat[i][j])
                coup = {i, j};
    return coup;
}

void IA_Player::afficherPlateau(const vector<vector<char>> &board) {
    cerr << "Plateau : \n";
    for (auto &rangee : board) {
        for (char _case : rangee)
            cerr << _case;
        cerr << '\n';
    }

}


#endif

