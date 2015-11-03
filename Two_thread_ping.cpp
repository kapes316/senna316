/*
 * Two_thread_ping.cpp
 *------------------------------------------------------
 * Description : Input a classless subnet...Get all
 * the Host IPs and start pinging..Using only two threads 
 * one from first and other from reverse order. incremention
 * done on number as the ips are conceived as a whole number
 * Getting the maximum and the minumum number by bit operations
 * Keeping and using 2 files tables for thread 1 and thread 2
 * is better than using one table. Each table will get the last 
 * last IP value...no unnecessary synchronization. one thread
 * just reads and other writes.
 * Discarding the misses...not keeping them in a file!!
 *-------------------------------------------------------
 * Author of this file - Kaushik Pidugu
 */

/* NOTE: OUTPUT FILE RESULT WILL BE IN ./ping_output.txt file (relative path) */

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <sstream>
#include <thread>
#include <mutex>
#include <bitset>
#include <string>
#include <cmath>
#include "user_header.h"

inline void Action_api_ping (int x)
{
 unsigned int bits = 0, temp = 0, base = 0, i = 0;
 std::fstream fp;
 string line;
 char *pingstring = NULL;

 if (!(pingstring = new char[50])) {
     cout << "\n memory allocation failed" << endl;
     exit(1);
 }
 bits = 32 - ping.getSubnet();
 /* Get the maximum value */
 temp = ((((ping.getSub1()) << 24) + ((ping.getSub2()) << 16) + ((ping.getSub3()) << 8) + (ping.getSub4())) + 1 );
 ping.setMin(temp);
 temp = 0;
 /* Get the minimum value */
 temp = ((((ping.getSub1()) << 24) + ((ping.getSub2()) << 16) + ((ping.getSub3()) << 8) + (ping.getSub4()))
          | (((int ) pow(2, bits))- 2));
 ping.setMax(temp);

 if (x == 0) {
     for (i = ping.getMin(); i <= ping.getMax(); i++) {
          // Get the IP number from table 2..so it doesn't overlap
          fp.open ("./ping_table2.txt", ios::in);
          if (!fp.is_open()){
              cout << "\n Error opening the file table 2!" << endl;
              exit(1);
          }
          getline(fp, line);
          std::istringstream istr(line);
          istr >> base;
          fp.close();
          if (i < base) {
              ping.getIpstring(i, pingstring);
              fp.open ("./ping_table1.txt", ios::out | ios::trunc);
              fp << i << endl;
              fp.close();
          }
     }
 } else if (x == 1) {
     for (i = ping.getMax(); i >= ping.getMin(); i--) {
          // Get the IP number from table 1..so it doesn't overlap
          fp.open ("./ping_table1.txt", ios::in);
          if (!fp.is_open()){
              cout << "\n Error opening the file table 1!" << endl;
              exit(1);
          }
          getline(fp, line);
          std::istringstream istr(line);
          istr >> base;
          fp.close();
          if (i >= base) {
              ping.getIpstring(i, pingstring);
              fp.open ("./ping_table2.txt", ios::out | ios::trunc);
              fp << i << endl;
              fp.close();
          }
     }
 } else {
     // Do nothing!!!
 }
delete [] pingstring;
}

/* Converting the integer to the IP address by bit operations */
inline void PingStruct::getIpstring (int i, char *pingstring)
{
 unsigned int ip1 = 0, ip2 = 0, ip3 = 0, ip4 = 0;
 char buffer[50];
 ip1 = ((i >> 24) & 255);
 ip2 = ((i >> 16) & 255);
 ip3 = ((i >> 8) & 255);
 ip4 = ((i) & 255);
 memset(buffer, 0, 50);
 // Mutually exclusive lock for file write
 mu.lock();
 sprintf(buffer, "%d.%d.%d.%d", ip1, ip2, ip3, ip4);
 sprintf(pingstring, "ping -w 1 %s > nul", buffer);
 if (!(system(pingstring))) {
     cout << "\n" << buffer << endl;
 } 
 //unlock now!!
 mu.unlock();
}

/* Spliting the IP address */
inline void PingStruct::SpiltIP (char *str)
{
char *temp = NULL;
char sub[4];
int  count = 0, i = 0;
char index[4];

temp = str;
count = 1;
while (*str != '/') {
       if (*str == '.') {
           index[i] = '\0';
           if (count == 1) {
               ping.setSub1((unsigned int) atoi(index));
           } else if (count == 2) {
               ping.setSub2((unsigned int) atoi(index));
           } else if (count == 3) {
               ping.setSub3((unsigned int) atoi(index));
           } else {
             // dis is it...I'm really angry now!!
           }
           memset(index, 0, sizeof(index));
           ++count;
           i = 0;
       } else {
           index[i] = *str;
           ++i;
       }
++str;
}
index[i] = '\0';
ping.setSub4((unsigned int) atoi(index));
++str;
strcpy(sub, str);
ping.setSubnet((unsigned int) atoi(sub));
str = temp;
}


int main ( int argc, char *argv[])
{
int i = 0;
char *str = NULL;
std::fstream fs;
std::thread th[NUM_THREADS];

if (!(str = new char[50])) {
   cout << "\n memory allocation failed:" << endl;
   return -1;
}

strcpy(str, argv[1]);
ping.SpiltIP(str);

fs.open ("./ping_table1.txt", ios::out | ios::trunc);
fs << "1" << endl;
fs.close();
fs.open ("./ping_table2.txt", ios::out | ios::trunc);
fs << "4294967295" << endl;
fs.close();
fs.open ("./ping_output.txt", ios::out | ios::trunc);
if (!(fs.is_open())){
    cout << "\n Error creating the file or clearing the file!" << endl;
    exit(1);
}
fs.close();

// Register signal and signal handler
signal(SIGINT, signal_callback_handler);
i = 0;
while (i < 2) {
    th[i] = std::thread (Action_api_ping, i);
    ++i;
}
i = 0;
while (i < 2) {
    th[i].join();
    ++i;
}
delete [] str;
return 0;
}



