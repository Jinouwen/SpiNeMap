#include "Switch.h"

void Switch::updatePort() {
    if (input_enable & LEFT_MASK) {
        Spike spike;
        spike = left_in.read();
        spike.intra_seg_hop_num++;
        if (output_enable & RIGHT_MASK) {
            right_out.write(spike);
        }

        if (output_enable & MID_MASK) {
            mid_out = spike;
        }
    }

    if (input_enable & MID_MASK) {
        Spike spike;
        spike = mid_in;
        spike.intra_seg_hop_num++;
        if (output_enable & LEFT_MASK) {
            left_out = spike;
        }
        if (output_enable & RIGHT_MASK) {
            right_out = spike;
        }
    }

    if (input_enable & RIGHT_MASK) {
        Spike spike;
        spike = right_in;
        spike.intra_seg_hop_num++;
        if (output_enable & LEFT_MASK) {
            left_out = spike;
        }
        if (output_enable & MID_MASK) {
            mid_out = spike;
        }
    }
}

int Switch::getSwitchId() {
    return id;
}
