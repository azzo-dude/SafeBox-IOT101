#ifndef DEBUG_H
#define DEBUG_H

// First, include the configuration file to check the mode
#include "SafeBoxConfig.h"

// Check if DEBUG_SERIAL_MODE is defined and set to TRUE
#if defined(DEBUG_SERIAL_MODE) && DEBUG_SERIAL_MODE == TRUE

  // If TRUE, define the debug macros to print to the Serial port
  #define DEBUG_PORT Serial
  
  // Macro to initialize the serial port
  #define DEBUG_BEGIN(baud) DEBUG_PORT.begin(baud)
  
  // Macros for printing
  #define DEBUG_PRINT(...) DEBUG_PORT.print(__VA_ARGS__)
  #define DEBUG_PRINTLN(...) DEBUG_PORT.println(__VA_ARGS__)
  #define DEBUG_PRINTF(...) DEBUG_PORT.printf(__VA_ARGS__)

#else

  #define DEBUG_BEGIN(baud) ((void)0)
  #define DEBUG_PRINT(...) ((void)0)
  #define DEBUG_PRINTLN(...) ((void)0)
  #define DEBUG_PRINTF(...) ((void)0)

#endif // DEBUG_SERIAL_MODE

#endif // DEBUG_H
