#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define BUT_PIN 0x10 // 4 бит - кнопка
#define LED_PIN 0x01 //0 бит - диод
#define FIVE_MS 156	// ((8 000 000[частота процессора] / 256[делитель таймера]) / 1000[одна мс]) * 5[пять мс] = 156.25 тактов, за 5 милисекунд пройдет ~156 тактов
#define msDelay 50 // 250/5=50 : прерывание вызывается раз в 5 мс, за 250 мс - 50 раз
#define msDelay2 300 // 750/5=150 
int counter = 0; // счетчик
int led_state = 0; // 1 - диод горит, 0 - диод не горит
int isChecked = 0; //нужно ли зажигать лампочку в след. раз (isChecked =1, если 250 мс уже прошло и светодиод уже включался)

void ledOff(void) 
{
	led_state = 0;
	PORTB |= 0x01; // OR с 00000001 - нулевой разряд становится = 1
}

void ledOn(void)
{
	led_state = 1;
	PORTB &= 0xfe; // AND с 11111110 - нулевой разряд становится = 0
}

void led_reverse (void) //переключение диода
{
	if (led_state == 0)
		ledOn();
	else
		ledOff();
}

int ChekBut (void) 
{ 	
	return !(PINB & BUT_PIN);//проверка нажатия кнопки
}

ISR (TIMER1_COMPA_vect) // прерывание таймера
{
	// если кнопка нажата и еще не прошло 250 мс
    if(ChekBut()) 
	{
		if(isChecked == 0)// если лампочка не горит в данный момент
			{
				counter++;
				if(counter >= msDelay) // если удерживаем кнопку 250 мс, переключаем светодиод.
				{
					led_reverse();
					counter = 0;
				}
			}
			if(led_state == 1) // если лампочка горит
			{
				counter++;
				if(counter >= msDelay2)// если прошло 750 мс, то выключаем лампочку и устанавливаем флаг того, что лампочка уже включалась
				{
					led_reverse();
					isChecked = 1;
				}
			}
			if(led_state == 0) // если лампочка не горит
			{
				counter++;
				if(counter >= msDelay2)// если прошло 750 мс, то выключаем лампочку и устанавливаем флаг того, что лампочка уже включалась
				{
					led_reverse();
					isChecked = 1;
				}
			}
		
	}
	else// если кнопка не нажата
	
	{
		if(led_state) ledOff();
		counter = 0;
		//isChecked = 0;
	}
}

int main ()
{
    DDRB = LED_PIN;
	PORTB = BUT_PIN;
	SREG = 0x80;//разрешение прерываний
    // режим CTC(сброс при совпадении), предделитель равен 256 
    TCCR1B = _BV(WGM12) | _BV(CS12);
	TIMSK  = _BV(OCIE1A);// прерывание по совпадению с OCR1A    
	OCR1A  = FIVE_MS;
	ledOff();// начальное состоянии светодиода - выключен
	while (1) 
	{
    	} 
    return 0;
}