#include <cstdlib>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>

#define NUM_THREADS  2

using namespace std;

/* PingStruct Class which four members sub1.sub2.sub3.sub4 (IPv4 address is split into four parts)
 * subnet contains the subnet mask.
 */
class PingStruct {
    private:
        unsigned int subnet;
        unsigned int sub1;
        unsigned int sub2;
        unsigned int sub3;
        unsigned int sub4;
        unsigned int min;
        unsigned int max;
    public:
        void setSubnet (unsigned int x) {
            subnet = x;
        }
        void setSub1 (unsigned int x) {
            sub1 = x;
        }
        void setSub2 (unsigned int x) {
            sub2 = x;
        }
        void setSub3 (unsigned int x) {
            sub3 = x;
        }
        void setSub4 (unsigned int x) {
            sub4 = x;
        }
         void setMin (unsigned int x) {
            min = x;
        }
         void setMax (unsigned int x) {
            max = x;
        }

        unsigned int getSubnet (void) {
            return (subnet);
        }
        unsigned int getSub1 (void) {
            return (sub1);
        }
        unsigned int getSub2 (void) {
            return (sub2);
        }
        unsigned int getSub3 (void) {
            return (sub3);
        }
        unsigned int getSub4 (void) {
            return (sub4);
        }
        unsigned int getMin  (void) {
            return (min);
        }
        unsigned int getMax  (void) {
            return (max);
        }
        void getIpstring (int i, char *pingstring);
        void SpiltIP (char *str);
};



void Action_api_ping(int i);
// Define the function to be called when ctrl-c (SIGINT) signal is sent to process
void signal_callback_handler(int signum)
{
printf("Caught signal %d\n",signum);
// Cleanup and close up stuff here
// Terminate program
exit(signum);
}

PingStruct ping;
std::mutex mu;

