#include "comando_serial.h"

Comando_serial::Comando_serial(uint16_t label)
{

    this->label = label & 0x07FF; // Limite de 11 bits

    this->dados.fill(0x00); // Array de dados
}


uint16_t Comando_serial::Get_label(){
    return this->label;
}
