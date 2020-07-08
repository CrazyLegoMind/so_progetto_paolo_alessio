#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <fcntl.h>
#include "../../common_lib/defs.h"

//AA test ricezione e invio dati su host

#define AMOUNT_PKGS 10
#define PIN 1
#define EOT 0x1

int main(int argc, char** argv) {
    //pacchetti locali: simulano una serie di letture sulla seriale
    static int* samples[] = { 20, 110, 76, 3, 58, 250, 67, 45, 200, 17 };
    DataPkg** pkgs = (DataPkg**)malloc(sizeof(DataPkg*) * AMOUNT_PKGS);
    for(int i=0; i < AMOUNT_PKGS; i++) {
        //fill packets
        pkgs[i] = (DataPkg*)malloc(sizeof(DataPkg));
        if(i == AMOUNT_PKGS-1)
            pkgs[i]->cmd = EOT;     //EOT = end of transmission
        else
            pkgs[i]->cmd = 0;
        pkgs[i]->data = samples[i];
    }

    //waiting for server uart...

    //freed memory
    for(int i=0; i < AMOUNT_PKGS; i++)
        free(pkgs[i]);
    free(pkgs);

    return 0;
}