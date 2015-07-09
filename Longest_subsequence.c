/*
 * sequence.c
 * Author of this file - Kaushik Pidugu
 * Started coding on June 25th.
 * Completed on July 9th, But solve the problem 4 and upload in Github(kapes316).
 * Do that in summer buddy!!!
 */

 /* The Project is Strictly Increasing!!
  * Problem 4 count all the subsequence testing is failing
  * Rest three problem passed the testing and output is corrent!!
  */

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

// This is TOP_STACK LINEAR LINKED LIST. Inside the structure there a stack structure!!!
struct stacklist
{
 int tor_data;
 struct  stacklist *link;
 struct  stackseq  *stk_addr;
};

// this is each stack strucuture...only backtracing...move top to down!!
struct stackseq 
{
 int number;
 struct stackseq *left;
 struct stackseq *down;
};

int TOTAL_STACK = 0; //Global how many stacks

//Limit is 100 Subsequence....change this if you want to give more!!!
struct stacklist *start[100] = { }; //top of stack head
struct stacklist *head = NULL, *tail = NULL;


/* print function to print all the stacks for that 
 * particular subsequence.
 */
int print(struct stacklist *head)
{
struct stacklist *temp = NULL;
struct stackseq *st = NULL;
int count = 1;

if (head == NULL) {
    printf("\n stack is empty");
    return FAILURE;
}

printf("\n===================================START======SEQUENCE===================================================\n");
temp = head->link;
while (temp != NULL)  {
    st = temp->stk_addr;
    if (st != NULL) {
        printf("\n --------STACK:%d----TOP--------->DOWN-----------------\n",count);
        while (st != NULL) {
               printf("\t %d", st->number);
               st = st->down;
        }
        printf("\n-----------------------------------------------------\n");
    }
++count;
temp = temp->link;
}
return 0;
}

/* My main function to push into the stack
 * with the input number or create new one.
 */
int create_or_push ( int number, struct stacklist *head)
{
 struct stacklist *temp = NULL, *new = NULL, *prev = NULL;
 struct stackseq *st = NULL;

 if (tail->tor_data == -1) { //New stack
     new = (struct stacklist *) malloc(sizeof(struct stacklist ));
     if (new == NULL) {
         printf("\n Memory allocation failed dude:");
         return FAILURE;
     }
     new->tor_data = number;
     new->link = NULL;
     head->link = new;
     tail = new;
     head->tor_data = 0;
     st = (struct stackseq *)malloc(sizeof(struct stackseq ));
     if (st == NULL) {
         printf("\n Memory allocation failed dude:");
         return FAILURE;
     }
     st->number = number;
     ++TOTAL_STACK;
     st->left = NULL;
     st->down = NULL;
     new->stk_addr = st;
 } else { //stack list is not empty buddy!!
     temp = head->link;
     prev = head;
     while (temp != NULL) {
         // For same numbers, it tries put it in a different stack!!!
         if (temp->tor_data > number) {
             break;
         } 
         prev = temp;
         temp = temp->link;
     }
     st = (struct stackseq *)malloc(sizeof(struct stackseq ));
     if (st == NULL) {
         printf("\n Memory allocation failed dude:");
         return FAILURE;
     }
     st->number = number;
     if (temp == NULL) { //create a new stack sequence list
         ++TOTAL_STACK;
         new = (struct stacklist *) malloc(sizeof(struct stacklist ));
         if (new == NULL) {
             printf("\n Memory allocation failed dude:");
             return FAILURE;
         }
         new->tor_data = number;
         new->link = NULL;
         prev->link = new;
         new->stk_addr = st;
         st->down = NULL;
         st->left = prev->stk_addr;
         tail = new;
      } else {
         temp->tor_data = number; //change top of stack
         st->down = temp->stk_addr;
         temp->stk_addr = st;
         tail = temp;
         if (prev != head) {
             st->left = prev->stk_addr;
         } else {
             st->left = NULL;
         }
     } 
 } 
 return 0;
}

/* Function to find the smallest subsequence in increasing */
int Find_smallest_sum_sequence( struct stacklist *head) 
{
 //Smallest number in a stack and increasing subsequence, where the sum is smallest
 // is always the top of the stack.
 unsigned long long sum = 0;
 struct stackseq *st = NULL;
 struct stacklist *temp = NULL;

 if (head == NULL) {
     return FAILURE;
 }

 temp = head;
 while (temp->link != NULL) {
        temp = temp->link;
 }
     
 st = temp->stk_addr;
 printf("\n------------------SMALLEST SUBSEQUENCE------------------------------------------------\n");
 while (st != NULL) {
     sum += (unsigned long long)st->number;
     printf("\t %d", st->number);
     st = st->left;
 } //end of loop
 printf("\n--------------------------------------------------------------------------------------\n");
 printf("\n**********************************************************\n");
 printf("\n SMALLEST SUBSEQUENCE SUM :%llu\n",sum);
 printf("\n**********************************************************\n");
 return 0;
} //end of function 



/* Function to find the Largest subsequence in increasing */
int Find_largest_sum_sequence( struct stacklist *head)
{
 struct stackseq *st = NULL, *prev = NULL, *right = NULL, *current = NULL, *last = NULL;
 long long sum = 0;
 int count = 1;
 struct stacklist *temp = NULL;

 if (head == NULL) {
     return FAILURE;
 }

 temp = head;
 while (temp->link != NULL) {
        temp = temp->link;
 }

 st = temp->stk_addr;
 while (st->down != NULL) {
        st = st->down;
 }
 sum = (long long)st->number;
 printf("\n-----------------------------------LARGEST SUBSEQUENCE----------------------------------\n");
 printf("\t %d", st->number);
 right = st;
 st = st->left;
 last = st;
 while (st != NULL) {
    current = st;
    while (current != NULL) {
           prev = current->left;
           if (prev == NULL) {
               last = current;
               break;
           }
           if ((right->number < current->number) &&
               (prev->number  > current->number) ) {
                break;
           } else {
               last = current;
           }  
           current = current-> down;
    }
 st = last; 
 sum += (long long)st->number;
 printf("\t %d", st->number);
 right = st;
 st = st -> left;
 ++count;
 }
 TOTAL_STACK = count;
 printf("\n--------------------------------------------------------------------------------------\n");
 printf("\n**********************************************************\n");
 printf("\n LARGEST SUM OF LUS : %llu AND LENGTH :%d\n",sum, count);         
 printf("\n**********************************************************\n");
 return 0;
}

/* Function to find the length of the stack1 */

int Length_stack1( struct stacklist *head)
{
 struct stacklist *temp = NULL;
 struct stackseq  *st = NULL;
 unsigned long long sum = 0;
 int count = 0;

 if (head == NULL) {
     return FAILURE;
 }

 temp = head -> link;
 st = temp->stk_addr;
// printf("\n-----------------------------------STACK1----------------------------------\n");
 while (st  != NULL) {
     ++count;
     sum += (unsigned long long)st->number;
  //   printf("\t %d", st->number);
     st = st->down;
 }
// printf("\n---------------------------------------------------------------------\n");
// printf("\n**********************************************************\n");
 printf("\n STACK SUM1 : %llu AND LEGNTH :%d\n",sum, count);
 printf("\n**********************************************************\n");
 return 0;
} //end of function


//Count ALL subsequence testing is failing....Kaushik think again about the logic
int Count_All_Subsequence(struct stacklist *head)
{

 struct stackseq *st = NULL, *current = NULL, *last = NULL;
 struct stackseq *temp = NULL, *prev = NULL, *right = NULL;
 long long count = 0;
 struct stacklist *node = NULL;

 if (head == NULL) {
     return FAILURE;
 }

 node = head;
 while (node->link != NULL) {
        node = node->link;
 }

 st = node->stk_addr;
 right = st;
 st = st->left;
 last = st;

 while (st != NULL) {
        temp = st;
        while (temp != NULL) {
               current = temp;
               while (current != NULL) {
                      prev = current->left;
                      if (prev == NULL) {
                          last = current;
                          break;
                      }
                      // For non decreasing numbers
                      if ((right->number < current->number) &&
                          (prev->number  > current->number) ) {
                          break;
                      } else {
                          last = current;
                      }  
                       current = current-> down;
                       ++count;
               }
               st = last; 
               right = temp;
               temp = temp -> left;
        }
 st = st->down;
 ++count;
 }
 --count;
 //printf("\n-------------------------NO OF SEQUENCE--------------------------------------------------------\n");
 //printf("\n**********************************************************\n");
// printf("\n NO OF SUBSEQUENCES : %lld\n",count);         
// printf("\n**********************************************************\n");
 return 0;
}



/* Start of main function
 * We are taking the input CSV file 
 */
int main ( int argc, char *argv[])
{
 FILE *fp = NULL;
 char file_path[100] = {0};
 int input = 0;
 int flag = 0;
 int i = 0;

 memset(file_path, 0, 100);
 strcpy(file_path, "./RandomUpSequenceSS15large4.dat");  
 for(i = 0; i < 100; i++) { 
     start[i] = (struct stacklist *)malloc(sizeof(struct stacklist ));
     if (start[i] == NULL) {
         printf("\n Memory allocation failed dude:");
         return FAILURE;
     } else {
         start[i]->tor_data = -1;
         start[i]->link = NULL;
         start[i]->stk_addr = NULL;
     }
 }

    fp = fopen(file_path, "r+");
    if (fp == NULL) {
        printf("\n Couldn't open the file dude:");
        return FAILURE;
    }
 
    i = 0;
    head = start[0];
    tail = head;
    
    while (!feof(fp) && i < 99)
    {
      fscanf(fp, "%d,", &input);
      if (input == 0 && flag == 1) {
          break;
      }
      if (input == 0 ) {
          print(start[i]);
          Find_smallest_sum_sequence(start[i]);
          Find_largest_sum_sequence(start[i]);
          Length_stack1(start[i]);
          //Count_All_Subsequence(start[i]);
          ++i;
          flag = 1;
          head = start[i];
          tail = head;
	  printf("\n\n==================================END=============================================================\n");
      } else {  
        //  printf("  %d",input);
          TOTAL_STACK = 0;
          create_or_push(input, start[i]);
          flag = 0;
      }
    }

    fclose(fp);
    return SUCCESS;
}

