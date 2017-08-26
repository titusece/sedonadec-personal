// History:
//   22 Aug 17  Titus  creation

#include "sedona.h"

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <errno.h>

#include "BBBiolib.h"

// Uncomment this to print diagnostics
//#define SHOW_DEBUG


#define MIN_DIO_NUM 0
#define MAX_DIO_NUM 27

//
// For the Raspberry Pi, use only GPIOs available on P2 and 3
//
#define NUM_P1_GPIOS 7
#define NUM_PORTS	2
static int allowedGpios[NUM_P1_GPIOS] = { 12, 11, 16, 15, 23, 26, 18 };
static int allowedPorts[NUM_PORTS] = { 8, 9};//P8 & P9


// Flag to indicate whether one-time init has occurred since SVM started
static bool bIsGpioInitialized = FALSE;

//
// isValidGpio - Allow only certain GPIOs, defined by allowedGpios[] array
//
static bool isValidGpio(int which)
{
  int g;
  for (g=0; g<NUM_P1_GPIOS; g++)
    if (which == allowedGpios[g]) return TRUE;
  return FALSE;
}


static bool isValidPort(int Port)
{
  int g;
  for (g=0; g<NUM_PORTS; g++)
    if (Port == allowedPorts[g]) return TRUE;
  return FALSE;
}

// int Dio.doInit()
Cell BBBDigitalIo_Dio_BBB_doInit(SedonaVM* vm, Cell* params)
{
  int whichDio  = params[0].ival;
  int whichPort = params[1].ival;
  int bDir      = params[2].ival;

#ifdef SHOW_DEBUG
  printf("native:Dio_BBB.doInit( P%d:%d %s )\n", whichPort, whichDio, (bDir?"in":"out"));  
#endif

  //
  // Do one-time gpio setup if not already done
  //
  if (!bIsGpioInitialized)
  {
    iolib_init();

    bIsGpioInitialized = TRUE;
  }


  //
  // Check for invalid DIO
  //
  if ( !isValidGpio(whichDio) ) {
    printf("native:Dio_BBB.doInit( INVALID isValidGpio P%d:%d )\n", whichPort, whichDio);  
    return negOneCell;
  } 

  //
  // Check for invalid PORT
  //
  if ( !isValidPort(whichPort) ) {
    printf("native:Dio_BBB.doInit( INVALID isValidPort P%d:%d )\n", whichPort, whichDio);  
    return negOneCell;
  } 

  //
  // Configure DIO direction:
  //   If bDir==0 then configure DIO for output,
  //   if bDir==1 then configure DIO for input;
  //   o/w do nothing.
  //
  if (bDir==0)
  {
//    printf("native:Dio_BBB.doInit( BBBIO_DIR_OUT P%d:%d %s )\n", whichPort, whichDio, (bDir?"in":"out"));
    iolib_setdir(whichPort,whichDio, BBBIO_DIR_OUT);
  }
  else if (bDir==1)
  {
//    printf("native:Dio_BBB.doInit( BBBIO_DIR_IN P%d:%d %s )\n", whichPort, whichDio, (bDir?"in":"out"));
    iolib_setdir(whichPort,whichDio, BBBIO_DIR_IN);
  }


  return zeroCell;
}




// int  Dio.doRead()
Cell BBBDigitalIo_Dio_BBB_doRead(SedonaVM* vm, Cell* params)
{
  int whichDio = params[0].ival;
  int whichPort     = params[1].ival;

#ifdef SHOW_DEBUG
  printf("native:Dio_BBB.doRead( P%d:%d )\n", whichPort, whichDio); 
#endif

  //
  // Do one-time gpio setup if not already done
  //
  if (!bIsGpioInitialized)
  {
    iolib_init();

    bIsGpioInitialized = TRUE;
  }

  // 
  // Check for invalid DIO
  //
  if ( !isValidGpio(whichDio) )
    return negOneCell;

  //
  // Check for invalid PORT
  //
  if ( !isValidPort(whichPort) )
    return negOneCell;


  // Assume DIO is set up for input 

/*  int rdVal = readGpio() & DIO_CTRL_MASK(whichDio); 
  if (rdVal==0)
    return zeroCell;
  else
    return oneCell;
*/

  if (is_high(whichPort,whichDio)) {
//    printf("native:Dio_BBB.doRead( IS_HIGH P%d:%d )\n", whichPort, whichDio); 
    return oneCell;//
  }
  if (is_low(whichPort,whichDio)) {
//    printf("native:Dio_BBB.doRead( IS_LOW P%d:%d )\n", whichPort, whichDio); 
    return zeroCell;//
  }


}



// int  Dio.doWrite(bool bValue)
Cell BBBDigitalIo_Dio_BBB_doWrite(SedonaVM* vm, Cell* params)
{
  int whichDio  = params[0].ival;
  int whichPort = params[1].ival;
  int bValue    = params[2].ival;


#ifdef SHOW_DEBUG
  printf("native:Dio_BBB.doWrite( P%d:%d, %s )\n", whichPort, whichDio, (bValue?"true":"false")); 
#endif


  //
  // Do one-time gpio setup if not already done
  //
  if (!bIsGpioInitialized)
  {
//    printf("native:Dio_BBB.doWrite( NOT INIT P%d:%d )\n", whichPort, whichDio); 
    iolib_init();
    bIsGpioInitialized = TRUE;
  }

  //
  // Check for invalid DIO
  //
  if ( !isValidGpio(whichDio) ) {
    printf("native:Dio_BBB.doWrite( INVALID isValidGpio P%d:%d )\n", whichPort, whichDio); 
    return negOneCell;
  }

  //
  // Check for invalid PORT
  //
  if ( !isValidPort(whichPort) ) {
    printf("native:Dio_BBB.doWrite( INVALID isValidPort P%d:%d )\n", whichPort, whichDio); 
    return negOneCell;
  }


  // Assume DIO is set up for output 

  
/*
// If bValue==0 or 1 then write value to DIO;
  // o/w arg is "null" - do nothing.
  if (bValue==0)
    clrGpio(whichDio);
  else if (bValue==1)
    setGpio(whichDio);
*/

  if (bValue==0) {
//    printf("native:Dio_BBB.doWrite( PIN_LOW:  P%d:%d )\n", whichPort, whichDio); 
    pin_low(whichPort,whichDio);
  }
  else if (bValue==1) {
//    printf("native:Dio_BBB.doWrite( PIN_HIGH:  P%d:%d )\n", whichPort, whichDio); 
    pin_high(whichPort,whichDio);
  }

  return zeroCell;
}
