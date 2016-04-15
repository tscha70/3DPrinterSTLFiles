//Includes
#include <avr/io.h>
#include <avr/interrupt.h>

#define INTERRUPTPIN PCINT1 //this is PB1 per the schematic
#define PCINT_VECTOR PCINT0_vect  //this step is not necessary
#define DATADIRECTIONPIN DDB1 //Page 64 of data sheet
#define PORTPIN PB1 //Page 64
#define READPIN PINB1 //page 64
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit)) //OR
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit)) //AND

/*
   Alias for the ISR: "PCINT_VECTOR" (Note: There is only one PCINT ISR.
   PCINT0 in the name for the ISR was confusing to me at first,
   hence the Alias, but it's how the datasheet refers to it)
*/

int outDir = 0;
int outEnable = 4;
const byte analogInPin = A1;

// mapped feedback from potentiometer (0 - 180)
int feedbackValue = 0;

// mapped PWM-Signal (0 - 180)
int pwmSignalDecoded = 0;
int cachedPwmSignalDecoded = 0;
int currentPwmSignal = 0;
unsigned long startMicro = 0;
unsigned long deltaMicro = 0;



void setup() {
  cli();//disable interrupts during setup
  // direction of motor
  pinMode(outDir, OUTPUT);
  // enable motor
  pinMode(outEnable, OUTPUT);
  digitalWrite(outDir, LOW);
  digitalWrite(outEnable, LOW);

  PCMSK |= (1 << INTERRUPTPIN); //sbi(PCMSK,INTERRUPTPIN) also works but I think this is more clear // tell pin change mask to listen to pin2 /pb3 //SBI
  GIMSK |= (1 << PCIE);   // enable PCINT interrupt in the general interrupt mask //SBI
  DDRB &= ~(1 << DATADIRECTIONPIN); //cbi(DDRB, DATADIRECTIONPIN);//  set up as input  - pin2 clear bit  - set to zero
  PORTB |= (1 << PORTPIN); //cbi(PORTB, PORTPIN);// disable pull-up. hook up pulldown resistor. - set to zero
  sei(); //last line of setup - enable interrupts after setup
}

void loop() {

  // feedback from potentiometer (glider-resistor)
  feedbackValue = map (analogRead(analogInPin), 0, 1023, 0, 180);

  if ( IsPWMSignalSameAsFeedback(currentPwmSignal, feedbackValue))
  {
    // disable motor - keep position
    digitalWrite(outEnable, LOW);
  }
  else
  {
    // try to get to the position
    digitalWrite(outEnable, HIGH);
    if (feedbackValue <= currentPwmSignal)
    {
      digitalWrite(outDir, LOW);
    }
    else
    {
      digitalWrite(outDir, HIGH);
    }
  }

  delay(4);
}

bool IsPWMSignalSameAsFeedback(int pwm, int feedback)
{
  const int tolerance = 1;
  int delta = abs(feedback - pwm);

  if (delta <= tolerance)
  {
    return 1;
  }

  return 0;
}



// this is the interrupt handler - thank's to GisMofx
// http://arduino.stackexchange.com/questions/3929/attiny85-interrupt-id-vs-pin-when-programming-with-arduino
ISR(PCINT_VECTOR)
{
  //PCINTn triggers on both rising and falling edge
  //i.e. pin goes to 5V or falls to 0V
  byte pinState;
  pinState = (PINB >> READPIN) & 1; //PINB is the register to read the state of the pins
  if (pinState > 0) //look at the pin state on the pin PINB register- returns 1 if high
  {
    // rising edge - peak start _|
    startMicro = micros();
  }
  else
  {
    // ¬ peak ends
    // falling edge of interrupt - calculate delta-time in microseconds as deltaMicro
    deltaMicro = micros() - startMicro;
    cachedPwmSignalDecoded = pwmSignalDecoded;
    
    deltaMicro = constrain(deltaMicro, 1000, 2000);
    pwmSignalDecoded = map(deltaMicro, 1000, 2000, 0, 180);

    // filter spikes and ignore them
    if (abs(cachedPwmSignalDecoded - pwmSignalDecoded) < 25)
    {
      currentPwmSignal = pwmSignalDecoded;
    }
  }
}
