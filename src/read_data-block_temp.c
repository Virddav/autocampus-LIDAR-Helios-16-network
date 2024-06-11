#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <errno.h>

void read_datablock (unsigned char* packet){
	for(int y = 0; y<12; y++){
		unsigned char* azimuth = malloc(sizeof(unsigned char) * 2);
		memcpy(azimuth,packet+42+2+100*y,2);
	        unsigned char* cloudpoint = malloc(sizeof(unsigned char) * 96);
	        memcpy(cloudpoint,packet+42+4+100*y,96);

        	printf("Datablock %d : \nAzimuth = %02X%02X\n", y, azimuth[0], azimuth[1]);
	    	for (int i=0; i<96;i++){
	      		printf("%02X ",cloudpoint[i]);
	    	}
	    	printf("\n\n\n");
	}
}

int main() {
    unsigned char packet[1248];

    // Remplir le tableau avec des valeurs hexadécimales (0x00 à 0xFF)
    for (int i = 0; i < 1248; i++) {
        packet[i] = (unsigned char)(i % 256); // Les valeurs cyclent de 0x00 à 0xFF
    }

    read_datablock(packet);

    return 1;
}
