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
void switchInitLinks(switchState * sstate, linkArrayType * linkArray);

//functions for switch queue
void switchQueueInsert(switchState * sstate, packetBuffer packetbuff);
packetBuffer switchQueueRemove(switchState * sstate);

//functions for switch table
int switchTableSearch(switchTable table, int dstaddr);
int switchTableUpdate(switchState * sstate, packetBuffer packet, int link);

void switchMain(switchState * sstate)
{
    while(1){
        int i,j, sendIndex = 0, sendAddr = 0, size = 0;
        for(i = 0; i < sstate->numInLinks; i++){
            size = linkReceive(&(sstate->linkin[i]),&(sstate->rcvPacketBuf));
            if(size > 0){
                sstate->rcvPacketBuf.payload[size] = '\0';
                //printf("received packet on incoming link %d\n",i);
                //printf("packet contents:"); puts(sstate->rcvPacketBuf.payload); printf("\n");
                if(switchTableUpdate(sstate, sstate->rcvPacketBuf, i) == -1)
                    printf("error in table update\n");
                switchQueueInsert(sstate, sstate->rcvPacketBuf);
                size = 0;
            }
        }
        if(sstate->head != NULL){
            sstate->sendPacketBuf = switchQueueRemove(sstate);
	    sendIndex = switchTableSearch(sstate->table, sstate->sendPacketBuf.dstaddr);
            if(sendIndex == -1){
		//printf("Dest Addr not found in table\n");
                for(j = 0; j < sstate->numOutLinks; j++){
                    if(j != sstate->sendPacketBuf.srcaddr){
			//printf("Sending packet on outgoing link %d\n",j);
                        linkSend(&(sstate->linkout[j]), &(sstate->sendPacketBuf));
                    }
                }
            }
            else{
		sendAddr = sstate->table.outlink[sendIndex];
		//printf("Dest Addr found, sending on outgoing link %d", sendAddr);
                linkSend(&(sstate->linkout[sendAddr]), &(sstate->sendPacketBuf));
            }
        }
        usleep(TENMILLISEC);
    }
}

void switchInitLinks(switchState * sstate, linkArrayType * linkArray)
{
    int i, j = 0;
    //find switch in links
    for(i = 0; i < linkArray->numlinks; i++){
        if(linkArray->link[i].uniPipeInfo.physIdDst == sstate->physid){
            sstate->linkin[j] = linkArray->link[i];
            sstate->numInLinks++;
            j++;
        }

    }
    j = 0;
    //find switch out links
    for(i = 0; i < linkArray->numlinks; i++){
        if(linkArray->link[i].uniPipeInfo.physIdSrc == sstate->physid){
            sstate->linkout[j] = linkArray->link[i];
            sstate->numOutLinks++;
            j++;
        }
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
            if(table.dstaddr[i] == dstaddr){
                //printf("index found at %d\n", i);
                return i;    //return index in table
            }
        }
    }
    //printf("address in table not found\n");
    return -1; //dest. addr not in table
}

int switchTableUpdate(switchState * sstate, packetBuffer packet, int link)
{
    int index;
    
    index = switchTableSearch(sstate->table, packet.srcaddr);
    
    if(index != -1){ //found in table
        //printf("updating table at index %d with dest addr %d to %d\n", index, sstate->table.dstaddr[index], packet.srcaddr);
        sstate->table.dstaddr[index] = packet.srcaddr;
        return 0;
    }
    else{
        //printf("index not found in table\n");
        int i;
        for(i = 0; i < TABLE_LENGTH; i++){
            if(sstate->table.valid[i] == 0){
                //printf("inserting into table at index %d with dest addr %d and link %d\n", i, packet.srcaddr, link);
                sstate->table.outlink[i] = link;
                sstate->table.dstaddr[i] = packet.srcaddr;
                sstate->table.valid[i] = 1;
                return 0;
            }
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
	//printf("inserting into empty queue\n");
        sstate->head = new;
        sstate->tail = new;
    }
    else {
	//printf("inserting into queue\n");
        sstate->tail->next = new; // Insert at the tail
        sstate->tail = new;         
    }
}
packetBuffer switchQueueRemove(switchState * sstate)
{
    packetBuffer temp;
    temp = sstate->head->packet;
	//printf("removing packet from queue\n");
    sstate->head = sstate->head->next;
	return temp;
}

void switchInit(switchState * sstate, int physid, int numlinks)
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
    sstate->rcvPacketBuf.valid = 0;
    sstate->rcvPacketBuf.new = 0;
    sstate->numInLinks = 0;
    sstate->numOutLinks = 0;
}
