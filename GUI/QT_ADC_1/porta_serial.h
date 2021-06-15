#include <comando_serial.h>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#ifndef PORTA_SERIAL_H
#define PORTA_SERIAL_H


class porta_serial
{
public:
    porta_serial();
    ~porta_serial();

    void serial_open(std::string dev, uint baud);

    void serial_config();

    void serial_send(comando_serial comando);

    void serial_receive(std::string* dados_recebidos, uint tamanho_bytes);

    void serial_close();

private:

    boost::asio::io_service io; // Contexto
    boost::asio::serial_port *porta; // Porta

    std::vector<comando_serial> vetor_comandos;

};

#endif // PORTA_SERIAL_H
