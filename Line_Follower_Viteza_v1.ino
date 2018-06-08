/*
    Robot Line Follower with Arduino

    Autor: Grigoras Alexandru
*/

/* --------------INCEPUT PROGRAM------------- */

#include <QTRSensors.h>
#include <OrangutanMotors.h>

#define NUM_SENSORS       8   /* Numarul de senzori utilizati */

/*
   Cantitatea de citiri analogice pentru senzori
*/
#define TIMEOUT 2500

/* ------------------------------------------ */

/*
   Structura senzorilor
   Senzorii sunt conectati in pinii analogici A0-A7 sau pinii digitali 14-21
*/
QTRSensorsRC qtrrc((unsigned char[]) {
  8, 12, 13, 0, 1, 2, 4, 7
} , NUM_SENSORS, TIMEOUT, QTR_NO_EMITTER_PIN);

OrangutanMotors motors;
/*
   Stocarea valorilor senzorilor
*/
unsigned int sensorValues[NUM_SENSORS];

/*
   Variabile utilizate
*/
unsigned int position = 0;    /* Pozitia curenta a senzorilor */
int derivative = 0;           /* Derivat */
int proportional = 0;         /* Proportional */
int power_difference = 0;     /* Diferenta de viteza */
int max = 70;                /* Viteza maxima */
int last_proportional;        /* Proportionala anterioara */
//float KP = 0.35;              /* Constanta proportionala */
//float KD = 15;                 /* Constanta derivata */
float KP = 0.35;              /* Constanta proportionala */
float KD = 0.8;                 /* Constanta derivata */

/* Contanta pentru intervalul de franare */
#define RANGEBRAKE 2500

/* ------------------------------------------ */

/*
   Functie pentru setari motor si calibrare senzori
*/
void setup()
{
  /* calibrare sensori qtr */
  for ( int i = 0; i < 100; i++)
  {
    qtrrc.calibrate();
  }

  /* Asteptare 5 secunde */
  //delay(5000);
  delay(500);

  /* Miscare robot inainte pentru 0.3 secunde */
  motors.setSpeeds(-90, -90);
  delay(300);
}

/* ------------------------------------------ */

/*
   Bucla principala a programului
*/
void loop()
{
  /*
     Obține poziția liniei
     Nu ne interesează valorile individuale ale fiecărui senzor
  */
  position = qtrrc.readLine(sensorValues);

  /* Termenul proporțional trebuie să fie 0 atunci când suntem pe linie */
  proportional = ((int)position) - 2500;

  /* Dacă se încadrează în intervalul de frânare, se aplica în direcţia curbei */
  if ( proportional <= -RANGEBRAKE )
  {
    motors.setM1Speed(255);
    motors.setM2Speed(0);
    delay(1);
  }
  else if ( proportional >= RANGEBRAKE )
  {
    motors.setM1Speed(0);
    motors.setM2Speed(255);
    delay(1);
  }

  /* Calculare pozitie termen derivat si modificare */
  derivative = proportional - last_proportional;

  /* Memoreaza ultima pozitie */
  last_proportional = proportional;

  /*
     Calculează diferența dintre puterea celor doua motoare [m1 - m2]
     Dacă este un număr pozitiv, robotul se roteşte [dreapta]
     Dacă este un număr pozitiv, robotul se roteşte [stanga]
     Mărimea numărului determină unghiul de rotație
  */
  int power_difference = ( proportional * KP ) + ( derivative * KD );

  /*
     Daca diferentiala viteza este mai mare decât cea posibilă atât pozitiv şi negativ,
     atribuie valoarea maximă permisă
  */
  if ( power_difference > max )
  {
    power_difference = max;
  }
  else if ( power_difference < -max )
  {
    power_difference = -max;
  }

  /* Atribuiți viteză calculată în puterea diferențială a motorului */
  if ( power_difference < 0 )
  {
    motors.setSpeeds(-(max + power_difference), -max);
  }
  else
  {
    motors.setSpeeds(-max, -(max - power_difference));
  }
}

/* --------------SFARSIT PROGRAM------------- */


