#ifndef COMANDO_SERIAL_H
#define COMANDO_SERIAL_H


class comando_serial
{
public:
    ccomando_serial(uint8_t label = 0x01, uint8_t data_4 = 0x00, uint8_t data_8 = 0x00);
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
