/* switch.h */

#define TABLE_LENGTH 100

typedef struct {
	int physid;
	packetBuffer sendPacketBuf;
	packetBuffer rcvPacketBuf;	
	struct node * head;
	struct node * tail;
	LinkInfo linkin[NUMHOSTS];
	LinkInfo linkout[NUMHOSTS];
	//switchTable table;
} switchState;

typedef struct node{
	packetBuffer packet;
	struct node * next;
} switchNode;

/*
typedef struct {
	int valid[TABLE_LENGTH];
	int dstaddr[TABLE_LENGTH];
	int outlink[TABLE_LENGTH];
} switchTable;
*/
void switchMain(switchState * sstate); //main loop for switch

void switchInit(switchState * sstate, int physid);

