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
#include <chrono>

#include "Hex_Environement.h"

using namespace std;

class IA_Player : public Player_Interface {

private:
    char _player;
    char _otherPlayer;
    unsigned int _taille;
    vector<tuple<int, int>> historiqueCoups = vector<tuple<int, int>>();

    // Nœud dans l'arbre
    struct Noeud {
        // Constructeurs
        Noeud(const vector<vector<char>> &board, const shared_ptr<Noeud> &parent, const bool &estRacine);
        Noeud();
        // Attributs
        bool etanduSansEnfants = false; // Utilse si aucun enfants est pertinent
        vector<vector<char>> _board = vector<vector<char>>();
        double nbSimulations = 0.0;
        double nbVictoires = 0.0;
        double nbSimulationsEnfants = 0.0;
        double nbVictoiresEnfants = 0.0;
        shared_ptr<Noeud> _parent;
        vector<shared_ptr<Noeud>> enfants = vector<shared_ptr<Noeud>>();
        // Indique quel joueur a joué le coup qui a mené à ce nœud
        // Joueur 'O' si c'est la racine, car c'est au tour du joueur 'X'
        char coupJoue = 'O';
        bool estGagnant = false;
    };

    // Indique le nœud correspondant à l'état du jeu actuel
    shared_ptr<Noeud> noeudCourant;
    // Constante 'c' pour le calcul de l'UCT
    constexpr static const double CONSTANTE_C = 0.5;

public:
    explicit IA_Player(char player, unsigned int taille=10) : _player(player), _taille(taille) {
        assert(player == 'X' || player == 'O');
        _otherPlayer = _player == 'X' ? 'O' : 'X';
        shared_ptr<Noeud> parent = make_shared<Noeud>();
        vector<vector<char>> emptyBoard;
        emptyBoard.resize(taille, std::vector<char>(taille, '-'));
        noeudCourant = make_shared<Noeud>(emptyBoard, parent, true);
    }

    void otherPlayerMove(int row, int col) override {
        mettreNoeudAJour({row, col}, _otherPlayer);
    }

    tuple<int, int> getMove(Hex_Environement& hex) override {
        tuple<int, int> coup = bestMove();
        mettreNoeudAJour(coup, _player);
        return coup;
    }

/**************************************************************************
 ************************ Déclarations de fonctions ***********************
 **************************************************************************/

/**
 *
 * @return
 */
tuple<int, int> bestMove();

/**
 *
 * @param noeud, le noeud à partir d'où l'algorithme fait la séléction.
 * Il représente initialement l'état actuel du jeu.
 * @return
 */
static shared_ptr<IA_Player::Noeud> &selection(shared_ptr<Noeud> &noeud,
                                               const vector<tuple<int, int>> &historiqueCoups);

/**
 *
 * @param enfants
 * @return
 */
static shared_ptr<Noeud> & meilleurNoeud(vector<shared_ptr<Noeud>> &enfants);

/**
 *
 * @param noeud
 * @return
 */
static shared_ptr<Noeud> & expansion(shared_ptr<Noeud> &noeud);

/**
 *
 * @param noeud
 * @return
 */
static char simulation(shared_ptr<Noeud> &noeud, vector<vector<char>> &copiePlateau);

/**
 *
 * @param noeud
 */
static void retropropagation(shared_ptr<Noeud> &noeud, const char &joueurGagnant,
                             const vector<vector<char>> &plateauTerminal);

/**
 *
 * @param original
 * @param resultat
 * @return
 */
static tuple<int, int, char> determinerActionJoueur(const vector<vector<char>> &original,
                                                    const vector<vector<char>> &resultat);

/**
 * Affiche le plateau passé en paramètre
 */
static void afficherPlateau(const vector<vector<char>> &board);

/**
 *
 * @param noeudParent
 */
static bool genererEnfants(shared_ptr<Noeud> &noeudParent);

/**
 *
 * @param action
 * @param board
 * @return
 */
static bool partieFinie(const tuple<int, int, char> &action,
                        const vector<vector<char>> &board);

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
 * @param board
 * @return
 */
static int calculerNbCasesVides(const vector<vector<char>> &board);

/**
 *
 * @return
 */
static double calculerScore(const shared_ptr<Noeud> &noeud, const bool &exploration);

/**
 *
 * @param enfants
 * @return
 */
static tuple<int, int> meilleureAction(vector<shared_ptr<Noeud>> &enfants);

/**
 *
 * @param board
 * @return
 */
static tuple<int, int> coupAleatoire(const vector<vector<char>> &board);

/**
 *
 * @param action
 * @param simulation
 * @param joueur
 */
void mettreNoeudAJour(const tuple<int, int> &action, const char &joueur);

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
 * @param board
 * @return
 */
static tuple<int, int> coupValide(const vector<vector<char>> &board);

static bool jouer(vector<vector<char>> &board, const tuple<int, int> &coup,
                  const char &joueur);

static void verifierSiNoeudGagnant(shared_ptr<Noeud> &noeud);


static void ajouterDonnesSimulation(vector<shared_ptr<Noeud>> &enfants,
                                    const vector<vector<char>> &plateauFinal,
                                    const char &joueurGagnant);

static bool estUnEnfant(const shared_ptr<Noeud> &enfant, const vector<vector<char>> &plateauFinal);

static bool fillinPruning(const tuple<int, int> &coup,
                          const vector<vector<char>> &board,
                          const char &joueur);

    static vector<tuple<int, int>>
    genererCoupsGagnants(const char &joueur, const vector<vector<char>> &board);

    static bool contient(const tuple<int, int> &coup,
                         const vector<tuple<int, int>> &coupsGagnants);
};


/***************************************************************************
 *********************** Définition de fonctions ***************************
 ***************************************************************************/

IA_Player::Noeud::Noeud(const vector<vector<char>> &board, const shared_ptr<Noeud> &parent,
                        const bool &estRacine) : _board(board) {
    if (estRacine) {
        shared_ptr<Noeud> ptrParent = nullptr;
        _parent = ptrParent;
    } else {
        _parent = parent;
        coupJoue = parent->coupJoue == 'X' ? 'O' : 'X';
    }

}

IA_Player::Noeud::Noeud() = default;

tuple<int, int> IA_Player::coupAleatoire(const vector<vector<char>> &board) {
    int indiceCoup;
    vector<tuple<int, int>> coupsPossibles = vector<tuple<int, int>>();
    for (int i = 0; i < board.size(); ++i)
        for (int j = 0; j < board[i].size(); ++j)
            if (board[i][j] == '-') // Case valide
                coupsPossibles.emplace_back(i, j);
    assert(!coupsPossibles.empty());
    indiceCoup = rand()%coupsPossibles.size();
    return coupsPossibles[indiceCoup];
}

void
IA_Player::mettreNoeudAJour(const tuple<int, int> &action, const char &joueur) {
    // Ajouter le coup dans l'historique
    auto [x, y] = action;
    historiqueCoups.emplace_back(x, y);
    // Mettre à jour le plateau de jeu
    vector<vector<char>> plateauEnfant = noeudCourant->_board;
    plateauEnfant[get<0>(action)][get<1>(action)] = joueur;
    // Ajoute le nœud enfant, dans le vecteur d'enfant du nœud courant, s'il n'est pas déja dedans.
    shared_ptr<Noeud> nouveauNoeud = ajouterNoeudEnfant(plateauEnfant, noeudCourant);
    // MAJ le nœud courant
    noeudCourant = nouveauNoeud;
    // Modification
    noeudCourant->_parent = nullptr;
}

shared_ptr<IA_Player::Noeud> & IA_Player::ajouterNoeudEnfant(
        const vector<vector<char>> &plateauEnfant, shared_ptr<Noeud> &noeudParent) {
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

double IA_Player::calculerScore(const shared_ptr<Noeud> &noeud, const bool &exploration) {
    // Modification : Nœuds non-explorés sont prioritaires
    if (noeud->nbSimulations == 0)
        return exploration ? MAXFLOAT : 0;
    else {
        assert(noeud->_parent->nbSimulations != 0);
        assert(noeud->nbSimulationsEnfants != 0);
        double amaf = noeud->nbVictoiresEnfants / noeud->nbSimulationsEnfants;
        double omega = 1.0 / (1.0 + 0.1 * noeud->nbSimulations);
        double uct = (noeud->nbVictoires / noeud->nbSimulations) +
                     CONSTANTE_C * sqrt(log(noeud->_parent->nbSimulations) /
                                    noeud->nbSimulations);
        return (1 - omega) * uct + omega * amaf;
    }
}

tuple<int, int> IA_Player::meilleureAction(vector<shared_ptr<Noeud>> &enfants) {
    assert(!enfants.empty());
    double meilleurScore = -1.0;
    double scoreEnfant;
    tuple<int, int> action = {-1, -1};
    for (auto &noeud : enfants) {
        auto [i, j] = determinerAction(noeud->_parent->_board, noeud->_board);
        if (partieFinie({i, j, noeud->coupJoue}, noeud->_board))
            // Retourner une action gagnante si c'est possible
            return {i, j};
        scoreEnfant = calculerScore(noeud, false);
        if (scoreEnfant >= meilleurScore)
            // Rajouter une chance de 50% pour prendre ou non le nœud dont
            // le UCT est égal à l'autre
            if (scoreEnfant > meilleurScore || rand() % 2 == 0) {
                meilleurScore = scoreEnfant;
                action = determinerAction(noeud->_parent->_board, noeud->_board);
            }
    }
    // TODO : S'assurer que l'IA retourne une action qui empêche l'adversaire de gagner
    auto [i, j] = action;
    assert(i != -1 && j != -1);
    return action;
}

tuple<int, int> IA_Player::coupValide(const vector<vector<char>> &board) {
    int row, col = -1;
    for (int i = 0; i < board.size(); ++i)
        for (int j = 0; j < board[i].size(); ++j)
            if (board[i][j] == '-') {
                row = i;
                col = j;
            }
    return {row, col};
}

int IA_Player::calculerNbCasesVides(const vector<vector<char>> &board) {
    int compteur = 0;
    for (auto &rangee : board)
        for (char _case : rangee)
            if (_case == '-')
                ++compteur;
    return compteur;
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
    cerr  << endl;
    for (auto &rangee : board) {
        for (char _case : rangee)
            cerr << _case;
        cerr << '\n';
    }

}

shared_ptr<IA_Player::Noeud> &IA_Player::selection(shared_ptr<Noeud> &noeud,
                                                   const vector<tuple<int, int>> &historiqueCoups) {
    tuple<int, int, char> action; // Action qui mène à l'enfant nœud
    while (!noeud->enfants.empty()) { // S'arrêter quand on tombe sur une feuille
        noeud = meilleurNoeud(noeud->enfants);
        if (noeud->nbSimulations == 0) {
            verifierSiNoeudGagnant(noeud);
            return noeud; // Explorer tous les enfants avant
        }
    }
    bool premierParcours = false;
    // Déterminer l'action qui a été jouée et quel joueur l'a jouée.
    if (noeud->_parent)
        action = determinerActionJoueur(noeud->_parent->_board, noeud->_board);
    else {
        if (!historiqueCoups.empty()) { // Premier parcours
            auto [x, y] = historiqueCoups.back();
            action = {x, y, noeud->coupJoue};
        } else // Premier parcours
            premierParcours = true;
    }
    // Si c'est un noeud terminal
    if (!premierParcours && partieFinie(action, noeud->_board)) {
        noeud->estGagnant = true;
        return noeud;
    }
    // TODO : Si le noeud a été exploré, mais qu'il n'a aucun enfants (aucun coup utile)
    //  Solution 1 : retourner le noeud
    //  Solution 2 : Remonter dans l'arbre pour trouver un autre noeud
    // Génère tous les enfants possibles du nœud
    return expansion(noeud); // Retourne aléatoirement un de ces nœuds enfants
}

shared_ptr<IA_Player::Noeud> & IA_Player::meilleurNoeud(vector<shared_ptr<Noeud>> &enfants) {
    double meilleurScore = -1.0;
    double scoreEnfant;
    int indiceMeilleur;
    for (int i = 0; i < enfants.size(); ++i) {
        scoreEnfant = calculerScore(enfants[i], true);
        if (scoreEnfant >= meilleurScore)
            // Ajouter une chance de 50 %
            if (scoreEnfant > meilleurScore || rand() % 2 == 0) {
                indiceMeilleur = i;
                meilleurScore = scoreEnfant;
            }
    }
    assert(indiceMeilleur < enfants.size());
    return enfants[indiceMeilleur];
}

tuple<int, int, char>
IA_Player::determinerActionJoueur(const vector<vector<char>> &original,
                                  const vector<vector<char>> &resultat) {
    tuple<int, int, char> coup;
    for (int i = 0; i < original.size(); ++i)
        for (int j = 0; j < original[i].size(); ++j)
            if (original[i][j] != resultat[i][j])
                coup = {i, j, resultat[i][j]};
    return coup;
}

bool IA_Player::partieFinie(const tuple<int, int, char> &action,
                            const vector<vector<char>> &board) {
    auto [x, y, joueur] = action;
    // Vérifier si le joueur actuel a gagné
    std::set<std::pair<int, int>> visited = {{x, y}};
    std::vector<std::pair<int, int>> toVisit = {{x, y}};
    bool side1 = false;
    bool side2 = false;
    while( toVisit.size() ) {
        auto [i, j] = toVisit.back();
        toVisit.pop_back();

        if(joueur == 'X') {
            if(j == 0)
                side1 = true;
            if(j == (int)board.size()-1)
                side2 = true;
        } else {
            assert(joueur == 'O');
            if(i == 0)
                side1 = true;
            if(i == (int)board.size()-1)
                side2 = true;
        }

        if(side1 && side2)
            return true;

        for(int dx=-1; dx<=1; dx++)
            for(int dy=-1; dy<=1; dy++) {
                if(dx == 0 && dy == 0) continue;
                if(dx == 1 && dy == 1) continue;
                if(dx == -1 && dy == -1) continue;
                if(i+dx < 0 || i+dx >= (int)board.size() || j+dy < 0 || j+dy >= (int)board.size()) continue;

                if(board[i+dx][j+dy] == joueur)
                    if(visited.count({i+dx, j+dy}) == 0) {
                        visited.insert({i+dx, j+dy});
                        toVisit.emplace_back(i+dx, j+dy);
                    }
            }
    }
    return false;
}

shared_ptr<IA_Player::Noeud> & IA_Player::expansion(shared_ptr<Noeud> &noeud) {
    assert(noeud->enfants.empty());
    // Générer tous les nœuds enfants du nœud en paramètre
    if (genererEnfants(noeud))
        return noeud; // Retourner le noeud lui même s'aucun enfants n'a été créé
    size_t indice = rand()% noeud->enfants.size();
    assert(indice < noeud->enfants.size());
    verifierSiNoeudGagnant(noeud->enfants[indice]);
    return noeud->enfants[indice];
}

bool IA_Player::genererEnfants(shared_ptr<Noeud> &noeudParent) {
    assert(noeudParent->enfants.empty());
    char joueur = noeudParent->coupJoue == 'X' ? 'O' : 'X';
    vector<vector<char>> nouveauPlateau = vector<vector<char>>(noeudParent->_board);
    vector<vector<char>> plateauTmp = vector<vector<char>>(nouveauPlateau);
    tuple<int, int> coup;
    bool coupInutile;
    bool plusDePlace = false;
    bool coupGagnant; // Indique si le coup est gagnant
    // Ne pas inclure les coups qui ne sont pas gagnant lorsque le joueur peut gagner
    bool autreCoupsObsoletes = false;
    while (!plusDePlace) {
        coup = coupValide(plateauTmp);
        auto [i, j] = coup;
        plusDePlace = i == -1 || j == -1;
        if (!plusDePlace) {
            coupGagnant = partieFinie({i, j, joueur}, nouveauPlateau);
            if (coupGagnant && !autreCoupsObsoletes) { // S'il n'y avait pas de coup gagnant
                autreCoupsObsoletes = true;
                noeudParent->enfants.clear();
            }
            if (autreCoupsObsoletes)
                coupInutile = !coupGagnant;
            else
                coupInutile = fillinPruning(coup, nouveauPlateau, joueur);
            nouveauPlateau[i][j] = joueur;
            plateauTmp[i][j] = joueur;
            if (!coupInutile)
                noeudParent->enfants.push_back(
                        make_shared<Noeud>(nouveauPlateau, noeudParent,
                                           false));
            nouveauPlateau = vector<vector<char>>(noeudParent->_board);
        }
    }
    return (noeudParent->enfants.empty()); // Indique si aucun enfants a été créé
}

char IA_Player::simulation(shared_ptr<Noeud> &noeud, vector<vector<char>> &copiePlateau) {
    tuple<int, int, char> action;
    bool gameOver = noeud->estGagnant;
    tuple<int, int> coupChoisi;
    char joueur = noeud->coupJoue;
    while (!gameOver) {
        joueur = noeud->coupJoue == 'X' ? 'O' : 'X'; // Changer de joueur
        coupChoisi = coupAleatoire(copiePlateau);
        gameOver = jouer(copiePlateau, coupChoisi, joueur);
    }
    // Retourner le joueur gagnant, le joueur qui a joué en dernier
    return joueur;
}

bool IA_Player::jouer(vector<vector<char>> &board,
                      const tuple<int, int> &coup, const char &joueur) {
    auto [x, y] = coup;
    board[x][y] = joueur; // Placer le pion de l'adversaire sur le plateau

    // Vérifier si le joueur actuel a gagné
    std::set<std::pair<int, int>> visited = {{x, y}};
    std::vector<std::pair<int, int>> toVisit = {{x, y}};
    bool side1 = false;
    bool side2 = false;
    while( toVisit.size() ) {
        auto [i, j] = toVisit.back();
        toVisit.pop_back();

        if(joueur == 'X') {
            if(j == 0)
                side1 = true;
            if(j == (int)board.size()-1)
                side2 = true;
        } else {
            assert(joueur == 'O');
            if(i == 0)
                side1 = true;
            if(i == (int)board.size()-1)
                side2 = true;
        }

        if(side1 && side2)
            return true;

        for(int dx=-1; dx<=1; dx++)
            for(int dy=-1; dy<=1; dy++) {
                if(dx == 0 && dy == 0) continue;
                if(dx == 1 && dy == 1) continue;
                if(dx == -1 && dy == -1) continue;
                if(i+dx < 0 || i+dx >= (int)board.size() || j+dy < 0 || j+dy >= (int)board.size()) continue;

                if(board[i+dx][j+dy] == joueur)
                    if(visited.count({i+dx, j+dy}) == 0) {
                        visited.insert({i+dx, j+dy});
                        toVisit.emplace_back(i+dx, j+dy);
                    }
            }
    }
    return false;
}

void
IA_Player::retropropagation(shared_ptr<Noeud> &noeud, const char &joueurGagnant,
                            const vector<vector<char>> &plateauTerminal) {
    // Retourner au nœud qui représente l'état actuel du jeu
    while (noeud->_parent) {
        noeud->nbSimulations++;
        if (noeud->coupJoue == joueurGagnant)
            noeud->nbVictoires++;
        noeud = noeud->_parent;
        ajouterDonnesSimulation(noeud->enfants, plateauTerminal, joueurGagnant);
    }
    // Mettre à jour le nœud qui représente l'état actuel
    noeud->nbSimulations++;
    if (noeud->coupJoue == joueurGagnant)
        noeud->nbVictoires++;
    ajouterDonnesSimulation(noeud->enfants, plateauTerminal, joueurGagnant);
}

tuple<int, int> IA_Player::bestMove() {
    double nombreMaxCoup = calculerNbCasesVides(noeudCourant->_board);
    double timer = 0.0;
    double timerLimit = 100.0 / nombreMaxCoup + 5.0;
    char joueurGagnantSimulation;
    vector<vector<char>> copiePlateau;
    // Définition du point de départ
    auto start = chrono::high_resolution_clock::now();
    while (timer < timerLimit) {
        // Mesurer le temps écoulé à chaque itération
        auto end = chrono::high_resolution_clock::now();
        chrono::duration<double> elapsed = end - start;
        timer = elapsed.count();
        noeudCourant = selection(noeudCourant, historiqueCoups);
        // Le plateau sera modifié en un état terminal après la simulation
        copiePlateau = vector<vector<char>>(noeudCourant->_board);
        joueurGagnantSimulation = simulation(noeudCourant, copiePlateau);
        retropropagation(noeudCourant, joueurGagnantSimulation,
                         copiePlateau);
    }
    return meilleureAction(noeudCourant->enfants);
}

void IA_Player::verifierSiNoeudGagnant(shared_ptr<Noeud> &noeud) {
    tuple<int, int, char> action = determinerActionJoueur(noeud->_parent->_board,
                                                          noeud->_board
                                                          );
    noeud->estGagnant = partieFinie(action, noeud->_board);
}

void IA_Player::ajouterDonnesSimulation(vector<shared_ptr<Noeud>> &enfants,
                                        const vector<vector<char>> &plateauFinal,
                                        const char &joueurGagnant) {
    for (shared_ptr<Noeud> &unEnfant : enfants)
        if (estUnEnfant(unEnfant, plateauFinal)) {
            unEnfant->nbSimulationsEnfants++;
            if (unEnfant->coupJoue == joueurGagnant)
                unEnfant->nbVictoiresEnfants++;
        }
}

bool IA_Player::estUnEnfant(const shared_ptr<Noeud> &enfant, const vector<vector<char>> &plateauFinal) {
    bool estUnEnfant = true;
    for (int i = 0; estUnEnfant && i < plateauFinal.size(); ++i)
        for (int j = 0; estUnEnfant && j < plateauFinal[i].size(); ++j)
            // Les cases non-vides de l'enfant doivent avoir le même contenu
            if (enfant->_board[i][j] != '-')
                estUnEnfant = enfant->_board[i][j] == plateauFinal[i][j];
    return estUnEnfant;
}

bool IA_Player::fillinPruning(const tuple<int, int> &coup, const vector<vector<char>> &board,
                              const char &joueur) {
    auto [i, j] = coup;
    bool estInutile = false;
    char autreJoueur = joueur == 'X' ? 'O' : 'X';
    // TODO : Cas où le coup n'empêche pas l'adversaire de gagner alors qu'il peut
    //  Générer un vector de coup gagnants pour l'adversaire
    vector<tuple<int, int>> coupsGagnants = genererCoupsGagnants(autreJoueur, board);
    if (!coupsGagnants.empty() && !contient(coup, coupsGagnants))
        return true;
    // Implémenter les cas dans Sujet.pdf
    // TODO : Réviser pour confirmer si c'est valide
    // Cas 1
    try {
        estInutile = board.at(i).at(j - 1) == 'O' && board.at(i).at(j + 1) == 'O'
                && board.at(i + 1).at(j) == 'O' && board.at(i + 1).at(j - 1) == 'O';
    } catch (out_of_range &outOfRange) {}
    if (estInutile)
        return true;
    // Cas 2
    try {
        estInutile = board.at(i).at(j - 1) == 'O' && board.at(i).at(j + 1) == 'X'
                     && board.at(i - 1).at(j + 1) == 'X' && board.at(i + 1).at(j - 1) == 'O';
    } catch (out_of_range &outOfRange) {}
    if (estInutile)
        return true;
    // Cas 3
    try {
        estInutile = board.at(i).at(j - 1) == 'O' && board.at(i).at(j + 1) == 'X'
                && board.at(i - 1).at(j + 1) == 'X' && board.at(i + 1).at(j) == 'X';
    } catch (out_of_range &outOfRange) {}
    if (estInutile)
        return true;
    // Cas 4
    try {
        estInutile = (board.at(i).at(j + 1) == '-' && board.at(i - 1).at(j + 1) == 'O'
                && board.at(i + 1).at(j - 1) == 'O' && board.at(i + 1).at(j) == 'O'
                && board.at(i + 1).at(j + 1) == 'O' )
                || (board.at(i - 1).at(j) == 'O' && board.at(i).at(j - 1) == '-'
                && board.at(i + 1).at(j) == 'O' && board.at(i + 1).at(j - 1) == 'O'
                && board.at(i + 1).at(j - 2) == 'O');
    } catch (out_of_range &outOfRange) {}
    if (estInutile)
        return true;
    // Cas 5
    try {
        estInutile = (board.at(i).at(j + 1) == 'O' && board.at(i).at(j - 1) == '-'
                && board.at(i).at(j - 2) == 'O' && board.at(i + 1).at(j) == 'O'
                && board.at(i + 1).at(j - 1) == 'O' && board.at(i + 1).at(j - 2) == 'O')
                || (board.at(i).at(j - 1) == 'O' && board.at(i).at(j + 1) == '-'
                && board.at(i).at(j + 2) == 'O' && board.at(i + 1).at(j) == 'O'
                && board.at(i + 1).at(j - 1) == 'O' && board.at(i + 1).at(j + 1) == 'O');
    } catch (out_of_range &outOfRange) {}
    if (estInutile)
        return true;
    // Cas 6
    try {
        estInutile = (board.at(i - 1).at(j) == 'O' && board.at(i - 1).at(j + 1) == 'O'
                && board.at(i).at(j + 1) == '-' && board.at(i + 1).at(j) == 'O'
                && board.at(i + 1).at(j + 1) == 'O')
                || (board.at(i - 1).at(j) == 'O' && board.at(i - 1).at(j - 1) == 'O'
                && board.at(i).at(j - 1) == '-' && board.at(i + 1).at(j) == 'O'
                && board.at(i + 1).at(j - 1) == 'O');
    } catch (out_of_range &outOfRange) {}
    return estInutile;
}

vector<tuple<int, int>> IA_Player::genererCoupsGagnants(const char &joueur,
                                                        const vector<vector<char>> &board) {
    vector<tuple<int, int>> coupsGagnants = vector<tuple<int, int>>();
    vector<vector<char>> copieBoard = vector<vector<char>>(board);
    for (int i = 0; i < copieBoard.size(); ++i)
        for (int j = 0; j < copieBoard[i].size(); ++j)
            if (copieBoard[i][j] == '-') { // Cases vides
                copieBoard[i][j] = joueur; // Ajouter le pion du joueur
                if (partieFinie({i, j, joueur}, copieBoard))
                    // Ajoute le coup s'il met fin à la partie
                    coupsGagnants.emplace_back(i, j);
                copieBoard = vector<vector<char>>(board);
            }
    return coupsGagnants;
}

bool IA_Player::contient(const tuple<int, int> &coup, const vector<tuple<int, int>> &coupsGagnants) {
    auto [x, y] = coup;
    bool estDedans;
    for (auto &coupGagnant : coupsGagnants) {
        auto [i, j] = coupGagnant;
        estDedans = x == i && y == j;
        if (estDedans)
            break;
    }
    return estDedans;
}

#endif

