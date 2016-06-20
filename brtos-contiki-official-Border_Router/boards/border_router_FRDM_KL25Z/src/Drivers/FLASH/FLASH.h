
/*
** ===================================================================
** Global HAL types and constants
** ===================================================================
*/
typedef uint32_t LDD_TPinMask;         /*!< Pin mask type. */
typedef uint16_t LDD_TError;           /*!< Error type. */
typedef uint32_t LDD_TEventMask;       /*!< Event mask type. */
typedef uint8_t LDD_TClockConfiguration; /*!< CPU clock configuration type. */
typedef void LDD_TDeviceData;          /*!< Pointer to private device structure managed and used by HAL components. */
typedef void* LDD_TDeviceDataPtr;      /*!< Obsolete type for backward compatibility. */
typedef void LDD_TData;                /*!< General pointer to data. */
typedef void LDD_TUserData;            /*!< Pointer to this type specifies the user or RTOS specific data will be passed as an event or callback parameter. */

typedef uint8_t LDD_FLASH_TErrorFlags; /*!< Type specifying FLASH component's error flags bit field */

typedef uint32_t LDD_FLASH_TAddress;   /*!< Type specifying the Address parameter used by the FLASH component's methods */

typedef uint32_t LDD_FLASH_TDataSize;  /*!< Type specifying the Size parameter used by the FLASH component's methods */

typedef uint16_t LDD_FLASH_TErasableUnitSize; /*!< Type specifying the Size output parameter of the GetErasableUnitSize method (pointer to a variable of this type is passed to the method) */


void InitFlash(void);

void WriteToFlash(LDD_TData *FromPtr, LDD_FLASH_TAddress ToAddress, LDD_FLASH_TDataSize Size);

void ReadFromFlash(LDD_FLASH_TAddress FromAddress, LDD_TData *ToPtr, LDD_FLASH_TDataSize Size);

void EraseFlash(LDD_FLASH_TAddress FromAddress, LDD_FLASH_TDataSize Size);

