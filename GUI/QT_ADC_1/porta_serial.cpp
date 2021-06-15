#include "porta_serial.h"

porta_serial::porta_serial()
{

    this->porta = new boost::asio::serial_port(this->io);

}

porta_serial::~porta_serial()
{
    delete this->porta;
}

void porta_serial::serial_open(std::string dev, uint baud)
{
    this->porta->open(dev); // Abre a porta
    this->porta->set_option(boost::asio::serial_port_base::baud_rate(baud)); // Baudrate
}

void porta_serial::serial_send(comando_serial comando)
{

    boost::asio::write(*this->porta, boost::asio::buffer(&comando.valor, comando.Tamanho_bits()));

}

void porta_serial::serial_receive(std::string *dados_recebidos, uint tamanho_bytes)
{

    boost::asio::read(*this->porta, boost::asio::dynamic_buffer(*dados_recebidos, tamanho_bytes));

}

void porta_serial::serial_close()
{
    this->porta->send_break();
    this->porta->close();
}


