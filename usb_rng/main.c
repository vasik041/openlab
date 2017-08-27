#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

#include "usbdrv.h"
#include <util/delay.h>
#include "rng.h"


#define USB_LED_OFF 0
#define USB_LED_ON  1
#define USB_DATA_OUT 2
#define USB_DATA_WRITE 3
#define USB_DATA_IN 4

uchar replyBuf[16+1] = "";
uchar dataReceived = 0, dataLength = 0; // for USB_DATA_IN
uchar clear = 0;


// this gets called when custom control message is received
USB_PUBLIC uchar usbFunctionSetup(uchar data[8])
{
    usbRequest_t *rq = (void *)data; // cast data to correct type
        
    switch(rq->bRequest) { // custom command is in the bRequest field
    case USB_LED_ON:
        PORTB |= 1; // turn LED on
        return 0;
    case USB_LED_OFF: 
        PORTB &= ~1; // turn LED off
        return 0;

    case USB_DATA_OUT: // send data to PC
	clear = 1;
        usbMsgPtr = replyBuf;
        return sizeof(replyBuf);

    case USB_DATA_WRITE: // modify reply buffer
	replyBuf[7] = rq->wValue.bytes[0];
	replyBuf[8] = rq->wValue.bytes[1];
	replyBuf[9] = rq->wIndex.bytes[0];
	replyBuf[10] = rq->wIndex.bytes[1];
	return 0;

    case USB_DATA_IN: // receive data from PC
	dataLength  = (uchar)rq->wLength.word;
        dataReceived = 0;
	if(dataLength  > sizeof(replyBuf)) // limit to buffer size
	    dataLength  = sizeof(replyBuf);
	return USB_NO_MSG; // usbFunctionWrite will be called now
    }

    return 0; // should not get here
}


// This gets called when data is sent from PC to the device
USB_PUBLIC uchar usbFunctionWrite(uchar *data, uchar len)
{
    uchar i;
	    
    for(i = 0; dataReceived < dataLength && i < len; i++, dataReceived++)
	replyBuf[dataReceived] = data[i];
	
    return (dataReceived == dataLength); // 1 if we received it all, 0 if not
}


int main()
{
uchar i;
uint16_t b;

    DDRB = 1; // PB0 as output
    wdt_enable(WDTO_1S); // enable 1s watchdog timer

    usbInit();
    initRnd();
        
    usbDeviceDisconnect(); // enforce re-enumeration
    for(i=0; i < 250; i++) { // wait 500 ms
        wdt_reset(); // keep the watchdog happy
        _delay_ms(2);
    }
    usbDeviceConnect();
        
    sei(); // Enable interrupts after re-enumeration
        
    while(1) {
        wdt_reset(); // keep the watchdog happy
        usbPoll();

//	PORTB = PINC & 1;

	if(clear) {
	    replyBuf[0] = 0;
	    clear = 0;
	    PORTB = PINB ^ 1;
	}

	b = getRnd();
	if(b != 0x100) {
	    uint8_t len = replyBuf[0];
	    if(len < sizeof(replyBuf)-1) {
		replyBuf[len+1] = b;
		replyBuf[0]++;
	    }
	}
    }
        
    return 0;
}

