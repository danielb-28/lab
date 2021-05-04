#include "porta_serial.h"

porta_serial::porta_serial()
{

}

void porta_serial::serial_open(std::string dev, uint baud)
{
    //this->porta.open(dev); // Abre a porta
    //this->porta.set_option(boost::asio::serial_port_base::baud_rate(baud));
}

void porta_serial::serial_send(comando_serial comando)
{

    //boost::asio::write(this->porta, boost::asio::buffer(&comando.Valor(), comando.Tamanho_bits()));

}

void porta_serial::serial_receive(std::string *dados_recebidos, uint tamanho_bytes)
{

    //boost::asio::read(this->porta, boost::asio::dynamic_buffer(*dados_recebidos, tamanho_bytes));

}

