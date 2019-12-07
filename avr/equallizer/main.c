/*
* equallizer.c
*
* Created: 2019-11-06 오후 7:49:16
* Author : klop8
*/

#include <avr/io.h>
#include "avr/iom128.h"
#include "main.h"
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
//printf함수 사용을을위해 UART통신을 설정
static int USART0_putchar(char c, FILE *stream);
static FILE device = FDEV_SETUP_STREAM(USART0_putchar, NULL, _FDEV_SETUP_WRITE);
*/

int count,i,j;		//loop용 변수
int rs = 16;
int brightness = 1;
int sensitivity = 1;
int position = 0;
int mode = 0,state=0; //default: mode=0, state=0|mode:1=setting|state:조이스틱이벤트 감지(상하인지 좌우인지)
int min=0,max=120;

/*
//printf함수 사용을을위해 UART통신을 설정

void  uart0_init(void){
unsigned int baud=1;   // 초기 baud값을 "1"로 설정한다.
baud = ((F_CPU+(BAUD_RATE*8L))/(BAUD_RATE*16L)-1);            //baud를 프로그램으로 계산 한다.

UBRR0H=(unsigned char) (baud >>8);                                                       // baud 계산된 값으로  H레지스터 설정
UBRR0L=(unsigned char) (baud & 0xFF);                                                  // baud 계산된 값으로  L레지스트  설정

UCSR0A = 0X00;                                                                                       // Asynchronous normal mode로 설정한다
// UCSR0A : RXC0,TXC0,UDRE0,FE0,DOR0,UPE0,U2X0,MPCM0;

UCSR0B = 0X18;                                                                                       // Rx/Tx enable, 8bit Data 로 설정한다.
// UCSR0B : RXCIE0,TXCIE0,UDRIE0,RXEN0,TXEN0,UCSZ02,RXB80,TXB80

UCSR0C = 0X06;                                                                                       // 1 Stop bit, 8bit Data, No Parity 로 설정한다.
//UCSR0C : - , UMSEL0,UPM01,UPM00,USBS0,UCSZ01,UCSZ00,UCPOL0
}

static int USART0_putchar(char c, FILE *stream){
if(c == '\n'){
loop_until_bit_is_set(UCSR0A,UDRE0);
UDR0 = 0x0D;
loop_until_bit_is_set(UCSR0A,UDRE0);
UDR0 = 0x0A;
}else{
loop_until_bit_is_set(UCSR0A,UDRE0);
UDR0 = c;
}
return 0;

}
*/
int map(int x, int in_min, int in_max, int out_min, int out_max)
{
	return (x-in_min)*(out_max-out_min)/(in_max-in_min)+out_min;
}

void Initial(void){ // ------ 초기화 함수 ---------------------------

	PORTF= 0x00 ;
	DDRF = 0x00 ; 		// A/D1(PF.1) <-- 마이크
	PORTE= 0xff ;
	DDRE = 0xfe ;		//시리얼
	PORTD= 0xff ;		// PD.0 : INT0
	DDRD = 0x00 ;

	_delay_ms(10);

	ADCSRA = 0x86;		// (1000 0110) ADC enable, 128분주(125kHz), resolution:10bit
	//ADMUX = 0x41;		// 기준전압 = AVCC(+5V), select 채널 ADC1
	_delay_us(150);
	
}

int Adc(void){	 // ========== >>> AD 변환 함수 <<< ======
	ADCSRA = 0xD5;	        // A/D 변환시작 및 clear ADIF
	while( (ADCSRA & 0x10) == 0 );  // 변환 완료까지 대기
	return (int)ADCL + ((int)ADCH << 8);
}
void EqDisp(int *data){
	for(i=0;i<256;i++){
		led[i].r=0;
		led[i].g=0;
		led[i].b=0;
	}
	for(i=0;i<8;i++){
		for(j=0;j<data[position+i];j++){
			
			led[i*32+j].r=color[j/2][0]==0?0:(color[j/2][0]*brightness-1);
			led[i*32+j].g=color[j/2][1]==0?0:(color[j/2][1]*brightness-1);
			led[i*32+j].b=color[j/2][2]==0?0:(color[j/2][2]*brightness-1);
			
			led[(i+1)*32-1-j].r=color[j/2][0]==0?0:(color[j/2][0]*brightness-1);
			led[(i+1)*32-1-j].g=color[j/2][1]==0?0:(color[j/2][1]*brightness-1);
			led[(i+1)*32-1-j].b=color[j/2][2]==0?0:(color[j/2][2]*brightness-1);
		}
	}
	ws2812_setleds(led,LED_PIXEL);
}
void Display(int num){
	for(i=0;i<256;i++){
		led[i].r=0;
		led[i].g=0;
		led[i].b=0;
	}
	int n=num<17?num-1:16;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			int temp;
			temp=(screenData[n][i]&bit[j])>>j;
			if(temp==1){
				if(j%2==0){
					led[240-16*j+i].r=brightness*16-1;
					led[240-16*j+i].g=brightness*16-1;
					led[240-16*j+i].b=brightness*16-1;
					}else{
					led[255-16*j-i].r=brightness*16-1;
					led[255-16*j-i].g=brightness*16-1;
					led[255-16*j-i].b=brightness*16-1;
					
				}
			}
		}
	}
	ws2812_setleds(led,LED_PIXEL);
}
void Demo(){
	int demoState=1;
	int k;
	while(demoState){
		for(i=0;i<9;i++){
			for(j=0;j<16;j++){
				for(k=0;k<16;k++){
					if(PIND&0x01){
						_delay_ms(1);
						if(PIND&0x01){
							mode = 0;
							i=9;
							j=256;
							demoState=0;
							_delay_ms(100);
							
						}
					}
					if(k%2==0){
						led[240-16*k+j].r=demo[i][j*16+k][0]/8;
						led[240-16*k+j].g=demo[i][j*16+k][1]/8;
						led[240-16*k+j].b=demo[i][j*16+k][2]/8;
						}else{
						led[255-16*k-j].r=demo[i][j*16+k][0]/8;
						led[255-16*k-j].g=demo[i][j*16+k][1]/8;
						led[255-16*k-j].b=demo[i][j*16+k][2]/8;
						
					}
				}
			}
			ws2812_setleds(led,256);
			_delay_ms(1000);
		}
	}
}
void ButtonEvent(){
	if(PIND&0x01){
		_delay_ms(1);
		if(PIND&0x01){
			mode =  mode==1 ? 0 : 1;
			_delay_ms(200);
		}
	}
}

void JoystickEvent(){
	int temp;
	ADMUX = JOYSTIC_UD_PIN;
	temp=Adc();
	if(mode){
		if(temp<JOYSTICK_MIN&&brightness<16&&mode){
			brightness++;
			state=0;
			_delay_ms(100);
			}else if(temp>JOYSTICK_MAX&&brightness>1&&mode){
			brightness--;
			state=0;
			_delay_ms(100);
		}
		}else{
		if(temp<JOYSTICK_MIN&&sensitivity<11){
			sensitivity++;
			_delay_ms(100);
			}else if(temp>JOYSTICK_MAX&&sensitivity>1){
			sensitivity--;
			_delay_ms(100);
		}
	}
	ADMUX = JOYSTIC_LR_PIN;
	temp=Adc();
	if(mode){
		if(temp<JOYSTICK_MIN && rs<64){
			rs*=2;
			state=1;
			_delay_ms(100);
			}else if(temp>JOYSTICK_MAX&&rs>16){
			rs/=2;
			state=1;
			_delay_ms(100);
		}
		}else{
		if(temp<JOYSTICK_MIN&&position<(rs/2-8)){
			position++;
			_delay_ms(50);
			}else if(temp>JOYSTICK_MAX&&position>0){
			position--;
			_delay_ms(100);
		}
	}

}

void main(void){ // ========== >>> 메인 함수 <<< ========

	Initial();				 // 포트 및 ADC초기화
	/*
	//printf함수 사용을을위해 UART통신을 설정
	uart0_init();            // uart0를 초기화하고 설정한다.
	stdout = &device;        //  device 를 stdout으로 설정한다
	*/
	_delay_ms(10);
	Demo();
	while(1){
		ButtonEvent();
		JoystickEvent();
		if(!mode){
			int result[rs];
			//ADMUX = POTENTIOMETER;
			//max=Adc();
			ADMUX = MIC_PIN;
			for(count=0;count<FFT_N;count++){
				capture[count] = Adc()-512; 		// AD 변환값 추가
				_delay_us(SEC_PER_SAMPLE);
			}//end for
			
			// Compute the FFT
			fft_input(capture, bfly_buff);   // Samples -> complex #s
			fft_execute(bfly_buff);          // Process complex data
			fft_output(bfly_buff, spectrum); // Complex -> spectrum
			
			// Print the results
			spectrum[0]=0;spectrum[1]=0;
			
			for(i=0;i<rs;i++){
				int temp=0;
				for(j=0;j<FFT_N/rs;j++){
					temp += spectrum[FFT_N/rs*i+j];
				}
				temp=map(temp,0,sensitivity*100,0,16);
				result[i]=temp<17?temp:16;
			}
			EqDisp(result);
			}else{
			if(!state){
				Display(brightness);
				}else{
				Display(rs/2);
			}
		}
		_delay_ms(33);
	}
}
