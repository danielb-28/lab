#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <qserialportinfo.h>

#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/lexical_cast/lexical_cast_old.hpp>
#include <boost/chrono.hpp>
#include <boost/circular_buffer.hpp>

#include <termios.h>

#include <comando_serial.h>

#include <vector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:

    Ui::MainWindow *ui;

    // Grafico sinal
    QVector<double> x_data; // MOD
    QVector<double> y_data; // MOD

    // Grafico parametros
    QVector<double> x_data_param; // MOD
    QVector<double> y_data_param1; // MOD
    QVector<double> y_data_param2; // MOD
    QVector<double> y_data_param3; // MOD

    // Serial
    std::vector<comando_serial> comandos;
    void serial_open(); // Abrir a porta e configurar a baud rate // MOD
    void serial_config(); // Criar o comando serial // MOD
    void serial_start(); // Envia o comando e recebe os dados // MOD
    void serial_close(); // Fechar a porta serial // MOD

    // Processamento
    void convert_dados(std::string dados); // Sinal
    void update_parametros(std::string dados); // Parametros

    // Grafico
    void plot(); // Plot

    // IO
    void write_parametros(); // TEST
    void write_dados(); // TEST

private slots:

    // Botoes
    void bt_inicio_click();
    void bt_parar_click();
    void bt_salvar_click();

    // Timers
    void plot_update(); // Rotina de update - executada a cada timeout

};
#endif // MAINWINDOW_H
