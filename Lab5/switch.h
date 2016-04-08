/* switch.h */

#define TABLE_LENGTH 100

typedef struct {
	int physid;
	packetBuffer sendPacketBuf;
	packetBuffer rcvPacketBuf;	
	struct switchNode * head;
	struct switchNode * tail;
	LinkInfo linkin[NUMHOSTS];
	LinkInfo linkout[NUMHOSTS];
	//switchTable table;
} switchState;

typedef struct {
	packetBuffer packet;
	struct switchNode * next;
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

