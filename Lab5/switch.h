/* switch.h */

#define TABLE_LENGTH 100

typedef struct {
	packetBuffer sendPacketBuf;
	packetBuffer rcvPacketBuf;	
	switchQueue * head;
	switchQueue * tail;
	LinkInfo linkin[NUMHOSTS];
	LinkInfo linkout[NUMHOSTS];
	switchTable table;
} switchState;

typedef struct {
	packetBuffer data;
	switchQueue * next;
} switchQueue;

typedef struct {
	int valid[TABLE_LENGTH];
	int dstaddr[TABLE_LENGTH];
	int outlink[TABLE_LENGTH];
} switchTable;

void switchMain(switchState * sstate); //main loop for switch

void switchInit(switchState * sstate);

