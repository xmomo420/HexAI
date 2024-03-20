#ifndef ExternalProgram_PLAYER__H
#define ExternalProgram_PLAYER__H

#include <iostream>
#include <cstdio>
#include <csignal>
#include <chrono>
#include <thread>

#include "Hex_Environement.h"


class ExternalProgram : public Player_Interface {
    char player;
    int pipefd_child2parent[2] = {0,0};
    int pipefd_parent2child[2] = {0,0};

public:
    ExternalProgram(std::string path, char player) : player(player) {
        assert(player == 'X' || player == 'O');
        
        // Create a pipe
        pipe(pipefd_child2parent);

        // Create a pipe
        pipe(pipefd_parent2child);

        // Fork the process
        pid_t pid = fork();

        if (pid == 0)
        {
            // Child process
            close(pipefd_child2parent[1]);
            close(pipefd_parent2child[0]);

            // Redirect cout to the write end of the pipe
            dup2(pipefd_parent2child[1], STDOUT_FILENO);
            dup2(pipefd_child2parent[0], STDIN_FILENO);

            std::string s = "";
            s += path;
            s += " ";
            s += player;
            system(s.c_str());

            sleep(1);

            // Close the pipes
            close(pipefd_child2parent[0]);
            close(pipefd_parent2child[1]);
            exit(0);
        } else {
            sleep(1);

            close(pipefd_child2parent[0]);
            close(pipefd_parent2child[1]);
        }
    }

    ~ExternalProgram() {
        // Close the pipes
        close(pipefd_child2parent[1]);
        close(pipefd_parent2child[0]);
    }

    void otherPlayerMove(int row, int col) override {
        char buffer[3];
        buffer[0] = row + '0';
        buffer[1] = col + '0';
        buffer[2] = '\n';
        write(pipefd_child2parent[1], buffer, 3);
    }

    std::tuple<int, int> getMove(Hex_Environement& hex) override {
        char buffer[3];
        read(pipefd_parent2child[0], buffer, 3);
        int row = buffer[0]-'0';
        int col = buffer[1]-'0';
        return {row, col};
    }
};



#endif

