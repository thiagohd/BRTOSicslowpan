#include "cpu.h"

void enable_irq (int irq)
{
    /* Make sure that the IRQ is an allowable number. Up to 32 is
     * used.
     *
     * NOTE: If you are using the interrupt definitions from the header
     * file, you MUST SUBTRACT 16!!!
     */
    if (irq > 32)
    {
        //printf("\nERR! Invalid IRQ value passed to enable irq function!\n");
    }
    else
    {
      /* Set the ICPR and ISER registers accordingly */
      NVIC_ICPR |= 1 << (irq%32);
      NVIC_ISER |= 1 << (irq%32);
    }
}
