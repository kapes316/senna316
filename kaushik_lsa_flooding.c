/*
 * Main_Server.c
 * Author of this file - Kaushik Pidugu
 * References for sytem calls were taken from www.LinuxHowtos.org
 * Started coding on March 29...Too late!!.
 * Completed on April 5.
 * Implementing LSA flooding with my version of Shortest Path which
 * is Flooding only Valid packets and discarding Invalid packets.
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
#define NODE_CAPACITY   50

struct node
{
unsigned int Node_id; // to Identify node num //
unsigned int Base_node;
unsigned int Age; //if this node is new //
unsigned int Seq_num; // Assume the largest number is 1000. Seq to identify latest mesg //
int Node_Alive; //-1 means this node is dead!! //
unsigned int Cost;
char ip_addr[32];  // IPv4 address
unsigned short int Port_no;
int Directly_conneted; // -1 not directly connected, 1 -> directly connected, 0-> same node //
};

static int Routing_Table(struct node Packet[NODE_CAPACITY], int num_nodes)
{
    int i = 0;
    printf("\n ======Routing Table for this Node==============\n"); 
    for (i = 1; i <= num_nodes; i++) {
            printf("DESTINATION NODE -> %d",  i);
            printf("\t\t LINK COST -> %u",   Packet[i].Cost);
            printf("\t\t Connected :%d", Packet[i].Directly_conneted); 
            printf("\t\t ip address:%s", Packet[i].ip_addr); 
            printf("\n");
    }
    printf("\n =========END OF ROUTING TABLE===========================\n");
    return 0;
}


/* Start of main function
 * We are taking the Server port number 
 * from commandline arguments.
 */
int main ( int argc, char *argv[])
{
    struct sockaddr_in serv_sockaddr, Cli_addr;
    int sock_fd = -1, cli_fd = -1, ret_val = 0;
    struct node Packet[NODE_CAPACITY];
    struct node temp;
    int i = 0, counter = 0 ,j = 0, k = 0;
    int flag = 0 ;
    int num_nodes = 0, direct_nodes = 0;
    int id = -1;
    pid_t pid;
    pid_t pid1;
    int Direct_arr[NODE_CAPACITY];
    char ip, dummy;
    int cnt = 0;
    short int serv_portno = -1;
   
    /* Expecting Port no from ARGUMENT 2 */ 
    if (argc < 3) {
        printf("\n Commandline arguments are Wrong:");
        return FAILURE;
    }

    /* Create Server Socket */
    sock_fd = socket (AF_INET, SOCK_DGRAM, 0);
    if (sock_fd < 0) {
        printf("\n Socket Creation Failed!!");
        return FAILURE;
    } else {
        printf("\n Socket creation completed!!!");
    }
   
    /* Create client Socket */
    cli_fd = socket (AF_INET, SOCK_DGRAM, 0); 
    if (sock_fd < 0) {
        printf("\n Socket Creation Failed!!");
        return FAILURE;
    } else {
        printf("\n Socket creation completed!!!");
    } 


    /* Get the port number from the command Line.
     * atoi is to change from ASCII to integer format.
     */
    serv_portno = atoi(argv[2]);

    memset(&serv_sockaddr, 0, sizeof(serv_sockaddr));
    /* Populating ServerSocket struct */
    serv_sockaddr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_sockaddr.sin_family = AF_INET;
    serv_sockaddr.sin_port = htons(serv_portno);
    
    ret_val = bind(sock_fd, (struct sockaddr *)&serv_sockaddr, sizeof(serv_sockaddr));
    if (ret_val < 0 ) {
        printf("\n Error in binding!!");
        return FAILURE;
    } else {
        printf("\n Binding is Completed!!!");
    }
    
    for(i = 0; i < NODE_CAPACITY; i++) {
        memset(&Packet[i], 0, sizeof(Packet[i]));
        Packet[i].Node_Alive = -1;
        Packet[i].Node_id = i; 
        Packet[i].Base_node = -1;
        Packet[i].Directly_conneted = -1;
        Packet[i].Seq_num = (2^30); //Kinda of Infinity
        Packet[i].Age = 1;
        Packet[i].Cost = (2^30); //kinda of Infinity
        strcpy(Packet[i].ip_addr, "NOT CONNECTED");
        Packet[i].Port_no = -1;
    }

    printf("\n Node id for this node :");
    scanf("%d", &id);
    Packet[id].Node_id = id; 
    Packet[id].Base_node = id;
    Packet[id].Cost = 0;
    Packet[id].Node_Alive = 1;
    Packet[id].Directly_conneted = 0;
    Packet[id].Port_no = serv_portno; 
    strcpy(Packet[id].ip_addr, "MY HOST");
    printf("\n First give server ip of this node then continue then next");

    i = 1;
    counter = 0;
    j = 0;
    memset(Direct_arr, 0, sizeof(Direct_arr));

    while ((counter != -1) && ( i < NODE_CAPACITY) ) {
        if (i != id) {
        printf("\n ====User input for  :%d ---- %d  link =======", id, i); 
    	scanf("%c", &dummy);
    	printf("\n Is this link directly connected press [Y] or [N]");
    	scanf("%c", &ip);
    	Packet[i].Base_node = id;
        Packet[i].Node_id = i; 

        // Populating our Routing table //
    	if (ip == 'Y' || ip == 'y') {
            printf("\n Give me the Neighbour ip address mate:");
       	    scanf("%s", Packet[i].ip_addr);
	    printf("\n Give me the Neighbour ip Port number mate:");
            scanf("%hu", &Packet[i].Port_no);
      	    printf("\n  Link cost : ");
     	    scanf("%u", &Packet[i].Cost);
            Packet[i].Directly_conneted = 1;
            Packet[i].Node_Alive = 1;
            Packet[i].Age = Packet[i].Age + 1;
            Packet[i].Seq_num = Packet[i].Seq_num - 1;
	    Direct_arr[j] = i;
            ++j;
        }
    	printf("\n============DONE==================\n");
   	printf("\n Done with all the directed links...."
               "Please press -1 to terminate or press any number to continue:");
    	scanf("%d", &counter);
        }
    ++i;
    } 
   
    if (id == i ) {
        num_nodes = i;
    } else {
        num_nodes = i - 1;
    }

    direct_nodes = j;

    printf("\n Intial Routing Table \n");
    Routing_Table(Packet, num_nodes);

    /* Fork child process to send updates to neighbours */
    pid = vfork();
    if (pid == 0) {
       /*  This is the child process, sending the updates to all the connnected neighbours */
       for (i = 1 ; i <= num_nodes; i++) {
            if (Packet[i].Directly_conneted == 1) {
     		  /* Populating Client struct */
        	Cli_addr.sin_addr.s_addr = inet_addr(Packet[i].ip_addr);
        	Cli_addr.sin_family = AF_INET;
        	Cli_addr.sin_port = htons((unsigned short int)Packet[i].Port_no);
                Routing_Table(Packet, num_nodes);
                Packet[i].Age = Packet[id].Age + 1;
                Packet[i].Seq_num = Packet[id].Seq_num - 1;
        	sendto(cli_fd, (struct node *)&Packet[i], sizeof(Packet[i]), 0,
             		(struct sockaddr *)&Cli_addr, sizeof(Cli_addr));
                        j = 0;
                        k = 0;
                	while (j < direct_nodes) {
        	 		/* Send the Packet info to directly connected nodes */
                 		k = Direct_arr[j];
                                Packet[k].Age = Packet[i].Age;
                                Packet[k].Seq_num = Packet[i].Seq_num;
                                if (k != i) {
        	 		    sendto(cli_fd, (struct node *)&Packet[k], sizeof(Packet[k]), 0,
            		 	   	    (struct sockaddr *)&Cli_addr, sizeof(Cli_addr));
                                }
                         	++j;
                   	}
                
                }
        }
        exit(EXIT_SUCCESS);
    }

    // Press CTRL-C to get out 
    while (1) {
      // ADD TIMER HERE 30 Secs would be better
     memset(&temp, 0, sizeof(temp));
     recvfrom(sock_fd, (struct node *)&temp, sizeof(temp), 0, NULL, NULL);
      printf("\n temp.cost%d", temp.Cost );
      printf("\n temp.Basenode%d",temp.Base_node );
      printf("\n temp.AGE%d",temp.Age );
      printf("\n temp.seqno%d",temp.Seq_num);
      printf("\n PAcket.age%d", Packet[temp.Base_node].Age );
      printf("\n Packet.seqno%d", Packet[temp.Base_node].Seq_num );
      printf("\n temp.nodeid:%d",temp.Node_id);
      printf("\n Packet Received:");
      i = 1;
      flag = 0;

      /*
       * First check whether packer is valid or not, if it is valid
       * store in our Packet structure. Otherwise Discard the Packet.
       */
      if ((temp.Seq_num <= Packet[temp.Base_node].Seq_num)   &&
          (temp.Age     >= Packet[temp.Base_node].Age)       &&  
          (temp.Base_node != id)                              &&
          (temp.Cost     != 0)                                &&
          (temp.Node_Alive != -1) ) {    
          if ( (temp.Node_id == id) &&  
               (temp.Cost    < Packet[temp.Base_node].Cost) )  {
              Packet[temp.Base_node].Cost =  temp.Cost;
              Packet[temp.Base_node].Seq_num = temp.Seq_num;
              Packet[temp.Base_node].Age = temp.Age; 
              Packet[temp.Base_node].Node_Alive = 1;
              Packet[temp.Base_node].Base_node = id; 
              Packet[temp.Base_node].Node_id = temp.Base_node;
              cnt = temp.Base_node;
              flag = 1;
              printf("\n Received Valid Packet....But Wont flood as this is the destination");
          } else if (temp.Cost < Packet[temp.Node_id].Cost) {
              Packet[temp.Node_id].Cost = Packet[temp.Base_node].Cost + temp.Cost;
              Packet[id].Seq_num = temp.Seq_num;
              Packet[id].Age = temp.Age; 
              Packet[temp.Node_id].Node_Alive = 1;
              Packet[temp.Node_id].Base_node = id; 
              Packet[temp.Node_id].Node_id = temp.Node_id;
              flag = 1;
              cnt = temp.Node_id;
              printf("\n Received Valid Packet....will intiate flooding");
          } else {
              //Nothing!!!
          }
      } else { 
          // Dont flood as the above Packet we got is not valid.
          printf("\n NOT VALID: Do nothing Just Discard the Packet!!!! \n");
      }                     


      if (flag == 1) { 
          // Forking Chid process for flooding!!!
          printf ("\n Now start Flooding "); 
                /*
                 * Kaushik Machine Gun start shooting the Packets to other nodes, 
                 * Except the source it came from. we leave dat poor guy He is innocent!!!
                 * and can avoid unnecesssary looping.
                 */
   	  pid1 = vfork();
  	  if (pid1 == 0) {
              i = 0;
              j = 0;
              while (j < direct_nodes) {
                     i = Direct_arr[j];
                     if ( i != temp.Base_node) { 
                     	memset(&Cli_addr, 0, sizeof(Cli_addr));
                     	Cli_addr.sin_addr.s_addr = inet_addr(Packet[i].ip_addr);
                     	Cli_addr.sin_family = AF_INET;
        	     	Cli_addr.sin_port = htons((unsigned short int)Packet[i].Port_no);
                        Packet[cnt].Age = Packet[id].Age + 1;
                        Packet[cnt].Seq_num = Packet[id].Seq_num - 1;
                     	printf("\n Sending....................\n ");
                     	sendto(cli_fd, (struct node *)&Packet[cnt], sizeof(Packet[cnt]), 0,
                          		(struct sockaddr *)&Cli_addr, sizeof(Cli_addr));
                     }
                     ++j;
             }       
          exit(EXIT_SUCCESS);
          }
      }
      Routing_Table(Packet, num_nodes);
      waitpid(-1, NULL, WNOHANG);
    } // End of while Loop

    return SUCCESS;
}



