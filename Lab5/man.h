#define MAXHOSTS 100

typedef struct{ /* Connection used by the manager to a host */ 
   int toHost[2]; /* Pipe link to host */
   int fromHost[2]; /*  Pipe link from host */
} managerLink;

typedef struct {
   int numlinks;
   managerLink link[MAXHOSTS];
} manLinkArrayType;

/* 
 * Main loop for the manager.  It repeatedly gets command from
 * the user and then executes the command
 */
void manMain(manLinkArrayType * manLinkArray);


