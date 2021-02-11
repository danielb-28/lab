#include "mbed.h"
#include <LPC17xx.h>

#define SPI_MOSI p5
#define SPI_MISO p6
#define SPI_SCLK p7

#define SPI_CS0 p8
#define SPI_CS1 p9

// REFERENCIA =================================
// spi.format(,); // Tamanho bit comando, modo
// spi.frequency(1000000); // Freq 
// spi.write(0x0000); // Comando
    

DigitalOut cs0(SPI_CS0); // Pin
DigitalOut cs1(SPI_CS1); // Pin
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCLK); // mosi, miso, sclk

uint8_t valor;

void spi_setup(uint8_t bits, uint8_t mode, uint32_t freq);
void spi_pot(uint8_t id, uint8_t value);

void spi_setup(uint8_t bits, uint8_t mode, uint32_t freq){
    spi.format(bits, mode);
    spi.frequency(freq); 
    cs0 = 1;
    cs1 = 1;  
    return;
}

void spi_pot(uint8_t id, uint8_t value){
    switch(id){
        case 0:
            cs0 = 0;
        break;
        
        case 1:
            cs1 = 0;
        break;
    }
    
    spi.write(0x1300 | (uint16_t) value);
    
    switch(id){
        case 0:
            cs0 = 1;
        break;
        
        case 1:
            cs1 = 1;
        break;
    }
    
    return;        
}

int main(){
    
    spi_setup(16, 0, 1000000); // bits, mode, freq
        
    valor = 0;
    
    while(1){
        
        if(valor==255) valor = 1;
        
        spi_pot(0, valor); // id, valor
        spi_pot(1, valor); // id, valor
        
        valor++;
        wait_ms(500);

    }    
    
}