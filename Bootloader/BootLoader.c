#include <htc.h>

//#pragma config INTOSCSEL = HIGH, SOSCSEL = DIG, XINST = OFF, RETEN = OFF, FOSC = EC3, PLLCFG = ON, BOREN =OFF, WDTEN = OFF, RTCOSC = INTOSCREF, MCLRE = ON
#pragma config INTOSCSEL = HIGH, SOSCSEL = DIG, XINST = OFF, RETEN = OFF, FOSC = INTIO2, PLLCFG = ON, BOREN =OFF, WDTEN = OFF, RTCOSC = INTOSCREF, MCLRE = ON,  CCP2MX = 0

#define PROG_START 0x1000
#define UI_SWITCH 		((!PORTCbits.RC0) && (!PORTFbits.RF1))

#define LED1_On	TRISBbits.TRISB3=0
#define LED2_On TRISBbits.TRISB4=0
#define LED3_On TRISBbits.TRISB5=0
#define LED4_On TRISAbits.TRISA6=0
#define LED5_On TRISAbits.TRISA7=0

#define LED1_Off TRISBbits.TRISB3=1
#define LED2_Off TRISBbits.TRISB4=1
#define LED3_Off TRISBbits.TRISB5=1
#define LED4_Off TRISAbits.TRISA6=1
#define LED5_Off TRISAbits.TRISA7=1

union LONG {
    unsigned char ub[4];
    signed char b[4];
    unsigned int ui[2];
    signed int i[2];
    unsigned long ul;
    signed long l;
};


void Set_Pins(void);
void Setup_UART(void);
void putc(unsigned char dat);
void Delay_ms(int ms);
unsigned char getc(void);
signed char gethex(void);

#asm
psect intcode	// redirect interrupts to new routine addresses
	goto	PROG_START+8
psect intcodelo // redirect low priority interrupts to new routine addresses
    goto    PROG_START+0x18
#endasm

void main(void)
{
	unsigned char cc=0;
	unsigned char ab=0;
	unsigned char Buffer[128];
	unsigned char inp;
	unsigned char idx;
	signed char temp;
	unsigned char tbuf;
	unsigned char CheckSum;
	union LONG BlockAddress;
	
	unsigned int qqq;
	
	//Interal Oscillator @ 16Mhz	
	OSCCONbits.IRCF=0b111;
	
	//Enable the 4X PLL	
	PLLEN=1;

	Set_Pins();
	Setup_UART();
	
	LED1_On;
	LED2_Off;
	LED3_Off;
	LED4_Off;
	LED5_Off;
	
	Delay_ms(500);	
	
		
	//Check to see if we should run the boot loader...
	//If the switch is not pressed at start up then run the normal program
    if (PORTFbits.RF1 || PORTCbits.RC0)
    {  	   
	   asm("goto " ___mkstr(PROG_START)); 
	} 
	
	LED1_On;
	LED2_On;
	LED3_On;
	LED4_On;
	LED5_On;
		
	INTCONbits.GIE=0;	
	while(1)
	{
WaitForBlock:
		//Wait for a start of block character
		while(1)
		{			
			inp=getc();			
			switch (inp)
			{				
				case '!':					
					goto RecieveBlock;	//Start of block
				case '#':					
					RESET();	//End of program
					break;
				case '?':					
					putc('/');	//Respond to a ping request
					break;
				case 'X':	
					LED1_On;
					LED2_Off;
					LED3_On;
					LED4_Off;
					LED5_On;									
					putc('X');
					break;
				case 'x':
					asm("goto " ___mkstr(PROG_START)); 
					break;
					
				default:		
					putc('%');	//Inform the host of an error and to try again
					break;
			}	
		}	
		
RecieveBlock:
		INTCONbits.GIE=0;		
		CheckSum=0;	
			
		BlockAddress.ul=0;
		//Get the block address [LOWER(7-4), LOWER(3-0), UPPER(7-4), UPPER(3-0)]
		temp=gethex();
		if (temp==-1) goto rxerror;
		BlockAddress.ub[0]=temp<<4;	//LSB upper 4 bits		
		temp=gethex();
		if (temp==-1) goto rxerror;
		BlockAddress.ub[0]+=temp;	//LSB lower 4 bits

		temp=gethex();
		if (temp==-1) goto rxerror;
		BlockAddress.ub[1]=temp<<4;	//LSB upper 4 bits		
		temp=gethex();
		if (temp==-1) goto rxerror;
		BlockAddress.ub[1]+=temp;	//LSB lower 4 bits
		
		BlockAddress.ul<<=7;  //Shift up past the 7-bits of inter-block space
		
		//Get the 128bytes of Data
		for (idx=0;idx<128;idx++)
		{
			temp=gethex();
			if (temp==-1) goto rxerror;
			tbuf=temp;
			tbuf<<=4;
			temp=gethex();
			if (temp==-1) goto rxerror;
			tbuf+=temp;
			Buffer[idx]=tbuf;
			CheckSum+=tbuf;
		}
				
		//Erase the Flash Block		
		//Point to the block to erase
		TBLPTRL=BlockAddress.ub[0];	//Upper two bits = Block Address Bits (1-0)
		TBLPTRH=BlockAddress.ub[1];	//Maps to Block Address Bits (10-2)
		TBLPTRU=BlockAddress.ub[2];	//Maps to Block Address Bits (16-11)
		
		//Setup for the erase operation
		EECON1bits.EEPGD=1;
		EECON1bits.CFGS=0;
		EECON1bits.WREN=1;
		EECON1bits.FREE=1;		
		EECON2=0x55;
		EECON2=0xAA;
		EECON1bits.WR=1;	
		NOP();
		EECON1bits.WREN=0;
		if (EECON1bits.WRERR) goto rxerror;
		
		//Program FLASH 		
		//Point to the block to program
		TBLPTRL=BlockAddress.ub[0];	//Upper two bits = Block Address Bits (1-0)
		TBLPTRH=BlockAddress.ub[1];	//Maps to Block Address Bits (10-2)
		TBLPTRU=BlockAddress.ub[2];	//Maps to Block Address Bits (16-11)
		
		//Write the 128 bytes of data into the FLASH Registers
		//The first write doesn't increment the table pointer
		TABLAT=Buffer[0];
		asm("TBLWT*");		
		for (idx=1;idx<128;idx++)
		{
			TABLAT=Buffer[idx];
			asm("TBLWT+*");	//Increment the Table Pointer then write
		}
		
		//Actually write the FLASH
		EECON1bits.EEPGD=1;
		EECON1bits.CFGS=0;
		EECON1bits.WREN=1;	
		EECON1bits.FREE=0;	
		EECON2=0x55;
		EECON2=0xAA;
		EECON1bits.WR=1;		
		NOP();
		EECON1bits.WREN=0;		
		if (EECON1bits.WRERR) goto rxerror;
		
		//Tell the host that the last block was good and 
		//we are ready for the next block...
		cc++;
		if (cc==0) {
			LED1_Off;
			LED2_Off;
			LED3_Off;
			LED4_Off;
			LED5_Off;
		} else if (cc==1) {
			LED1_On;
		} else if (cc==2) {
			LED2_On;
		} else if (cc==3) {
			LED3_On;
		} else if (cc==4) {
			LED4_On;
		} else {
			LED5_On;
			cc=0;
		} 
								
		putc('*');
		
		goto WaitForBlock;
rxerror:
		putc('%');
		goto WaitForBlock;		
	}	
	

}


void putc(unsigned char dat)
{
	while (TXSTA1bits.TRMT==0) NOP();	
	TXREG=dat;	
}	

unsigned char getc(void)
{
	unsigned char temp=0;
	while(!RC1IF)
	{
		
		if ((RCSTA1bits.OERR) | (RCSTA1bits.FERR))
		{
			RCSTA1bits.CREN=0;
			NOP();
			RCSTA1bits.CREN=1;
			temp=RCREG;
			temp=RCREG;
			return(0);	
		}	
	}	 
	temp=RCREG;
	return (temp);
}	

signed char gethex(void)
{
	unsigned char temp;
	temp=getc();
	if (temp==0) return(-1);
	if (temp<'0') return(-1);
	if (temp>'@')
	{
		temp-='A';
		temp+=10;
		if (temp>15) return(-1);
		return(temp);
	}	
	temp-='0';
	if (temp>9) return(-1);
	return(temp);
}		
		
	
void Set_Pins()
{
     //PORTA:
    //RA0 - N/C (Output)
    //RA1 - N/C (Output)
    //RA2 - N/C (Output)
    //RA3 - N/C (Output)
    //RA4 - ServoX (Output)
    //RA5 - Current Sense (Analog Input)
    //RA6 - LED#4 (Input) - Open Collector Pull Low to turn on
    //RA7 - LED#5 (Input) - Open Collector Pull Low to turn on
    LATA  = 0b00000000;
    TRISA = 0b11100000;


    //PORTB
    //RB0 - Encoder Clock (Input)
    //RB1 - Encoder Up/Down (Input)
    //RB2 - Encoder Mode (Output)
    //RB3 - LED#1
    //RB4 - LED#2
    //RB5 - LED#3
    //RB6 - Program Clock (PGC) (Output)
    //RB7 - Pogram Data (PGD) (Output)
    LATB  = 0b00000000;
    TRISB = 0b00111011;
    RBPU = 1; //Enable Pull Ups


    //PORTC
    //RC0 - Top Switch (Input)
    //RC1 - ServoX (Output)
    //RC2 - N/C (Output)
    //RC3 - N/C (Output)
    //RC4 - N/C (Output)
    //RC5 - N/C (Output)
    //RC6 - MCU Serial TX (Output)
    //RC7 - MCU Serial RX (Input)
    LATC  = 0b00000000;
    TRISC = 0b10000001;

    //PORTD
    //RD0 - N/C (Output)
    //RD1 - N/C (Output)
    //RD2 - N/C (Output)
    //RD3 - N/C (Output)
    //RD4 - N/C (Output)
    //RD5 - N/C (Output)
    //RD6 - Motor Diag B (Input)
    //RD7 - Motor CCW Direction (Output)
    LATD =  0b00000000;
    TRISD = 0b01000000;

    //PORTE
    //RE0 - N/C (Output)
    //RE1 - N/C (Output)
    //RE2 - N/C (Output)
    //RE3 - N/C (Output)
    //RE4 - Motor CW Direction (Output)
    //RE5 - Motor Diag A (Input)
    //RE6 - N/C (Output)
    //RE7 - Motor PWN (Output)
    LATE =  0b00000000;
    TRISE = 0b00100000;

    //PORTF
    //RF0 - NO PIN (Output)
    //RF1 - Middle Switch (Input)
    //RF2 - N/C (Output)
    //RF3 - N/C (Output)
    //RF4 - Focus (Output)
    //RF5 - Shutter (Output)
    //RF6 - Battery Voltage (Analog Input)
    //RF7 - Bottom Switch (Input)
    LATF = 0b00000000;
    TRISF = 0b11000010;

    //PORTG
    //RG0 - N/C (Output)
    //RG1 - N/C (Output)
    //RG2 - N/C (Output)
    //RG3 - N/C (Output)
    //RG4 - N/C (Output)
    //RG5 - No PIN (Output)
    //RG6 - No PIN (Output)
    //RG7 - No PIN (Output)
    LATG = 0b00000000;
    TRISG = 0b00000000;

    //Setup ADC Pins
    //All Others --- DISABLE ---
    ANCON0 = 0;
    ANCON1 = 0;
    ANCON2 = 0;
    
    //Turn off the ADC
    ADON = 0;
}

void Setup_UART(void)
{
    //Setup Pins
    TRISCbits.TRISC7 = 1;
    TRISCbits.TRISC6 = 0;
    
    //Setup USART
    TXSTA1bits.BRGH = 1; //High Bit Rate Mode
    BAUDCON1bits.BRG16 = 1; //16-bit Baud Rate Generator Only
    BAUDCON1bits.RXDTP = 0; //RX signal is NOT inverted (Active-high)
    BAUDCON1bits.TXCKP = 0; //TX Signal is NOT inverted (Active-high)
    BAUDCON1bits.WUE = 0; //Wake Up is NOT enabled
    BAUDCON1bits.ABDEN = 0; //Auto baud rate is NOT enabled
    

    //Fosc=64Mhz
    //Baud=115200bps
    //BRGH=1, 16-Bit
    //Baud=Fosc/[4/(BRG+1)]
    //BRG=137 (115,942bps @ 0.64% Error)
    SPBRGH1 = 0;
    SPBRG1 = 137;

    TXSTA1bits.SYNC = 0; //Async Mode
    RCSTA1bits.SPEN = 1; //Serial Port Enabled

    TX1IE = 0; //Disable Interrupts
    TXSTA1bits.TX9 = 0; //8-bit Mode
    TXSTA1bits.TXEN = 1; //TX Enabled

    RC1IE = 0; //Disable Interrupts
    RCSTA1bits.RC9 = 0; //8-bit Mode
    RCSTA1bits.CREN = 1; //Async Mode Enable Reciever
}

void Delay_ms(int ms)
{
	int idx;
	int temp;
	
	for (idx=0;idx<ms;idx++)
	{
		//Delay for 1 ms
		for (temp=0;temp<1454;temp++) NOP();
	}		
	return;	
}	
	


