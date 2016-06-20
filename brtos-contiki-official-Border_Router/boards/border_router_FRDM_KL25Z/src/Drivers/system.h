

#define GPIOA_BASE              0x400FF000  // GPI0 A
#define GPIOB_BASE              0x400FF040  // GPI0 B
#define GPIOC_BASE              0x400FF080  // GPI0 C
#define GPIOD_BASE              0x400FF0C0  // GPI0 D
#define GPIOE_BASE              0x400FF100  // GPI0 E

#if 0
#define PORTD_BASE              0x4004C000  // Port D multiplexing control

void Clock_init(void);
void Enable_LLS(void);
void Enable_LLWU(void);
void ReEnable_LLWU(void);
void Enable_INT_Pin(void);
#endif
