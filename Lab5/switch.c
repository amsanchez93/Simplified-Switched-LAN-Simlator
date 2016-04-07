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
        int i,j, rcvSize = 0;
        for(i = 0; i < NUMHOSTS; i++){
            linkReceive(&(sstate->linkin[i]),&tempbuff);
            if(tempbuff.valid == 1 && tempbuff.new == 1){
                sstate->rcvPacketBuf = tempbuff;
                sstate->rcvPacketBuf.valid = 1;
                printf("received packet on link %d\n", i);
            }
        }
        if(sstate->rcvPacketBuf.valid == 1){
            sstate->sendPacketBuf.srcaddr = sstate->rcvPacketBuf.srcaddr;
            sstate->sendPacketBuf.dstaddr = sstate->rcvPacketBuf.dstaddr;
            sstate->sendPacketBuf.length = sstate->rcvPacketBuf.length;
            for(i = 0; i < sstate->sendPacketBuf.length;i++){
                sstate->sendPacketBuf.payload[i] = sstate->rcvPacketBuf.payload[i];
            }
            sstate->sendPacketBuf.valid = sstate->rcvPacketBuf.valid;
            sstate->sendPacketBuf.new = sstate->rcvPacketBuf.new;

            for(j = 0; j < NUMHOSTS; j++){
                if(j != sstate->sendPacketBuf.srcaddr){
                    linkSend(&(sstate->linkout[j]), &(sstate->sendPacketBuf));
                    printf("sent packet on link %d\n", j);
                }
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