#include <iostream>
#include <vector>
#include <chrono>
#include "thc.h"
#include "DifferentialEvolution.h"
#include <random>
#include <math.h>
#include <thread>

using namespace std;
using namespace thc;
using namespace std::chrono; 
// using namespace de;

class ChessCost : public de::IOptimizable {
  public:
    ChessCost() {
        n_players = 1000;
        n_params = 65;
        n_threads = std::thread::hardware_concurrency();
        std::cout << n_threads << " concurrent threads are supported.\n";
        // Each thread will write cost value to thread_costs under its own index
        // This avoids problems like two threads writing to the same memory address at the same time
        for (unsigned int i = 0; i < n_threads; ++i) {
            thread_costs.push_back(0);
        }
        n_players_per_thread = n_players / n_threads;

        uniform_real_distribution<double> dist(-1, 1);
        default_random_engine generator;
        for (unsigned int i = 0; i < n_threads; ++i) {
            vector<vector<double>> players_per_thread;
            for (unsigned int j = 0; j < n_players_per_thread; ++j) {
                vector<double> player;
                for (unsigned int k = 0; k < n_params; ++k) {
                    player.push_back(dist(generator));
                }
                players_per_thread.push_back(player);
            }
            players.push_back(players_per_thread);
        }
    }
    

    unsigned int n_players;
    unsigned int n_params;
    vector<vector<vector<double>>> players;
    

    unsigned int n_threads;
    unsigned int n_players_per_thread;
    mutable vector<double> thread_costs;


    int checkOutcome(ChessRules &cr) const{
        // Checks if white is checkmated or stalemated
        if (cr.WhiteToPlay()) {
            if (cr.AttackedPiece(cr.wking_square)) {
                return -1;
            } else {
                return 0;
            }
        // Checks if black is checkmated or stalemated
        } else {
            if (cr.AttackedPiece(cr.bking_square)) {
                return 1;
            } else {
                return 0;
            }
        }
    }


    double calculateWeight(vector<double> board_vector, vector<double> &params) const{
        double dot = 0;
        for (int i = 0; i < 64; ++i) {
            dot += board_vector[i] * params[i];
        }
        dot += params[64];
        if (dot < 0) {
            dot = 0;
        }
        return dot;
    }

    Move findBestMove(vector<double> &params, ChessRules &cr, vector<Move> &moves) const{
        Move best_move;
        double best_weight;
        double weight;
        bool is_checking_first_move = true;
        for (auto move : moves) {
            // Tries a legal move
            cr.PushMove(move);
            // Calculates a weight for this move
            weight = calculateWeight(cr.ToVector(), params);
            if (is_checking_first_move) {
                best_move   = move;
                best_weight = weight;
                is_checking_first_move = false;
            }
            // If this weight is bigger than last best weight, this move is better and it is saved as the new best move
            if (weight > best_weight) {
                best_move   = move;
                best_weight = weight;
            }
            // Undos the tried move on the board to check the next move
            cr.PopMove(move);
        }
        return best_move;
    }

    int play(vector<double> &params1, vector<double> &params2) const{
        ChessRules cr;
        vector<Move> moves;
        Move move;
        while (true) {
            // Checks if game is over. draw -> 0, white wins -> 1, black wins -> -1
            if (cr.IsJustDraw()) {
                return 0;
            }
            cr.GenLegalMoveList(moves);
            if (moves.size() == 0) {
                return checkOutcome(cr);
            }
            // Finds best move
            if (cr.WhiteToPlay()) {
                move = findBestMove(params1, cr, moves);
                
            } else {
                move = findBestMove(params2, cr, moves);
            }
            // Plays best move
            cr.PlayMove(move);
            // Clears moves vector for reasons of the GenLegalMoveList function implementation
            moves.clear();
        }
        
    }

    // Cost function that penalizes the ai if it loses or ties the games. Also it is a virtual function that must be implemented obviously
    double EvaluteCost(vector<double> inputs) const {
        // -&- in the capture list means lambda function can acces local variables, -this- means lambda function can acces class functions and variables
        vector<thread> threads;
        auto threadCost = [&, this](unsigned int thread_idx) {
            double losses_or_ties = 0;
            int outcome;
            for (auto params2 : players[thread_idx]) {
                outcome = play(inputs, params2);
                if (outcome == -1) {
                    losses_or_ties += 1000.0;
                } else if (outcome == 0) {
                    losses_or_ties += 1.0;
                }
            }
            thread_costs[thread_idx] = losses_or_ties;
        };
        for (unsigned int i = 0; i < n_threads; ++i) {
            threads.push_back(thread(threadCost, i));
        }

        for (auto &t : threads) {
            t.join();
        }

        double losses_or_ties = 0;
        for (auto cost : thread_costs) {
            losses_or_ties += cost;
        }

        return losses_or_ties;
    }

    // It is a virtual function that must be implemented
    unsigned int NumberOfParameters() const {
        return n_params;
    }

    // It is a virtual function that must be implemented. In our case optimization is not using those bounds
    vector<Constraints> GetConstraints() const {
        vector<Constraints> constraints(NumberOfParameters());
        for (auto &constraint : constraints) {
            constraint = Constraints(-1.0, 1.0, true);
        }
        return constraints;
    }
};


int main() {
    ChessCost cc;

    de::DifferentialEvolution de(cc, 65, std::time(nullptr));

    de.Optimize(400, true);

    
    return 0;
}