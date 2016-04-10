#ifndef LPTMR0_H_
#define LPTMR0_H_

/* MODULE LPTMR0. */

/* Including shared modules, which are used in the whole project */
#include "MKL25Z4.h"

/* Peripheral base address parameter */
#define LPTMR0_DEVICE LPTMR0_BASE_PTR


/*
** ===================================================================
**     Method      :  LPTMR0_Init (component Init_LPTMR)
**     Description :
**         This method initializes registers of the LPTMR module
**         according to the Peripheral Initialization settings.
**         Call this method in user code to initialize the module. By
**         default, the method is called by PE automatically; see "Call
**         Init method" property of the component for more details.
**     Parameters  : None
**     Returns     : Nothing
** ===================================================================
*/
void LPO_Init(unsigned int module);

#endif
