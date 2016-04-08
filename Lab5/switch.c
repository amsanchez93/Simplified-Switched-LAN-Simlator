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

//functions for switch queue
void switchQueueInsert(switchState * sstate, packetBuffer packetbuff);
packetBuffer switchQueueRemove(switchState * sstate);


void switchMain(switchState * sstate)
{
    while(1){
        int i,j, rcvSize = 0;
        for(i = 0; i < NUMHOSTS; i++){
            if(linkReceive(&(sstate->linkin[i]),&(sstate->rcvPacketBuf)) > 0){
                switchQueueInsert(sstate, sstate->rcvPacketBuf);
            }
        }
        if(sstate->tail != NULL){
            sstate->sendPacketBuf = switchQueueRemove(sstate);
            for(j = 0; j < NUMHOSTS; j++){
                if(j != sstate->sendPacketBuf.srcaddr){
                    linkSend(&(sstate->linkout[j]), &(sstate->sendPacketBuf));
                }
            }
        }
        usleep(TENMILLISEC);
    }
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
        sstate->switchTable.valid[i] = 0;
        sstate->switchTable.dstaddr[i] = 0;
        sstate->switchTable.outlink[i] = 0;
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
    sstate->rcvPacketBuf.valid = 0;
    sstate->rcvPacketBuf.new = 0;
}
