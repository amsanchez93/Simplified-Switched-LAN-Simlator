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
#define DEBUG

//initialization functions used by switchInit()
void switchInitRcvPacketBuf(packetBuffer * packetbuff);
void switchInitSendPacketBuf(packetBuffer * packetbuff);
void switchInitState(switchState * sstate, int physid);
void switchInitQueue(switchState * sstate);

//functions for switch queue
void switchQueueInsert(switchNode * head, switchNode * tail, packetBuffer packetbuff);
packetBuffer switchQueueRemove(switchNode * head);


void switchMain(switchState * sstate)
{
    while(1){
        int i,j, rcvSize = 0;
        for(i = 0; i < NUMHOSTS; i++){
            if(linkReceive(&(sstate->linkin[i]),&(sstate->rcvPacketBuf)) > 0){
                switchQueueInsert(sstate->head, sstate->tail, sstate->rcvPacketBuf);
#ifdef DEBUG
                printf("\nreceived packet on link %d with source address %d\n", i, sstate->rcvPacketBuf.srcaddr);
#endif
            }
        }
        if(sstate->head != NULL){
            sstate->sendPacketBuf = switchQueueRemove(sstate->head);
            
            for(j = 0; j < NUMHOSTS; j++){
                if(j != sstate->sendPacketBuf.srcaddr){
                    linkSend(&(sstate->linkout[j]), &(sstate->sendPacketBuf));
#ifdef DEBUG
                    printf("\nsent packet on link %d\n", j);
#endif
                }
            }
        }
        usleep(TENMILLISEC);
    }
}

void switchQueueInsert(switchNode * head, switchNode * tail, packetBuffer packet)
{
    switchNode * new;
    new = (switchNode*) malloc(sizeof(switchNode));
    new->packet = packet;
    new->next = NULL;
    // Insert into queue
    if (head == NULL) { // Insert into empty queue
        head = new;
        tail = new;
    }
    else {
        tail->next = new; // Insert at the tail
        tail = new;         
    }
}
packetBuffer switchQueueRemove(switchNode * head)
{
    packetBuffer temp;
    temp = head->packet;
    head = head->next;
}

void switchInit(switchState * sstate, int physid)
{
    switchInitState(sstate, physid);
    switchInitRcvPacketBuf(&(sstate->rcvPacketBuf));
    switchInitSendPacketBuf(&(sstate->sendPacketBuf));
    switchInitQueue(sstate);
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