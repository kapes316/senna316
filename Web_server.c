/*
 * Main_Server.c
 * Author of this file - Kaushik Pidugu.
 * References for sytem calls were taken from www.LinuxHowtos.org
 * Started coding on 08/13/2014.
 * Completed on 08/19/2014.
 */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define SUCCESS  0
#define FAILURE -1
#define BUF_SIZE 512
#define HALF_BUF 256

char msg[100] = {0}; // For message printout in Server side.

/* Error Function */
static void Error_func (char *msg)
{
    printf ("\n Error:");
    printf("%s\n", msg);
    return;
}    

/* SUCCESS Function */
static void Success_func (char *msg)
{
    printf ("\n SUCCESS:");
    printf("%s\n", msg);
    return;
}    


/* Action function in receiving and sending the response to 
 * the Client. 
 * 1. Receive the request.
 * 2. Parse the HTTP header to get the file_path.
 * 3. Search for the file in our Local directory.
 * 4. Sent the HTTP header with status, Content-Type and Server details
 * 5. Finally send the file data.
 */
static int Service_the_client_requestFunc(int Cli_sock) 
{
    int nbytes = 0, status = 0;
    char *buffer = NULL, *response = NULL;
    int i = 0, count = 0;
    char file_path[100] = {0};
    FILE *fd = NULL;
    char *Http_Header = NULL;

    /* Reading initiated!!
     * Allocating memory for Read Buffer
     */
    buffer = (char *)malloc((sizeof(char))*BUF_SIZE);
    if (buffer == NULL) {
        Error_func("Memory Allocation Failed!!");
        return FAILURE;
    } 
    
    memset(buffer, 0, BUF_SIZE);
    if ((nbytes = recv(Cli_sock, buffer, BUF_SIZE, 0)) < 0) {
        Error_func("receiving from client socket failed!");
        free(buffer);
        return FAILURE;
    } else if (nbytes == 0) {
        Error_func("receiving EOF from client socket completed!");
        free(buffer);
        return FAILURE;
    } else {
        Success_func(buffer);
    }
    /* Reading from the client completed */

    /* Now parsing the HTTP header and sending the response 
     * check whether it's HTTP GET request or anyother
     */
    if (!(buffer[0] == 'G' && buffer[1] == 'E' && buffer[2] == 'T')) {
        Error_func("Invalid request..NO response..not yet Implementing in the serverside!!!");
        free(buffer);
        return FAILURE;
    }

    memset(file_path, 0, 100);
    strcpy(file_path, "/home/kaushik/Network_project");

    while(1) {
        if (file_path[i] == '\0') 
            break;
        else
            ++i;
    }

    /* to Get the complete file */
    sscanf(buffer, "GET %s ", &file_path[i]); 
    Success_func(file_path);
    /* what if the client doesn't give any file_path
     * return failure if that is the case.
     */
    if (strcmp(file_path, "/home/kaushik/Network_project/") == 0) {
       Error_func("No HTTP file was given on the client side!!\n");
       free(buffer);
       return FAILURE;
    }
    
    /* Allocate memory for HTTP header */
    Http_Header = (char *)malloc((sizeof(char))*HALF_BUF);
    if (Http_Header == NULL) {
        Error_func("Memory Allocation Failed!!");
        free(buffer);
        return FAILURE;
    } 

    fd = fopen(file_path, "r+");
    if(fd == NULL) {
       /* send response to client with 404 Error */
       status = 404;
       memset(Http_Header, 0, HALF_BUF);
       sprintf(Http_Header, "HTTP/1.1 %d NOT FOUND\r\n", status);
       if (!(nbytes = send(Cli_sock, Http_Header, strlen(Http_Header), 0))) {
          Error_func("Sending failed....hmm...this shouldn't happen");
       } else {
          write(Cli_sock, "Server: OURTEAMWebServ v0.1\r\n", 29);
          write(Cli_sock, "\r\n", 2);
          write(Cli_sock, "FILE NOT FOUND 404 ERROR \r\n",27);  
       }
       Success_func("Request file not found...so sent 404 response to Client.\n");
       free(buffer);
       free(Http_Header);
       return SUCCESS;
    } else {
        status = 200;
        memset(Http_Header, 0, HALF_BUF);
        sprintf(Http_Header, "HTTP/1.1 %d OK\r\n", status);
        if (!(nbytes = send(Cli_sock, Http_Header, strlen(Http_Header), 0))) {
            Error_func("Sending failed....hmm...this shouldn't happen");
        } else {
           write(Cli_sock, "Server: OURTEAMWebServ v0.1\r\n", 29);
           write(Cli_sock, "\r\n", 2);
        }
        Success_func("Hurray!!!!:Sent the HTTP header response to client");
   } 

   /* DONE sending the HTTP header. 
    * Now, Allocate memory for response data packet
    * Send the data to the client.
    */   
   response = (char *)malloc((sizeof(char))*BUF_SIZE);
   if (response == NULL) {
       Error_func("Memory Allocation Failed!!");
       free(buffer);
       free(Http_Header);
       return FAILURE;
   } 
 
   do {
       memset(response, 0, BUF_SIZE);
       int Rbytes = fread(response, 1, BUF_SIZE, fd);
       if (!(nbytes = send(Cli_sock, response, Rbytes, 0))) {
          Error_func("Sending failed....hmm...this shouldn't happen");
       } else {
          if (count == 10) {
              printf("\n Sending......");
              count = 0;
          }
          ++count;
       }
   } while(!feof(fd));  
   write(Cli_sock, "\r\n", 2);
   Success_func("Sending complete....Job well done Team!!");
   fclose(fd);
   free(buffer);
   free(response);
   free(Http_Header);
   return SUCCESS;
}


/* Start of main function
 * We are taking the Server port number 
 * from commandline arguments.
 */

int main ( int argc, char *argv[])
{
    struct sockaddr_in serv_sockaddr;
    int  serv_portno = -1, ret_val = 0;
    int sock_fd = -1, new_sockfd = -1;
    pid_t pid;

    /* Expecting Port no from ARGUMENT 2 */ 
    if (argc < 2) {
        Error_func("Commandline arguments are Wrong:");
        return FAILURE;
    }

    /* Create Socket */
    sock_fd = socket (AF_INET, SOCK_STREAM, 0);
    if (sock_fd < 0) {
        Error_func("Socket Creation Failed!!");
        return FAILURE;
    } else {
        Success_func("Socket creation completed!!!");
    }

    /* Get the port number from the command Line.
     * atoi is to change from ASCII to integer format.
     */
    serv_portno = atoi(argv[1]);

    memset(&serv_sockaddr, 0, sizeof(serv_sockaddr));
    /* Populating ServerSocket struct */
    serv_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_sockaddr.sin_family = AF_INET;
    serv_sockaddr.sin_port = htons(serv_portno);

    ret_val = bind(sock_fd, (struct sockaddr *)&serv_sockaddr, sizeof(serv_sockaddr));
    if (ret_val < 0 ) {
        Error_func("Error in binding!!");
        return FAILURE;
    } else {
        Success_func("Binding is Completed!!!");
    }
    
    /* Intially we can have a backlog of 20 Clients */
    ret_val = listen (sock_fd, 20); 
    if (ret_val < 0 ) {
        Error_func("Error in Listening!!");
        return FAILURE;
    } else {
        sprintf(msg, "Server is in listening mode on portno:%d!!!", serv_portno);
        Success_func(msg);
    }
   
    /*  Infinite while Loop to accept and service connections 
     *  only way to get out of the Loop is to press Ctrl+c
     *  and kill the process, Until then our Server will 
     *  always be in listening for any new connections.
     */
    while (1) {
        /*  Wait for connection  */
        new_sockfd = accept (sock_fd, NULL, NULL);
        if (new_sockfd < 0) {
            Error_func("Client socket accept Failed!!");
            return FAILURE;
        } else {
            Success_func("Client socket accept completed!!!");
        }

        /* Fork child process to service each one */
        pid = vfork();
	if (pid == 0) {

	    /*  This is the child process, so
	     *  close listening socket. 
             */
	    if (close(sock_fd) < 0) {
		Error_func("closing listening socket in child process.");
            }
	    
	    ret_val = Service_the_client_requestFunc(new_sockfd);
            if (ret_val) {
                Error_func("Something went wrong in servicing the Client!!");
            } else {
                Success_func("Service to Client Completed!!");
            }

	    /*  Close connected socket and successfully exit  */
	    if (close(new_sockfd) < 0) {
		Error_func("closing connection socket in child.");
            }
	    exit(EXIT_SUCCESS);
	}

	/*  if we are at this point, then we are still in the parent process,
	 *  so close the connected socket, clean up child processes,
	 *  and go back to loop accepting new connections.
         */

	if (close(new_sockfd) < 0) {
	    Error_func("closing connection socket in parent");
        }
	waitpid(-1, NULL, WNOHANG);
    }
    /* Some thing went Wrong, we shouldn't be here
     * But close the socket and terminate buddy.
     */
    if (close(sock_fd) < 0) {
        Error_func("closing connection socket.");
    }
    return FAILURE;
}



