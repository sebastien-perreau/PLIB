#ifndef __DEF_PWM
#define	__DEF_PWM

#define _OCxCON_ON_MASK         _OC1CON_ON_MASK
#define _OCxCON_OCTSEL_MASK     _OC1CON_OCTSEL_MASK

// ----------------------------------------
// ---------- Defines of typedef ----------
// ----------------------------------------
typedef enum {
#ifdef _OCMP1
    PWM1 = 0,   // PWM Module 1 ID.
#endif
#ifdef _OCMP2
    PWM2,    // PWM Module 2 ID.
#endif
#ifdef _OCMP3
    PWM3,    // PWM Module 3 ID.
#endif
#ifdef _OCMP4
    PWM4,    // PWM Module 4 ID.
#endif
#ifdef _OCMP5
    PWM5,    // PWM Module 5 ID.
#endif
    PWM_NUMBER_OF_MODULES   // Number of available PWM modules.
} PWM_MODULE;
// ----------------------------------------
typedef union {
  struct {
    unsigned OCM:3;
    unsigned OCTSEL:1;
    unsigned OCFLT:1;
    unsigned OC32:1;
    unsigned :7;
    unsigned SIDL:1;
    unsigned :1;
    unsigned OCPON:1;
  };
  struct {
    unsigned OCM0:1;
    unsigned OCM1:1;
    unsigned OCM2:1;
  };
  struct {
    unsigned :13;
    unsigned OCSIDL:1;
  };
  struct {
    unsigned w:32;
  };
} __OCxCONbits_t;
// ----------------------------------------
typedef struct {
    union {
	volatile UINT32 OCxCON;
	volatile __OCxCONbits_t OCxCONbits;
    };
    volatile UINT32 OCxCONCLR;
    volatile UINT32 OCxCONSET;
    volatile UINT32 OCxCONINV;

    volatile UINT32 OCxR;
    volatile UINT32 OCxRCLR;
    volatile UINT32 OCxRSET;
    volatile UINT32 OCxRINV;

    volatile UINT32 OCxRS;
    volatile UINT32 OCxRSCLR;
    volatile UINT32 OCxRSSET;
    volatile UINT32 OCxRSINV;
} PWM_REGISTERS;
// ----------------------------------------

void PWMInit(PWM_MODULE mPwmModule, DWORD config);
void PWMDutyCycle(PWM_MODULE mPwmModule, double dc);
double PWMGetDutyCycleResolution(PWM_MODULE mPwmModule);
double PWMGetDutyCycle(PWM_MODULE mPwmModule);
double PWMGetFrequency(PWM_MODULE mPwmModule);

#endif

