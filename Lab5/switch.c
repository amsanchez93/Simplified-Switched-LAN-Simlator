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

//initialization functions used by switchInit()
void switchInitRcvPacketBuf(packetBuffer * packetbuff);
void switchInitSendPacketBuf(packetBuffer * packetbuff);
void switchInitState(switchState * sstate, int physid);

void switchMain(switchState * sstate)
{
    packetBuffer tempbuff;
    
    while(1){
        int i,j, rcvSize;
        for(i = 0; i < NUMHOSTS; i++){
            rcvSize = linkReceive(&(sstate->linkin[i]),&(sstate->rcvPacketBuf));
            if(rcvSize > 0){
                sstate->rcvPacketBuf.valid = 1;
                sstate->rcvPacketBuf.new = 1;
                printf("received packet on link %d", i);
            }
        }
        
        for(j = 0; j < NUMHOSTS; j++){
            if(j != sstate->rcvPacketBuf.srcaddr){
                linkSend(&(sstate->linkout[j]), &(sstate->sendPacketBuf));
                printf("sent packet on link %d", j);
            }
        }
        usleep(TENMILLISEC);
    }
}

void switchInit(switchState * sstate, int physid)
{
    switchInitState(sstate, physid);
    switchInitRcvPacketBuf(&(sstate->rcvPacketBuf));
    switchInitSendPacketBuf(&(sstate->sendPacketBuf));
    
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