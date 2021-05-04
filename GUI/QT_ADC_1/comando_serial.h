#ifndef COMANDO_SERIAL_H
#define COMANDO_SERIAL_H

#include<stdint.h>

class comando_serial
{
public:
    comando_serial(uint8_t label_4, uint8_t data_4, uint8_t data_8);
    uint16_t Valor();
    uint8_t Tamanho_bits();
    uint8_t Tamanho_bytes();

private:

    uint16_t valor;

    uint8_t label;
    uint8_t data_4;
    uint8_t data_8;

    uint8_t tamanho_bits;
    uint8_t tamanho_bytes;

};

#endif // COMANDO_SERIAL_H
