#define NO_ALIGN    (INT8U)0
#define SPACE_ALIGN (INT8U)1
#define ZEROS_ALIGN (INT8U)2


void Print4Digits(unsigned short int number, unsigned char align, char *buff);
void Print3Digits(unsigned short int number, unsigned char align, char *buff);
void Print2Digits(unsigned char number, unsigned char align, char *buff);
void PrintDecimal(signed short int val, char *buff);

#define ByteSwap(A)     (A=(A<<8)+(A>>8))
unsigned long int LWordSwap(unsigned long int u32DataSwap);

