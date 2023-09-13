#include <stdio.h>
#include <string.h>
#include "irslinger.h"
#include "electraAcRemoteEncoder.h"

int main (int argc,char *argv[])
{
    int i = 0,acState,acMode,acSwing;
    struct airCon newAc;
    uint32_t outPin = atoi(argv[1]); //Pin number according to BCM
    int frequency = 38000;
    double dutyCycle = 0.5;
    int *codes = NULL;

    if (argc!=7){
        printf("Invalid arguments\n");
        printf("usage: Pin number according to BCM (For example: 17), Fan Strength [1-4] (4=auto), Mode [COOL/HEAT], Temp [16-30], AC State [ON/OFF], Swing [SWING_ON/SWING_OFF/SWING_SINGLE]\n(AC state should be ON if the AC is already running)\n");
        printf("\nFor example: ./electraAcRemote 1 COOL 25 ON SWING_OFF\n");
        return 1;
    }
    if(!strcmp(argv[3],"COOL")){
        acMode = COOL;
    }
    if(!strcmp(argv[3],"HEAT")){
        acMode = HEAT;
    }

    if(!strcmp(argv[5],"ON")){
        acState = ON;
    }
    if(!strcmp(argv[5],"OFF")){
        acState = OFF;
    }

    if(!strcmp(argv[6],"SWING_ON")){
        acSwing = SWING_ON;
    }
    if(!strcmp(argv[6],"SWING_SINGLE")){
        acSwing = SWING_SINGLE;
    }
    if(!strcmp(argv[6],"SWING_OFF")){
        acSwing = SWING_OFF;
    }

    codes = getCodes(&newAc,atoi(argv[2]),acMode,atoi(argv[4]),acState,acSwing);

    int result = irSlingRaw(outPin,frequency,dutyCycle,codes,TIMINGS_LENGTH*sizeof(int)/sizeof(int));

    printf("[");
    for (i=0;i<TIMINGS_LENGTH;i++){
        printf("%d,",codes[i]);
    }
    printf("\b]\n");

    return result;
}
