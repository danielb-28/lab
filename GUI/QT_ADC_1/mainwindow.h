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

    QVector<double> x_data;
    QVector<double> y_data;
    QVector<double> x_data_param; // TEST
    QVector<double> y_data_param1; // TEST
    QVector<double> y_data_param2; // TEST
    QVector<double> y_data_param3; // TEST

    void serial_open(); // Abrir a porta e configurar a baud rate
    void serial_config(); // Criar o comando serial
    void serial_start(); // Envia o comando e recebe os dados
    void convert_dados(std::string dados); // Conversao e processamento dos dados
    void plot(); // Plot
    void update_parametros(std::string dados); // Atualizacao dos parametros
    void write_parametros(); // TEST
    void write_dados(); // TEST
    void serial_close(); // Fechar a porta serial

private slots:

    // Botoes
    void bt_inicio_click();
    void bt_parar_click();
    void bt_salvar_click();

    // Timers
    void plot_update(); // Rotina de update - executada a cada timeout

};
#endif // MAINWINDOW_H
