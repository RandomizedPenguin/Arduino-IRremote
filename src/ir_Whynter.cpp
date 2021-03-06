#include "IRremote.h"

//==============================================================================
//               W   W  H   H  Y   Y N   N TTTTT EEEEE  RRRRR
//               W   W  H   H   Y Y  NN  N   T   E      R   R
//               W W W  HHHHH    Y   N N N   T   EEE    RRRR
//               W W W  H   H    Y   N  NN   T   E      R  R
//                WWW   H   H    Y   N   N   T   EEEEE  R   R
//==============================================================================

#define WHYNTER_BITS            32
#define WHYNTER_HEADER_MARK   2850
#define WHYNTER_HEADER_SPACE  2850
#define WHYNTER_BIT_MARK       750
#define WHYNTER_ONE_SPACE     2150
#define WHYNTER_ZERO_SPACE     750

//+=============================================================================
#if SEND_WHYNTER
void IRsend::sendWhynter(unsigned long data, int nbits) {
    // Set IR carrier frequency
    enableIROut(38);

    // Start
    mark(WHYNTER_BIT_MARK);
    space(WHYNTER_ZERO_SPACE);

    // Header
    mark(WHYNTER_HEADER_MARK);
    space(WHYNTER_HEADER_SPACE);

    // Data
    sendPulseDistanceData(data, nbits, WHYNTER_BIT_MARK, WHYNTER_ONE_SPACE, WHYNTER_ZERO_SPACE);
//    for (unsigned long mask = 1UL << (nbits - 1); mask; mask >>= 1) {
//        if (data & mask) {
//            mark(WHYNTER_ONE_MARK);
//            space(WHYNTER_ONE_SPACE);
//        } else {
//            mark(WHYNTER_ZERO_MARK);
//            space(WHYNTER_ZERO_SPACE);
//        }
//    }

    // Footer
    mark(WHYNTER_BIT_MARK);
    space(0);  // Always end with the LED off
}
#endif

//+=============================================================================
#if DECODE_WHYNTER
bool IRrecv::decodeWhynter(decode_results *results) {
    long data = 0;
    int offset = 1;  // skip initial space

    // Check we have the right amount of data
    if (irparams.rawlen < (2 * WHYNTER_BITS) + 6) {
        return false;
    }

    // Sequence begins with a bit mark and a zero space
    if (!MATCH_MARK(results->rawbuf[offset], WHYNTER_BIT_MARK)) {
        return false;
    }
    offset++;

    if (!MATCH_SPACE(results->rawbuf[offset], WHYNTER_ZERO_SPACE)) {
        return false;
    }
    offset++;

    // header mark and space
    if (!MATCH_MARK(results->rawbuf[offset], WHYNTER_HEADER_MARK)) {
        return false;
    }
    offset++;

    if (!MATCH_SPACE(results->rawbuf[offset], WHYNTER_HEADER_SPACE)) {
        return false;
    }
    offset++;

    data = decodePulseDistanceData(results, WHYNTER_BITS, offset, WHYNTER_BIT_MARK, WHYNTER_ONE_SPACE, WHYNTER_ZERO_SPACE);
//    // data bits
//    for (int i = 0; i < WHYNTER_BITS; i++) {
//        if (!MATCH_MARK(results->rawbuf[offset], WHYNTER_BIT_MARK)) {
//            return false;
//        }
//        offset++;
//
//        if (MATCH_SPACE(results->rawbuf[offset], WHYNTER_ONE_SPACE)) {
//            data = (data << 1) | 1;
//        } else if (MATCH_SPACE(results->rawbuf[offset], WHYNTER_ZERO_SPACE)) {
//            data = (data << 1) | 0;
//        } else {
//            return false;
//        }
//        offset++;
//    }

    // trailing mark
    if (!MATCH_MARK(results->rawbuf[offset], WHYNTER_BIT_MARK)) {
        return false;
    }

    // Success
    results->bits = WHYNTER_BITS;
    results->value = data;
    results->decode_type = WHYNTER;
    return true;
}
#endif

