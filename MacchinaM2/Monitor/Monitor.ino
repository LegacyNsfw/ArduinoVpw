#include "M2_J1850-VPW.h"

//Create an instance of the class
j1850 j;

void setup() {
    //Initialize. RX pin, TX pin, monitoring ON
    j.init(J1850_VPW_RX, J1850P_TX, true, 2100);
    SerialUSB.begin(115200);
}

// the loop function runs over and over again forever
void loop() {

    byte rx_buf[j.get_rx_buf_size()];      //Buffer array for storing received messages, up to 12 bytes in length (3 header, max 8 payload, 1 CRC)
    int lastMessage = 0;

    if (j.accept(rx_buf)) {
        // Watch bus for traffic, receive a message.
        // Rx buffer printed here because monitoring is set to ON
        // 
        // if a message is received, immediately send back out a response of "Hi-M2!"
        //  "easy send" does all formatting automatically and calculates CRC.// 
        // j.easy_send(6, 0x48, 0x69, 0x2D, 0x4D, 0x32, 0x21);

      // j.easy_send(msg length, ww, xx, yy, zz, zz, zz, zz, zz, zz, zz, zz)
      // msg length = total length of message that you want to send, including 3-byte header
      // ww = header byte 1, message priority
      // xx = header byte 2, destination node that message is intended for
      // yy = header byte 3, source node that message is being sent from....usually set this to 0xF1 for "offboard scan tool"
      // zz = message payload, maximum 8 bytes.

      //  Tx buffer (message that was sent above) is also re-printed here because monitoring is set to ON
    }

/*
if (j.message != lastMessage)
    {
        SerialUSB.println(j.message, HEX);
        lastMessage = j.message;
    }
*/
    //       j.send(rx_buf, j.rx_nbyte);
    //     Sending a static message FF, FF, FF, FF, FF, CRC sum        J.send is used to send a direct J1850 message "as is"

}
