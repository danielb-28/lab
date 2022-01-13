#ifndef COMANDO_SERIAL_H
#define COMANDO_SERIAL_H

#include<stdint.h>
#include<array>

class comando_serial
{
public:

    comando_serial(uint16_t label);

    uint16_t label; // Label de 11 bits
    std::array<uint8_t, 8> dados; // Dados de 8 bytes

private:

};

#endif // COMANDO_SERIAL_H
