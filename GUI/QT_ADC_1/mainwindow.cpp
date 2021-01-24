#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#define BAUD 460800 // Baud rate

// Serial
boost::asio::io_service io; // Contexto
boost::asio::serial_port mcu(io); // Porta

QTimer timer; // Timer update

uint16_t comando; // Comando serial

bool primeira_exec2 = true; // Controle do update // MOD

int x_max = 0; // Valor maximo de amostras

bool inverter = false; // Inverter plot

// Tempo para monitorar o fps
auto t1 = std::chrono::steady_clock::now();
auto t2 = std::chrono::steady_clock::now();

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->horizontalFrame); // Set widget central

    // Grafico
    ui->plot_widget->addGraph();

    // Botoes - Connect
    connect(ui->bt_inicio, SIGNAL(released()), this, SLOT(bt_inicio_click()));
    connect(ui->bt_parar, SIGNAL(released()), this, SLOT(bt_parar_click()));
    connect(ui->bt_salvar, SIGNAL(released()), this, SLOT(bt_salvar_click()));

    // Botoes - Enable/Disable
    ui->bt_inicio->setEnabled(true);
    ui->bt_parar->setEnabled(false);
    ui->bt_salvar->setEnabled(true); 

    // Serial Ports
    Q_FOREACH(QSerialPortInfo porta_ava, QSerialPortInfo::availablePorts())
    {
            ui->comboBox_porta->addItem(porta_ava.portName());
    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

// Inicio
void MainWindow::bt_inicio_click()
{

    // Timer update
    connect(&timer, SIGNAL(timeout()), this, SLOT(plot_update()));
    timer.setInterval(0);

    // Botoes - Enable/Disable
    ui->bt_inicio->setEnabled(false);
    ui->bt_parar->setEnabled(true);
    ui->bt_salvar->setEnabled(false);

    // Box - Enable/Disable
    ui->comboBox_porta->setEnabled(false);
    ui->comboBox_amostras->setEnabled(false);
    ui->comboBox_clock->setEnabled(false);

    // Rotina de inicio
    this->serial_open();
    this->serial_config();
    this->serial_start();
    this->plot();

    timer.start(); // Inicia o timer

}

// Rotina de update - executada a cada timeout
void MainWindow::plot_update()
{
    t1 = std::chrono::steady_clock::now(); // Inicio

    this->serial_start();
    this->plot();

    t2 = std::chrono::steady_clock::now(); // Fim
    std::chrono::duration<double> total = t2 - t1; // Tempo update

    ui->label_fps->setText(QString::number(1/total.count(), 'g', 2)); // Fps

}

// Rotina de parada
void MainWindow::bt_parar_click()
{
    // Botoes - Enable/Disable
    ui->bt_inicio->setEnabled(true);
    ui->bt_parar->setEnabled(false);
    ui->bt_salvar->setEnabled(true);

    // Box - Enable/Disable
    ui->comboBox_porta->setEnabled(true);
    ui->comboBox_amostras->setEnabled(true);
    ui->comboBox_clock->setEnabled(true);

    // Fechar porta serial
    this->serial_close();

}

// Salvar grafico
void MainWindow::bt_salvar_click()
{

}

// Rotina para abrir a porta e configurar a baud rate
void MainWindow::serial_open()
{

    QString porta = ui->comboBox_porta->currentText(); // Get porta selecionada
    QString dev_str = "/dev/"; // Prefixo
    porta = dev_str.append(porta); // Forma a string da porta
    mcu.open(porta.toUtf8().constData()); // Abre a porta
    mcu.set_option(boost::asio::serial_port_base::baud_rate(BAUD)); // Configura a baud rate

    ui->label_status->setText("Conectado / " + porta); // Indica a porta aberta
    ui->label_baud->setText(QString::number(BAUD)); // Indica a baud rate

}

// Rotina para configurar o comando serial
void MainWindow::serial_config()
{

    int clock = ui->comboBox_clock->currentIndex(); // Get clock selecionado
    int amostras = ui->comboBox_amostras->currentIndex(); // Get amostra selecionada
    bool dac_sinal = ui->comboBox_sinaldac->currentIndex();

    ui->label_amostras->setText(ui->comboBox_amostras->currentText()); // Indica o numero de amostras
    ui->label_clock->setText(ui->comboBox_clock->currentText()); // Indica o clock

    x_max = ui->comboBox_amostras->currentText().toInt(); // Get tamanho do eixo horizontal

    // Criacao do comando serial
    comando = 0x02; // Trigger externo desativado
    comando |= (amostras << 2);
    comando |= (clock << 5);
    comando |= (dac_sinal << 8);

    qInfo() << (uint16_t) comando; // DEBUG

}

// Envia o comando e recebe os dados
void MainWindow::serial_start(){

    uint16_t i_label; // Label inteiro

    std::string s_label; // String de label

    std::string dados; // String de dados recebidos

    boost::asio::write(mcu, boost::asio::buffer(&comando, 16)); // Comando para aquisição

    boost::asio::read(mcu, boost::asio::dynamic_buffer(s_label, 2)); // Label do pacote de dados

    i_label = (uint16_t)((s_label[0] << 8) + (s_label[1] & 0x00FF));

    x_max = (i_label >> 4);

    boost::asio::read(mcu, boost::asio::dynamic_buffer(dados, 2*(i_label >> 4)));

    convert_dados(dados);

}

// Conversao e processamento dos dados
void MainWindow::convert_dados(std::string dados)
{

    uint16_t dado_conv;

    uint16_t dado_ant = 0; // MOD

    float alfa = this->ui->doubleSpinBox->value(); // TEST
    inverter = this->ui->checkBox->checkState(); // TEST

    if (primeira_exec2)
    {
        this->x_data.clear();
        this->y_data.clear();
    }

    for(int index=0; index<2*x_max; index+=2)
    {
        dado_conv = (uint16_t)((dados[index] << 8) + (dados[index+1] & 0x00FF));

        // Filtro
        if(index>0)
        {

            dado_conv = dado_ant + alfa * (dado_conv - dado_ant);

        }

        if (primeira_exec2){
            if(inverter==true){
                this->y_data.append(4096 - (double) dado_conv);
            }
            else{
                this->y_data.append((double) dado_conv);
            }
            this->x_data.append((double) index/2);
        }

        else
        {

            if(inverter==true){
                this->y_data.replace(index/2, 4096 - (double) dado_conv);
            }
            else{
                this->y_data.replace(index/2, (double) dado_conv);
            }
            this->x_data.replace(index/2, (double) index/2);
         }

        dado_ant = dado_conv;

    }

}

// Rotina de plot
void MainWindow::plot(){

    if (primeira_exec2){

        // Configuracao do plot
        ui->plot_widget->clearGraphs();
        ui->plot_widget->addGraph();
        ui->plot_widget->graph(0)->setData(this->x_data, this->y_data);
        ui->plot_widget->xAxis->setRange(0, x_max);
        ui->plot_widget->yAxis->setRange(0, 4096);

        // Configuracoes graficas
        ui->plot_widget->graph(0)->setAntialiased(true);
        ui->plot_widget->graph(0)->setAdaptiveSampling(true);

    }

    // Rotina de atualizacao do plot
    ui->plot_widget->graph(0)->setData(this->x_data, this->y_data);
    ui->plot_widget->replot();

}

// Rotina de parada
void MainWindow::serial_close()
{
    timer.disconnect(&timer, SIGNAL(timeout()), this, SLOT(plot_update())); // Desativa o timer de atualizacao do grafico

    // Send break serial
    //SetCommBreak(mcu.native_handle());
    //Sleep(1);
    //ClearCommBreak(mcu.native_handle());
    tcsendbreak(mcu.native_handle(), 0);
    mcu.close(); // Fecha a porta serial
}
