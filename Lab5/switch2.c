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
/*
void switchInitState(struct switchState * sstate); 
void switchInitRcvPacketBuff(packetBuffer * packetbuff);
void switchInitSendPacketBuff(packetBuffer * packetbuff);
void switchInitQueue(struct switchState * sstate);
void switchInitTable(struct switchState * sstate);

void switchInsertQueue(struct switchState * sstate, packetBuffer * packet);
packetBuffer switchPopQueue(struct switchQueue * head);

int switchTableSearch(struct switchTable table, int srcaddr);
void switchTableUpdate(struct switchTable table, int tbl_loc, int srcaddr, int linkaddr);

void switchTransmitPacket(struct switchState * sstate, packetBuffer outPacket, int link);
*/


// main loop for the switch
void switchMain(struct switchState * sstate){
int i,j;
packetBuffer tempbuff;
	while(1){
		for(i = 0; i < NUMHOSTS; i++){
			linkReceive(&(sstate->linkin[i]),&tempbuff);
			if(tempbuff.valid != 0){
				for(j = 0; j < NUMHOSTS; j++){
					if(j != i) linkSend(&(sstate->linkout[j]), &tempbuff);
				}
			}
		}	
	}
}

/*

void switchInsertQueue(struct switchState * sstate, packetBuffer * packet){
    new = (struct switchQueue*) malloc(sizeof(struct switchQueue));
    new->data.srcaddr = packet->srcaddr;
    new->data.dstaddr = packet->dstaddr;
    new->data.length =  packet->length;
    new->data.payload = packet->payload;
    new->data.valid = packet->valid;
    new->data.new = packet->new;
    new->next = NULL;
    
    if (sstate->head==NULL) { // Insert into empty queue
    	sstate->head=new;
    	sstate->tail=new;
    }
    else {
    	sstate->tail->next = new; // Insert at the tail
    	sstate->tail=new;         
    }
}

packetBuffer switchPopQueue(struct switchQueue * head){
    packetBuffer temp;
    temp = head->data;
    head = head->next;
    return temp;
}

int switchTableSearch(struct switchTable table, int srcaddr){
    int i;
    for(i = 0; i < TABLE_LENGTH; i++){
        if(table.valid[i] == 1){
            if(table.dstaddr[i] == srcaddr){
                return i;
            }
        }
    } // did not find in table so search for empty space in table to put addr
    return -1;
}

void switchTableUpdate(struct switchTable table, int tbl_loc, int srcaddr, int linkaddr){
    if(tbl_loc != -1){
        table.valid[table_loc] = 1;
        table.dstaddr[table_loc] = srcaddr;
        table.outlink[table_loc] = linkaddr;
    }
    else{
        int i;
        for(i = 0; i < TABLE_LENGTH; i++){
            if(table.valid[i] == 0){
                table.valid[i] = 1;
                table.dstaddr[i] = srcaddr;
                table.outlink[i] = linkaddr;
            }
        }
    }

void switchInit(struct switchState * sstate){
    switchInitState(sstate);
    
    switchInitRcvPacketBuff(&(sstate->rcvPacketBuf));
    switchInitSendPacketBuff(&(sstate->sendPacketBuf));
    
    switchInitTable(sstate);
    switchInitQueue(sstate);
}

void switchInitState(struct switchState * sstate){
    sstate->rcvPacketBuf.valid = 0;
    sstate->rcvPacketBuf.new = 0;
}

void switchInitSendPacketBuff(packetBuffer * packetbuff){
packetbuff->valid = 0;
packetbuff->new = 0;
}

void switchInitRcvPacketBuff(packetBuffer * packetbuff){
packetbuff->valid = 0;
packetbuff->new = 0;
}

void switchInitTable(struct switchState * sstate){
    int i;
    for(i = 0; i < TABLE_LENGTH; i++){
        sstate->table.valid[i] = 0;
        sstate->table.dstaddr[i] = 0;
        sstate->table.outlink[i] = 0;
    }
}

void switchInitQueue(struct switchState * sstate){
    sstate->head = NULL;
    sstate->tail = NULL;
}

switchTransmitPacket(struct switchState * sstate, packetBuffer * outPacket, int link){
    linkSend(&(sstate->linkout[link]), outPacket);
}

*/
