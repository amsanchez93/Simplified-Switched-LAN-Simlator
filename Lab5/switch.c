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

void switchInitState(hostState * sstate); 
void switchInitRcvPacketBuff(packetBuffer * packetbuff);
void switchInitSendPacketBuff(packetBuffer * packetbuff);
void switchInitQueue(void);
void switchInitTable(void);

void switchInsertQueue(switchQueue * head, packetBuffer * packet);
packetBuffer switchPopQueue(switchQueue * head);

int switchTableSearch(int srcaddr);

void switchTransmitPacket(switchState * sstate, packetBuffer outPacket, int link);



// main loop for the switch
void switchMain(switchState * sstate){
    packetBuffer * otempbuff, * itempbuff;
    int link_out, tbl_link;
    int i = 0;
    
    while(1){
        for(i = 0; i < NUMHOSTS>; i++){
            linkReceived(&(sstate->linkin[i]), itempbuff); 
            if(itempbuff.valid != 0){
                switchInsertQueue(sstate->head, itempbuff);
                tbl_link = switchTableSearch(sstate->table, itempbuff->srcaddr);
                switchTableUpdate(sstate->table, tbl_link, itempbuff->srcaddr, i);
            }
        }
        
        otempbuff = switchPopQueue(sstate->head);
        link_out = switchTableSearch(sstate->table, otempbuff->dstaddr);
        if(link_out == -1){  //not found in table; send on all links
            int j;
            for(j = 0; j < NUMHOSTS; j++){
                if(otempbuff.dstaddr != sstate->table[link_out]){
                    switchTransmitPacket(sstate, otempbuff, j);
                }
            }
        }
        else{
            switchTransmitPacket(sstate, otempbuff, link_out);
        }
        
        usleep(TENMILLISEC);
    }
}

void switchInsertQueue(switchState * sstate, packetBuffer * packet){
    new = (switchQueue*) malloc(sizeof(switchQueue));
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

packetBuffer switchPopQueue(switchQueue * head){
    packetBuffer temp;
    temp = head->data;
    head = head->next;
    return temp;
}

int switchTableSearch(switchTable table, int srcaddr){
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

void switchTableUpdate(switchTable table, int tbl_loc, int srcaddr, int linkaddr){
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

void switchInit(switchState * sstate){
    switchInitState(sstate);
    
    switchInitRcvPacketBuff(&(sstate->rcvPacketBuf));
    switchInitSendPacketBuff(&(sstate->sendPacketBuf));
    
    switchInitTable(sstate);
    switchInitQueue(sstate);
}

void switchInitState(switchState * sstate){
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

void switchInitTable(switchState * sstate){
    int i;
    for(i = 0; i < TABLE_LENGTH; i++){
        sstate->table.valid[i] = 0;
        sstate->table.dstaddr[i] = 0;
        sstate->table.outlink[i] = 0;
    }
}

void switchInitQueue(switchState * sstate){
    sstate->head = NULL;
    sstate->tail = NULL;
}

switchTransmitPacket(switchState * sstate, packetBuffer * outPacket, int link){
    linkSend(&(sstate->linkout[link]), outPacket);
}