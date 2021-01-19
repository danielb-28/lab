#include "mainwindow.h"

#include <QApplication>

/*
#include <boost/asio.hpp>
#include <boost/asio/basic_serial_port.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>
#include<windows.h>

#include <QtWidgets/QMainWindow>
#include <QtCharts/QChartView> // Display charts
#include <QtCharts/QLegend> // Legenda
#include <QtCharts/QLineSeries> // Grafico de linha
#include <QtCharts/QCategoryAxis> // Mudar nomes nos eixos dos graficos
*/
# define BAUD 460800
# define SMP 1000

//QT_CHARTS_USE_NAMESPACE

const char* porta = "COM3";

int main(int argc, char *argv[])
{
    // QT
    QApplication a(argc, argv);
    MainWindow w;

    /*
    // Porta Serial
    boost::asio::io_service io;

    boost::asio::serial_port mcu(io); // Porta Serial

    mcu.open(porta); // Abre a porta definida pela string porta
    mcu.set_option(boost::asio::serial_port_base::baud_rate(BAUD)); // Define a baud rate

    // Comando Serial
    uint8_t comando = 0x01; // Comando
    comando |= (0b100 << 2); // Amostras
    comando |= (0b111 << 5); // Clock

    char* byte = reinterpret_cast<char*>(comando); // Transforma o comando em uma char

    // Dados
    std::string dados; // String de dados recebidos

    double dados_double[SMP]; // Array de dados double

    // Tempo
    double t_medio = 0; // Tempo medio das execucoes
    auto t1 = std::chrono::steady_clock::now(); // Tempo inicial
    auto t2 = std::chrono::steady_clock::now(); // Tempo final

    // Controle
    int j = 0; // Indice do array de dados double
    int inicio = 0; // Inicio da string de um dado especifico
    int fim = 1; // Final da string de um dado especifico

    t1 = std::chrono::steady_clock::now(); // Tempo inicial

    boost::asio::write(mcu, boost::asio::buffer(&comando, 8)); // Pedido dos dados

    boost::asio::read_until(mcu, boost::asio::dynamic_buffer(dados), 'E'); // Leitura dos dados

    mcu.close();

    j = 0;
    inicio = 0;
    fim = 1;

    // Conversao da string retornada para um array double
    for (int i = 0; i < dados.size(); i++)
    {

        // Final de uma string especifica
        if (dados[i] == ',')
        {

            dados_double[j] = boost::lexical_cast<double>(dados.substr(inicio, fim - inicio - 1)); // Conversao da string

            inicio = i + 1; // Redefine o inicio da string para a proxima string
            fim = inicio + 1; // Redefine o final da string

            j++; // Incrementa o indice do array double

        }

        // Continuacao da leitura
        else
        {
            if (dados[i] == 'E') break; // Verifica se chegou ao fim
            else fim++; // Incrementa o fim da string e continua a leitura
        }

    }

    t2 = std::chrono::steady_clock::now(); // Tempo final

    QLineSeries *series = new QLineSeries();

    for(int i = 0; i < SMP; i++){
        series->append(i, dados_double[i]);
    }

    QChart *grafico = new QChart(); // chart
    grafico->addSeries(series);
    grafico->legend()->hide();
    grafico->createDefaultAxes();
    grafico->setAnimationOptions(QChart::NoAnimation);

    QChartView *chartview = new QChartView(grafico); // Display chart

    w.setChart(grafico);
    */
    w.show();
    return a.exec();
}
