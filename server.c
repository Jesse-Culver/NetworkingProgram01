/* Compilation: gcc -o server server.c
   Execution  : ./server 5000
   Tutorial followed at: https://www.tutorialspoint.com/unix_sockets/socket_server_example.htm and then expanded upon
   Threading is probably the better solution for long term but forking is easier
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

void CountTheVowels (int sock);

int main(int argc, char *argv[])
{
		if (argc < 2)
		{
			printf("\nPort number is missing...\n");
			exit(0);
		}
		printf("Server has started\n");
    	int sockfd, newsockfd, portno, clilen;
   		struct sockaddr_in serv_addr, cli_addr;
    	char buffer[256]; 
    	int n, pid;

    	sockfd = socket(AF_INET, SOCK_STREAM, 0);

    	if(sockfd < 0){
    		printf("ERROR opening socket\n");
    		exit(1);
    	}
    	//Initialize socket structure
    	bzero((char *) &serv_addr, sizeof(serv_addr));
    	portno = atoi(argv[1]);

    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = INADDR_ANY;
    	serv_addr.sin_port = htons(portno);

    	//Bind host
    	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
    	{
    		printf("ERROR on binding\n");
    		exit(1);
    	}

    	//starting listening for clients
    	listen(sockfd, 5);
    	clilen = sizeof(cli_addr);

    	//Loop forever
    	while(1)
    	{
    		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    		if(newsockfd < 0)
    		{
    			printf("ERROR on accept\n");
    			exit(1);
    		}

    		//create child process
    		pid = fork();

    		if(pid < 0)
    		{
    			perror("ERROR on fork\n");
    			exit(1);
    		}
    		if(pid == 0)
    		{
    			//Client Process
    			close(sockfd);
    			printf("Client Connected\n");
    			CountTheVowels(newsockfd);
    			exit(0);
    		}
    		else
    		{
    			close(newsockfd);
    		}
    	}
}

void CountTheVowels(int sock)
{
	int quit = 0;
	while(quit == 0)
	{
		int n;
		char buffer[256];
		bzero(buffer, 256);
		n = read(sock, buffer, 255);

		if(n < 0)
		{
			perror("ERROR reading from socket\n");
			exit(1);
		}
		if(strstr(buffer, "Bye") == NULL)
		{
			printf("Sentence recieved: %s\n", buffer);

			int i;
			int vowel_count = 0;
			for(i = 0; i < strlen(buffer); i++)
			{
				if(buffer[i] == 'a' || buffer[i] == 'e'|| buffer[i] == 'i' || buffer[i] == 'o' || buffer[i] == 'u')
					vowel_count++;
			}
			//This counts the number of characters that make up a number
			//Source: https://stackoverflow.com/questions/3068397/finding-the-length-of-an-integer-in-c
			int char_count = floor(log10(abs(vowel_count))) + 1;
			
			//itoa(vowel_count,buffer,10); //Well our compiler is out of date I guess...
			char message[12];
			snprintf(message, 12,"%d",vowel_count);
			n = write(sock, message, char_count);

			if( n < 0)
			{
				perror("ERROR writing to socket\n");
				exit(1);
			}
		}
		else
		{
			quit = 1;
			printf("Connection Closing\n");
		}
	}
}