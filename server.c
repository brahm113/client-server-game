
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>

int check_verdict(int player_1, int player_2, int score) {
    /**
     * Check if the game is over
     * If game is over return 1, send result messages and close both sockets
     * Return 0 if the game is not over
     */
    
    if(score >= 100) { // Game over

        // Declare winner and looser
        write(player_1, "Game over: you won the game");
        write(player_2, "Game over: you lost the game");
        
        // CLose socket connections
        close(player_1);
        close(player_2);

        // Return 1 to notift that the game is over
        return 1;
    }

    sleep(1); // Wait a second 
    return 0; // Return 0 to notify that the game is on
} 

void servicePlayers(int player_1, int player_2) {

    /**
     * Manage the game of 2 players as refree
     * Instruct player 1 to play
     * Get score from player 1
     * Instruct player 2 to play
     * Get score from player 2
     * If any player wins, declare the verdict to both players
     */

    int p1_score = 0;
    int p2_score = 0;

    int p1_new_score = 0;
    int p2_new_score = 0;

    while(1) {

        // Let player 1 TOTO play;
        write(player_1, "You can now play", 255);
        
        // Read score from TOTO
        
        read(player_1, &p1_new_score, sizeof(p1_new_score));
        p1_new_score = ntohl(p1_new_score);
        p1_score += p1_new_score;

        // CHeck if player 1 won
        if(check_verdict(player_1, player_2, p1_score)) { 
            break;
        }

        // Let player 2 TITI play;
        write(player_2, "You can now play", 255);
        read(player_2, &p2_new_score, sizeof(p2_new_score));
        p2_new_score = ntohl(p2_new_score);
        p2_score += p2_new_score;
        
        // Check if player 2 won
        if(check_verdict(player_2, player_1, p2_score)) { 
            break;
        }

    }

    exit(0);
}

int main(int argc, char * argv[]) {

    // Check for valid arguements
    if(argc != 2) {
        printf("Usage: %s <server port number>\n", argv[0]);
        exit(1);
    }

    // Create a socket
    int sd;
    if((sd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Could not start server\n");
        exit(1);
    }

    // Create an address structure
    socklen_t len;
    int port_number;
    struct sockaddr_in addr;
    sscanf(argv[1], "%d", &port_number);

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons((uint16_t) port_number);

    // Bind the socket to the address
    bind(sd, (struct sockaddr *) &addr, sizeof(addr));

    // Start listening on the socket
    listen(sd, 20); // Allow 20 connections in queue

    while(1) {
        printf("========== Initiating new game ==========\n\n");
        int player_1, player_2;
        
        printf("Waiting for player 1...\n");
        player_1 = accept(sd, (struct sockaddr *) NULL, NULL);
        printf("Player 1 joined\n");
        write(player_1, "Your name is TOTO\n", 255);
        
        printf("Waiting for player 2...\n");
        player_2 = accept(sd, (struct sockaddr *) NULL, NULL);
        printf("Player 2 joined\n");
        write(player_2, "Your name is TITI\n", 255);

        printf("Allocating new room (server process) for the game\n\n");
        
        int pid;

        // Fork the process
        pid = fork();

        if(pid == 0) { // Child process -> refree
            // Make the child process act as a refree for the 2 players
            servicePlayers(player_1, player_2);
        }

        printf("Game started\n\n");
    }
}
