#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <unistd.h>
#include <fcntl.h>

#include "main.h"
#include "utilities.h"
#include "link.h"
#include "man.h"
#include "host.h"
#include "switch.h"
#include "net.h"

#define EMPTY_ADDR 0xffff
#define PIPEREAD    0
#define PIPEWRITE   1
#define TENMILLISEC 10000
//#define DEBUG

//initialization functions used by switchInit()
void switchInitRcvPacketBuf(packetBuffer * packetbuff);
void switchInitSendPacketBuf(packetBuffer * packetbuff);
void switchInitState(switchState * sstate, int physid);
void switchInitQueue(switchState * sstate);
void switchInitTable(switchState * sstate);

//functions for switch queue
void switchQueueInsert(switchState * sstate, packetBuffer packetbuff);
packetBuffer switchQueueRemove(switchState * sstate);

//functions for switch table
int switchTableSearch(switchTable table, int dstaddr);
int switchTableUpdate(switchState * sstate, packetBuffer packet, int link);

void switchMain(switchState * sstate)
{
    while(1){
        int i,j, sendAddr;
        for(i = 0; i < NUMHOSTS; i++){
            if(linkReceive(&(sstate->linkin[i]),&(sstate->rcvPacketBuf)) > 0){
                switchTableUpdate(sstate, sstate->rcvPacketBuf, i);
                switchQueueInsert(sstate, sstate->rcvPacketBuf);
            }
        }
        if(sstate->tail != NULL){
            sstate->sendPacketBuf = switchQueueRemove(sstate);
            if(sendAddr = switchTableSearch(sstate->table, sstate->sendPacketBuf.dstaddr) == -1){
                for(j = 0; j < NUMHOSTS; j++){
                    if(j != sstate->sendPacketBuf.srcaddr){
                        linkSend(&(sstate->linkout[j]), &(sstate->sendPacketBuf));
                    }
                }
            }
            else{
                linkSend(&(sstate->linkout[sendAddr]), &(sstate->sendPacketBuf));
            }
        }
        usleep(TENMILLISEC);
    }
}


//given dest addr
//returns out link
//if not found then returns -1
int switchTableSearch(switchTable table, int dstaddr)
{
    int i;
    for(i = 0; i < TABLE_LENGTH; i++){
        if(table.valid[i] == 1){
            if(table.dstaddr[i] == dstaddr) return table.outlink[i]; 
        }
    }
    return -1; //dest. addr not in table
}

int switchTableUpdate(switchState * sstate, packetBuffer packet, int link)
{
    int i,j, addr;
    for(i = 0; i < TABLE_LENGTH; i++){
        if(sstate->table.valid[i] == 1){
            if(sstate->table.dstaddr[i] == packet.dstaddr){
                sstate->table.outlink[i] = link;
                return 1;
            }
        }
    }
    for(j = 0; j < TABLE_LENGTH; j++){
        if(sstate->table.valid[j] == 0){
            sstate->table.dstaddr[j] = packet.dstaddr;
            sstate->table.outlink[j] = link;
            sstate->table.valid[j] = 1;
            return 1;
        }
    }
    return -1; //error
}

void switchQueueInsert(switchState * sstate, packetBuffer packet)
{
    switchNode * new;
    new = (switchNode*) malloc(sizeof(switchNode));
    new->packet = packet;
    new->next = NULL;
    // Insert into queue
    if (sstate->head == NULL) { // Insert into empty queue
        sstate->head = new;
        sstate->tail = new;
    }
    else {
        sstate->tail->next = new; // Insert at the tail
        sstate->tail = new;         
    }
}
packetBuffer switchQueueRemove(switchState * sstate)
{
    packetBuffer temp;
    temp = sstate->head->packet;
    sstate->head = sstate->head->next;
	return temp;
}

void switchInit(switchState * sstate, int physid)
{
    switchInitState(sstate, physid);
    switchInitRcvPacketBuf(&(sstate->rcvPacketBuf));
    switchInitSendPacketBuf(&(sstate->sendPacketBuf));
    switchInitQueue(sstate);
    switchInitTable(sstate);
}

void switchInitTable(switchState * sstate)
{
    int i;
    for(i = 0; i < TABLE_LENGTH; i++){
        sstate->table.valid[i] = 0;
        sstate->table.dstaddr[i] = 0;
        sstate->table.outlink[i] = 0;
    }
}

void switchInitQueue(switchState * sstate)
{
    sstate->head = NULL;
    sstate->tail = NULL;
}

void switchInitRcvPacketBuf(packetBuffer * packetbuff)
{
    packetbuff->valid = 0;
    packetbuff->new = 0;
}

void switchInitSendPacketBuf(packetBuffer * packetbuff)
{
    packetbuff->valid = 0;
    packetbuff->new = 0;
}

void switchInitState(switchState * sstate, int physid)
{
    sstate->physid = physid;
    sstate->netaddr = physid;
    sstate->rcvPacketBuf.valid = 0;
    sstate->rcvPacketBuf.new = 0;
}
