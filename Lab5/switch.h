/* switch.h */

#define TABLE_LENGTH 100
#define MAXHOSTS 100

typedef struct {
	int valid[TABLE_LENGTH];
	int dstaddr[TABLE_LENGTH];
	int outlink[TABLE_LENGTH];
} switchTable;

typedef struct {
	int physid;
	int numInLinks;
	int numOutLinks;
	packetBuffer sendPacketBuf;
	packetBuffer rcvPacketBuf;	
	struct node * head;
	struct node * tail;
	LinkInfo linkin[MAXHOSTS];
	LinkInfo linkout[MAXHOSTS];
	switchTable table;
} switchState;

typedef struct node{
	packetBuffer packet;
	struct node * next;
} switchNode;



void switchMain(switchState * sstate); //main loop for switch

void switchInit(switchState * sstate, int physid, int numlinks);

