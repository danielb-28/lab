#include "comando_serial.h"

comando_serial::comando_serial(uint16_t label)
{

    this->label = label & 0x07FF; // Limite de 11 bits

    this->dados.fill(0x00); // Array de dados
}

/*
uint8_t comando_serial::Tamanho_bits(){
    return this->tamanho_bits;
}
*/
