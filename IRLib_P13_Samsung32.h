/* IRLibProtocols/IRLib_P13_Samsung32.h
Basic Samsung television remote controller
Samsung BN59-00865A

IRLibProtocols/IRLibProtocols.cpp:
    ,F("Samsung32")

IRLibProtocols/IRLibProtocols.h:
#define SAMSUNG32 13
#define LAST_PROTOCOL 13

IRLib2/IRLibAll.h:
#include <IRLib_P13_Samsung32.h>

*/


#ifndef IRLIB_PROTOCOL_13_H
#define IRLIB_PROTOCOL_13_H
#define IR_SEND_PROTOCOL_13		case 13: if(data2==0)data2=38;IRsendSamsung32::send(data,data2); break;
#define IR_DECODE_PROTOCOL_13	if(IRdecodeSamsung32::decode()) return true;
#ifdef IRLIB_HAVE_COMBO
	#define PV_IR_DECODE_PROTOCOL_13 ,public virtual IRdecodeSamsung32
	#define PV_IR_SEND_PROTOCOL_13   ,public virtual IRsendSamsung32
#else
	#define PV_IR_DECODE_PROTOCOL_13  public virtual IRdecodeSamsung32
	#define PV_IR_SEND_PROTOCOL_13    public virtual IRsendSamsung32
#endif
#ifdef IRLIBSENDBASE_H

class IRsendSamsung32: public virtual IRsendBase {
  public:
    void send(uint32_t data, uint8_t kHz=38) {
      /*if (data==REPEAT_CODE) {
        enableIROut(kHz);
        mark (564* 16); space(564*4); mark(564);space(572);delay(97);//actually 97572us
      } else {
        sendGeneric(data,32, 564*16, 564*8, 564, 564, 564*3, 564, kHz, true,108000);
      }*/
      sendGeneric(data, 32, 4500, 4500, 670, 670, 1530, 446, kHz, true, 108000);
    };
};
#endif  //IRLIBSENDBASE_H

#ifdef IRLIBDECODEBASE_H
class IRdecodeSamsung32: public virtual IRdecodeBase {
  public:
    bool decode(void) {
      resetDecoder();//This used to be in the receiver getResults.
      /*IRLIB_ATTEMPT_MESSAGE(F("Samsung32 repeat"));
      // Check for repeat
      if (recvGlobal.decodeLength == 4 && MATCH(recvGlobal.decodeBuffer[1],564*16) && MATCH(recvGlobal.decodeBuffer[2],564*4) 
          && MATCH(recvGlobal.decodeBuffer[3],564)) {
        bits = 0;
        value = REPEAT_CODE;
        protocolNum = NEC;
        return true;
      }*/
      IRLIB_ATTEMPT_MESSAGE(F("Samsung32"));
	  if (recvGlobal.decodeLength != 68) return RAW_COUNT_ERROR;
      if (!MATCH(recvGlobal.decodeBuffer[1],4500)) return HEADER_MARK_ERROR(4500);
      if (!MATCH(recvGlobal.decodeBuffer[2],4500)) return HEADER_SPACE_ERROR(4500);
      if(!decodeGeneric(68, 4500, 4500, 670, 1530, 446)) return false;
      protocolNum = SAMSUNG32;
      return true;
    }
};

#endif //IRLIBDECODEBASE_H

#define IRLIB_HAVE_COMBO

#endif //IRLIB_PROTOCOL_13_H
