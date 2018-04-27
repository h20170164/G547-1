#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>  /* for sei() */
#include <util/delay.h>     /* for _delay_ms() */

#include <avr/pgmspace.h>   /* required by usbdrv.h */
#include "usbdrv.h"
#include "oddebug.h"        /* This is also an example for using debug macros */
#include "requests.h"       /* The custom request numbers we use */

#define X_channel 0
#define Y_channel 1
#define Z_channel 2

uint8_t x_byte_1,x_byte_2,y_byte_1,y_byte_2,z_byte_1,z_byte_2;
int x_value,y_value,z_value;
static uint8_t    replyBuf[2]; 
/* ------------------------------------------------------------------------- */
/* ----------------------------- USB interface ----------------------------- */
/* ------------------------------------------------------------------------- */

void adc_init(void)
{
	ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);
}

int get_adc_data(unsigned char channel)
{
	int adc_value;
	unsigned char temp;
	ADCSRA |= (1<<ADSC);//start_conversion
	while((ADCSRA&(1<<ADSC)));//wait till end of conversion
	ADMUX=(1<<REFS0)|channel;
	_delay_ms(1);
	temp=ADCL;
	adc_value=ADCH;
	adc_value=(adc_value<<8)|temp;
	return adc_value;
}

usbMsgLen_t usbFunctionSetup(uchar data[8])
{
usbRequest_t    *rq = (void *)data;
int length;

    if(rq->bRequest == 0){  /* return X axis values */
	PORTB^=(1<<1);
	replyBuf[0]=x_byte_1;
	replyBuf[1]=x_byte_2;
	usbMsgPtr = (unsigned char *)replyBuf;	
        return 2;
    }
    else if(rq->bRequest == 1){  /* return Y axis values */
	PORTB^=(1<<2);	
	replyBuf[0]=y_byte_1;
	replyBuf[1]=y_byte_2;
	usbMsgPtr = (unsigned char *)replyBuf;
        return 2;
    }
    else if(rq->bRequest == 2){ /* return Z axis values */
	replyBuf[0]=z_byte_1;
	replyBuf[1]=z_byte_2;
	usbMsgPtr = (unsigned char *)replyBuf;
        return 2;
    }
        
    return 0;   /* default for not implemented requests: return no data back to host */
}

/* ------------------------------------------------------------------------- */

int __attribute__((noreturn)) main(void)
{
uchar   i;
    ADCSRA = (1<<ADEN) | (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0);//init_adc
    DDRB=(1<<1)|(1<<2);
    wdt_enable(WDTO_1S);
    usbInit();
    usbDeviceDisconnect();  /* enforce re-enumeration, do this while interrupts are disabled! */
    i = 0;
    while(--i){             /* fake USB disconnect for > 250 ms */
        wdt_reset();
        _delay_ms(1);
    }
    usbDeviceConnect();
    sei();
    for(;;){                /* main event loop */
        wdt_reset();
        usbPoll();
	x_value=get_adc_data(X_channel);
	x_byte_1=x_value%256;
	x_byte_2=x_value/256;
	y_value=get_adc_data(Y_channel);
	y_byte_1=y_value%256;
	y_byte_2=y_value/256;
	z_value=get_adc_data(Z_channel);
	z_byte_1=z_value%256;
	z_byte_2=z_value/256;
    }
}

/* ------------------------------------------------------------------------- */
