#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED_BIT PD7
#define LED_HIGH PORTB |= _BV(LED_BIT)
#define LED_LOW PORTB &= ~(_BV(LED_BIT))


int main(void) {
	uint8_t led_state = 1;
	DDRB |= (1 << LED_BIT);
	int i=0;
	int j=0;
	
	_delay_ms(500);
	
	for(j=0; j<8; j++){
	
		_delay_ms(1000);
		
	}

	LED_HIGH;
	
	for(i=0; i<40; i++){
	
		for(j=0; j<60; j++){
	
		_delay_ms(1000);
		
		}
		
	}
	LED_LOW;
}