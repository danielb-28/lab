#include "comando_serial.h"

comando_serial::comando_serial(uint8_t label, uint8_t data_4, uint8_t data_8)
{

    this->label = label;
    this->data_4 = data_4;
    this->data_8 = data_8;

    this->tamanho_bits = 16;
    this->tamanho_bytes = 2;

    this->valor = this->label;
    this->valor |= (uint16_t) data_4 << 4;
    this->valor |= (uint16_t) data_8 << 8;

}

uint16_t comando_serial::Valor(){
    return this->valor;
}

uint8_t comando_serial::Tamanho_bits(){
    return this->tamanho_bits;
}

uint8_t comando_serial::Tamanho_bytes(){
    return this->tamanho_bytes;
}
