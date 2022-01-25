#ifndef COMANDO_SERIAL_H
#define COMANDO_SERIAL_H

#include<stdint.h>
#include<array>

class Comando_serial
{
public:

    Comando_serial(uint16_t label);

    uint16_t Get_label();

    std::array<uint8_t, 8> dados; // Dados de 8 bytes

private:

    uint16_t label; // Label de 11 bits

};

#endif // COMANDO_SERIAL_H
