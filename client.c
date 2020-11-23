
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <stdlib.h>

void play(int server_fd) {

    // Buffer to store incoming messages
    char incoming_message[255];

    // Initialize message strings
    char play_message[255] = "You can now play";
    char win_message[255] = "Game over: you won the game";
    char loss_message[255] = "Game over: you lost the game";    
    
    int dummy_int = 0;
    int score = 0; // Total score so far
    int new_score; // To store score obtained in each dice roll
    int new_score_nl; // To store score converted network byte order

    time_t t;
    srand((unsigned) time(&t));

    // Start reading messages from server
    while(1) {
        // Read a message from server
        read(server_fd, incoming_message, 255);

        if( strcmp(&incoming_message, &play_message) == 0 ) {
            // If the refree instructs to play

            // Generate random score from 1 to 10
            new_score = (int)rand() % 10 + 1;
            score += new_score;

            printf("\nRolling dice...\n");
            printf("Score obtained: %d -- ", new_score);
            printf("Total score : %d\n", score);

            // Convert score integer from host byte to network byte order
            new_score_nl = htonl(new_score);

            // Send score to the server
            write(server_fd, &new_score_nl, sizeof(new_score_nl));

        } else if (strcmp(&incoming_message, &win_message) == 0){
            
            // The game has been won
            printf("I won the game,\n");
            return;
        
        } else if (strcmp(&incoming_message, &loss_message) == 0) {
            
            // The game is lost
            printf("I lost the game,\n");
            return;

        } else {

            // Some other message from server
            printf("The server says : %s", incoming_message);
        }
    }
}

int main(int argc, char * argv[]) {

    printf("Client running\n");

    int server_fd, port_number;
    socklen_t len;
    struct sockaddr_in addr;

    // Check for valid arguements
    if(argc != 3) {
        printf("Usage: %s <server IP> <server port number>\n", argv[0]);
        exit(1);
    }

    if((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Failed to open socket\n");
        exit(1);
    } else {
        printf("Socket initialized\n");
    }

    // Set port number, socket type and address

    sscanf(argv[2], "%d", &port_number);
    addr.sin_family = AF_INET;
    addr.sin_port = htons((uint16_t)port_number);
    
    // Set address
    if(inet_pton(AF_INET, argv[1], &addr.sin_addr) < 0) {
        printf("Failed to execute inet_pton\n");
        exit(2);
    }

    printf("Address object initialized\n");

    // Connect to the server
    if(connect(server_fd, (struct sockaddr *) &addr, sizeof(addr)) < 0) {
        printf("Connection to server failed\n");
        exit(3);
    } else {
        printf("Connected to server\n");
    }

    // Call the function to play the game
    play(server_fd);
    printf("EOG: End of game\n");
}
