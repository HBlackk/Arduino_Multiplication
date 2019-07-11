#include <Arduino.h>
int n;
word BR;
word AD;
byte A;
byte D;
byte B;
byte R;
int a;

int i=0;
void setup()
{
    Serial.begin(9600);
}

int readInt(void)
{    
    char num[6];
    int incomingByte;    
    if(Serial.available() > 0)
    {        
        incomingByte = Serial.readBytesUntil('\n', num, 5);
        num[incomingByte] = '\0';
        //Serial.print("Input an integer in range -32768 to 32767: ");
        n = (atoi(num)); 
        return n;       
    }    
}

String decimalToBinary(String message, int n)
{
    
    Serial.print(message);
    String lead;
    if(n == 0)
    {
        lead = "00000000";
    }
    if(n >= 1)
    {
        lead = "0000000";
    }
    if(n >= 2)
    {
        lead = "000000";
    }
    if(n >= 4)
    {
        lead = "00000";
    }
    if(n >= 8)
    {
        lead = "0000";
    }
    if(n >= 16)
    {
        lead = "000";
    }
    if(n >= 32)
    {
        lead = "00";
    }
    if(n >= 64)
    {
        lead = "0";
    }
    if(n >= 128)
    {
        lead = "";
    }    
    Serial.print(lead);
    Serial.print(n, BIN);
    Serial.print(" ");
    //Serial.print(n);
    Serial.println();
}

void trace(word BR, word AD, byte n)
{   
    //Serial.println(n);
    //decimalToBinary("BIN=", BR);
    //decimalToBinary("BIN=", AD);
    
    B = BR >> 8;
    A = AD >> 8;
    R = BR & 0xFF;
    D = AD & 0xFF;    

    Serial.println();
    Serial.print("n=");
    Serial.print(n);
    Serial.print(" D="); 
    decimalToBinary("", D);    
    Serial.println(" B=");
    decimalToBinary("", B); 
    Serial.println(" A=");
    decimalToBinary("", A); 
    Serial.println(" R=");
    decimalToBinary("", R);      
}

void loop()
{ 
  while(Serial.available()>0)
  {       
      i++;
      a = readInt(); 
       if(i==1)
       {        
          Serial.print("Enter BR in range -32768 to 32767: ");
          BR = a;               
          Serial.print(BR);
          Serial.println();                    
       }
       if(i==2)
       {        
          Serial.print("Enter AD in range -32768 to 32767: ");        
          AD = a;        
          Serial.print(AD);        
          Serial.println();                                    
       }  
       if(i==3)
       {
          Serial.print("Input n in range 1 to 8:           ");
          Serial.print(n);
          n=a;
          trace(BR, AD, n);
          i=0;          
       }          
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Program 4 - Signed Multiplication using 4 registers with correction factor
// 
// Input parameters are automatically passed in the following registers:
//    r24 - multiplier          alias (R)
//    r22 - multiplicand        alias (D)
//
// IMPORTANT - use the following machine registers for your other varialbles:
//    r25 - Product high byte   alias (B)
//    r23 - Product low  byte   alias (A)
//    r20 - Interation counter  alias (n)
// 
// The contents of registers r25 and r24 are returned from the function
// You must ensure that you copy the product to these if necessary i.e. B->r25 A->r24
//
// to find out more about C function parameter passing, see https://gcc.gnu.org/wiki/avr-gcc#Register_Layout
//  
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int signedMult(char multiplier, char multiplicand)
{
    asm volatile(
          // your assembly code goes here
 "         lds r20, n;  n is assigned to register 20   \n"
 "         lds r22, D;  D is assigned to register 22   \n"
 "         lds r23, A;  A is assigned to register 23   \n"
 "         lds r24, R;  R is assigned to register 24   \n"  
 "         lds r25, B;  B is assigned to register 25   \n" 
 

 "         loop%=: clc; this clears the carry flag     \n"
 "         sbrc r24,0;                                 \n"
 "         call add;                                   \n"
 "         call shift;                                 \n"
 "         dec r20;                                    \n"
 "         brne loop%=;                                \n" 

 "         sts B,r25;                                  \n"
 "         sts A,r24;                                  \n"   
 :
 :
 : "r20","r22","r23","r24","r25","r26"
    );
 }

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Global inline assembly language subroutines
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

asm volatile(
//-----------------------------------------------------------------------
// Wrapper for the C trace function, which saves and restores
// registers and flags, so we can basically do anything we want in it ;)
//-----------------------------------------------------------------------
"dumpRegs:   push r20           ; save the loop count n               \n"
"            push r22           ; save the multiplicand, Yreg         \n"
"            push r23           ; save the loop count, Nreg           \n"
"            push r24           ; save the product lo byte Pl         \n"
"            push r25           ; save the product hi byte Ph         \n"  
"            in r16, __SREG__   ; save the status register SREG       \n"
"            call TRACE         ; call the C function trace()         \n"
"            out __SREG__, r16  ; restore the status register, SREG   \n"
"            pop r25            ; restore working registers....       \n"
"            pop r24            ; note that these must be popped off  \n"
"            pop r23            ; the stack in reverse order to that  \n"
"            pop r22            ; in which they were pushed           \n"
"            pop r20            ; in which they were pushed           \n"
"            ret                ; return back to multiplication       \n"     

// put your add and shift subroutines here
"add:       r25, r22             ; adds the values together            \n"
"           call dumpRegs        ; calls the dumpRegs function         \n"
"           ret                  ; returns back                        \n"


"shift:     ror r25             ; shifts register 25                  \n"
"           ror r24             ; shifts register 24                  \n" 
"           call dumpRegs       ; calls the dumpRegs function         \n"
"           ret                 ; returns back                        \n"
);
