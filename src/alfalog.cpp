#include "alfalog.h"

std::vector<byte> alogScanI2C(TwoWire &i2c){
  std::vector<byte> ret;
  for (byte i = 8; i < 120; i++)
  {
    i2c.beginTransmission (i);
    if (i2c.endTransmission () == 0)
    {
      ret.push_back(i);
    }
  }
  return ret;
}

const char* alogGetInitString(){
    // a lot of macro preprocessing so that it does no
    // string concatenation at runtime

    #ifndef PROJECT_NAME
        #warning "you can set -DPROJECT_NAME in platformio.ini"
        #define PROJECT_NAME "firmware"
    #endif
    
    #ifdef FW_REV
        #define PRINT_FW_REV " rev. " FW_REV        
    #else
        #warning "you can set -DFW_REV in platformio.ini"
        #define PRINT_FW_REV " "
    #endif

    return
    PROJECT_NAME PRINT_FW_REV "\n\r"
    "Compiled " __DATE__ " " __TIME__;
}


AlfaLogger_ &AlfaLogger = AlfaLogger.getInstance();