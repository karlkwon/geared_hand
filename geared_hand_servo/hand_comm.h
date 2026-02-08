/*
 * Geared Hand Communication Protocol
 *
 * Copyright (c) 2025 Karl Kwon
 * All rights reserved.
 *
 * Custom binary protocol for servo command communication over serial.
 *
 * Author: Karl Kwon (mrthinks@gmail.com)
 */

//  FR + 0 + Angle + 1 + Angle + 2 + Angle + 3 + Angle + 4 + Angle
//  FR00001000200030004000, FR00901091209230934094, FR01801180218031804180, FR01601160216031604160
//  F + ID + Angle
//  F1090

enum PACKET_STATE {WAIT_PACKET, WAIT_TYPE, WAIT_ID, WAIT_DEG};
enum PACKET_TYPE {PACKET_SINGLE=0, PACKET_ALL=1};
const int PACKET_LEN[2] = {5, 22};
int degs[5] = {0, 0, 0, 0, 0};
int degs_past[5] = {0, 0, 0, 0, 0};

int mortor_id = 0;
int packet_rx_cnt = 0;
int deg_rx_cnt = 0;
PACKET_TYPE packet_type = PACKET_SINGLE;
PACKET_STATE packet_state = WAIT_PACKET;

typedef void (*PacketRx)(PACKET_TYPE type, int id);
PacketRx packet_rx_callback = nullptr;

void packet_mgr_reset() {
    packet_state = WAIT_PACKET;
    deg_rx_cnt = 0;
}

void packet_mgr_init(PacketRx cb) {
    packet_rx_callback = cb;
    packet_mgr_reset();
}

int packet_mgr_parse(unsigned char c) {
    switch(packet_state) {
        case WAIT_PACKET:
            if(c == 'F') {
                packet_state = WAIT_TYPE;
                packet_rx_cnt = 1;
            }
            break;

        case WAIT_TYPE:
            if(c == 'R' || c == 'L') {
                packet_rx_cnt++;
                packet_type = PACKET_ALL;
                packet_state = WAIT_ID;
            } else if(c >= '0' && c <= '4') {
                mortor_id = c - '0';
                degs[mortor_id] = 0;
                deg_rx_cnt = 0;
                packet_rx_cnt++;
                packet_type = PACKET_SINGLE;
                packet_state = WAIT_DEG;
            } else {
                packet_rx_cnt = 0;
                packet_state = WAIT_PACKET;
            }
            break;

        case WAIT_ID:
            if(c >= '0' && c <= '5') {
                mortor_id = c - '0';
                degs[mortor_id] = 0;
                deg_rx_cnt = 0;
                packet_rx_cnt++;
                packet_state = WAIT_DEG;
            } else {
                packet_rx_cnt = 0;
                packet_state = WAIT_PACKET;
            }
            break;

        case WAIT_DEG:
            if(c >= '0' && c <= '9') {
                degs[mortor_id] = degs[mortor_id] * 10 + (c - '0');
                deg_rx_cnt++;
                packet_rx_cnt++;
            } else {
                packet_rx_cnt = 0;
                packet_state = WAIT_PACKET;
                break;
            }

            if(deg_rx_cnt < 3) {
                break;
            }

            if(packet_rx_cnt >= PACKET_LEN[packet_type]) {
                packet_state = WAIT_PACKET;
                //  receive success
                if(packet_rx_callback != nullptr) {
                    packet_rx_callback(packet_type, mortor_id);
                }
            } else {
                packet_state = WAIT_ID;
            }
            break;
    }
    return packet_state;
}
