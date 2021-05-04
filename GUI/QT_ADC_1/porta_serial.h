#include <comando_serial.h>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>

#ifndef PORTA_SERIAL_H
#define PORTA_SERIAL_H


class porta_serial
{
public:
    porta_serial();

private:

    boost::asio::io_service io; // Contexto
    boost::asio::serial_port porta(this->io); // Porta

    std::vector<comando_serial> vetor_comandos;

    void serial_open(std::string dev, uint baud);

    void serial_config();

    void serial_send(comando_serial comando);

    void serial_receive(std::string* dados_recebidos);

};

#endif // PORTA_SERIAL_H
