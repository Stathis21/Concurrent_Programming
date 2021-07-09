char * fifo;
char * start;
char * end;
int write_index;
int read_index;
int elements;
int capacity;
int closed;

volatile int crit_write;
volatile int crit_read;


void pipe_init(int size);

void pipe_write(char c);

int pipe_read(char *c);

void pipe_close();
