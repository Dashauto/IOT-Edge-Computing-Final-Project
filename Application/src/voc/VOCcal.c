#define MAX_VOC_RAW 65535
#define MAX_VOC_INDEX 500

#include "VOCcal.h"

void VocAlgorithm_process2(const uint16_t voc_raw, int *voc_index) {
    double scale_factor = 1.0;  // Define scale factor directly in the function

    // Calculate the VOC index using the fixed scale factor
    *voc_index = (int)(voc_raw * ((double)MAX_VOC_INDEX / MAX_VOC_RAW) * scale_factor);

    // Bounds checking to ensure VOC index remains within expected limits
    if (*voc_index > MAX_VOC_INDEX) {
        *voc_index = MAX_VOC_INDEX;
    } else if (*voc_index < 0) {
        *voc_index = 0;
    }
}
