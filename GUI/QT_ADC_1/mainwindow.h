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

//#include <WinBase.h>


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

    void serial_open();
    void serial_config();
    void serial_close();
    void serial_start();
    void plot();
    void convert_dados(std::string dados);
    void update_parametros(std::string dados);
    void write_parametros(); // TEST
    void write_dados(); // TEST

private slots:

    void bt_inicio_click();
    void bt_parar_click();
    void bt_salvar_click();

    void plot_update();

};
#endif // MAINWINDOW_H
