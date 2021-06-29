#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

#define BAUD 460800 // Baud rate

#define N_PAR 3 // Numero de parametros monitorados

#define TMP_PATH /tmp // Path para arquivos temporarios

#define DEBUG_FLAG 1 // Ativa o DEBUG (1 - Mensagens de acoes e erros)

#define FPS 1 // Ativa a contagem de FPS

#define VREF 3.298 // Tensao de referencia para o ADC

// Serial
boost::asio::io_service io; // Contexto
boost::asio::serial_port mcu(io); // Porta

QTimer timer; // Timer para update

uint16_t comando; // Comando serial // TEST

uint16_t comando_pot; // Comando potenciometro serial // TEST

char comando_t[5]; // MOD

bool primeira_exec = true; // Controle do update - marcador de primeira execucao

bool pex; // TEST

int x_max = 0; // Valor maximo de amostras

bool inverter = false; // Inverter plot

// Buffers circulares para os parametros
boost::circular_buffer<double> param1_buffer(50);
boost::circular_buffer<double> param2_buffer(50);
boost::circular_buffer<double> param3_buffer(50);

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
    ui->plot_widget->addGraph(); // Sinal/parametro1
    ui->plot_widget->addGraph(); // Parametro2 - TEST
    ui->plot_widget->addGraph(); // Parametro3 - TEST
    ui->checkBox_parametro1->hide(); // Selecao 1 - TEST
    ui->checkBox_parametro2->hide(); // Selecao 2 - TEST
    ui->checkBox_parametro3->hide(); // Selecao 3 - TEST

    // Botoes - Connect
    connect(ui->bt_inicio, SIGNAL(released()), this, SLOT(bt_inicio_click()));
    connect(ui->bt_parar, SIGNAL(released()), this, SLOT(bt_parar_click()));
    connect(ui->bt_salvar, SIGNAL(released()), this, SLOT(bt_salvar_click()));

    // Botoes - Enable/Disable
    ui->bt_inicio->setEnabled(true);
    ui->bt_parar->setEnabled(false);
    ui->bt_salvar->setEnabled(true); 

    // Serial Ports - listar
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


    pex = true; // TEST

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
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "===Inicio"; // DEBUG 1 2
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Abrindo Porta..."; // DEBUG 1 2

    this->serial_open(); // Abre porta serial

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Configurando Serial..."; // DEBUG 1 2

    this->serial_config(); // Configura porta serial

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Aquisicao..."; // DEBUG 1 2

    this->serial_start(); // Aquisicao dos dados

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Plot..."; // DEBUG 1 2

    this->plot(); // Plota os dados

    //primeira_exec = false;

    pex=false; // TEST

    timer.start(); // Inicia o timer e a rotina de update

}

// Rotina de update - executada a cada timeout
void MainWindow::plot_update()
{

    if(FPS) t1 = std::chrono::steady_clock::now(); // Inicio

    this->serial_start(); // Aquisicao dos dados
    this->plot(); // Plota os dados

    if(FPS)
    {
        t2 = std::chrono::steady_clock::now(); // Fim
        std::chrono::duration<double> total = t2 - t1; // Tempo update

        ui->label_fps->setText(QString::number(1/total.count(), 'g', 2)); // Fps instantaneo
    }
}

// Rotina de parada
void MainWindow::bt_parar_click()
{
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "===Parada"; // DEBUG 1 2

    // Botoes - Enable/Disable
    ui->bt_inicio->setEnabled(true);

    ui->bt_parar->setEnabled(false);
    ui->bt_salvar->setEnabled(true);

    // Box - Enable/Disable
    ui->comboBox_porta->setEnabled(true);
    ui->comboBox_amostras->setEnabled(true);
    ui->comboBox_clock->setEnabled(true);

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Fechando porta serial..."; // DEBUG 1 2

    // Fechar porta serial
    this->serial_close();
}

// Salvar grafico
void MainWindow::bt_salvar_click(){

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Salvando dados e parametros..."; // DEBUG 1 2

    // Salvar dados - Pasta tmp - Alterar path para /tmp
    this->write_dados();
    this->write_parametros();

}

// Salva os parametros
void MainWindow::write_parametros(){

    QString path_parametros = "parametros.txt";

    // Verifica se os buffers estao cheios
    if(!param1_buffer.full()||!param2_buffer.full()||!param3_buffer.full()){
        if(DEBUG_FLAG>=0) qInfo() << "+++Erro ao salvar os parametros: os buffers nao estao cheios"; // DEBUG 0
        return;
    }
    QFile arquivo(path_parametros);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text)){
        if(DEBUG_FLAG>=0) qInfo() << "+++Erro ao Abrir o Arquivo: " << path_parametros; // DEBUG 0
        return;
    }

    QTextStream arquivo_in(&arquivo);

    for(uint i=0; i<(uint)param1_buffer.capacity(); i++){
        arquivo_in << param1_buffer.at(i) << "," << param2_buffer.at(i) << "," << param3_buffer.at(i) << "\n";
    }

    arquivo.close();

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Parametros salvos em: " << path_parametros; // DEBUG 1 2

    return;

}

// Salva os dados
void MainWindow::write_dados(){

    QString path_dados = "plot.txt";

    QFile arquivo(path_dados);
    if (!arquivo.open(QIODevice::WriteOnly | QIODevice::Text)){
        if(DEBUG_FLAG>=0) qInfo() << "+++Erro ao Abrir o Arquivo: " << path_dados; // DEBUG 0
        return;
    }

    QTextStream arquivo_in(&arquivo);

    for(int i=0; i<this->y_data.size(); i++){
        arquivo_in << this->y_data[i] << "\n";
    }

    arquivo.close();

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Dados salvos em: " << path_dados; // DEBUG 1 2

    return;
}

// Rotina para abrir a porta e configurar a baud rate
void MainWindow::serial_open()
{

    QString porta = ui->comboBox_porta->currentText(); // Get porta selecionada
    QString dev_str = "/dev/"; // Prefixo
    porta = dev_str.append(porta); // Forma a string da porta
    mcu.open(porta.toUtf8().constData()); // Abre a porta
    mcu.set_option(boost::asio::serial_port_base::baud_rate(BAUD)); // Configura a baud rate

    ui->label_status->setText("Conectado " + porta); // Indica a porta aberta
    ui->label_baud->setText(QString::number(BAUD)); // Indica a baud rate

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Conectado " << porta; // DEBUG 1 2

}

// Rotina para criar o comando serial
void MainWindow::serial_config()
{

    uint16_t clock = ui->comboBox_clock->currentIndex(); // Get clock selecionado
    int amostras = ui->comboBox_amostras->currentIndex(); // Get amostra selecionada
    bool dac_sinal = ui->comboBox_sinaldac->currentIndex(); // TEST
    uint8_t subsmp = (uint8_t) ui->comboBox_subamostragem->currentText().toInt() - 1; // TEST

    ui->label_amostras->setText(ui->comboBox_amostras->currentText()); // Indica o numero de amostras
    ui->label_clock->setText(ui->comboBox_clock->currentText()); // Indica o clock

    x_max = ui->comboBox_amostras->currentText().toInt(); // Get tamanho do eixo horizontal - Desatualizado

    // Criacao do comando serial - MOD - Reformular todo o processo de criacao do comando serial
    comando = 0x01;
    comando |= (uint16_t) subsmp << 4;
    comando |= (uint16_t) amostras << 8;
    comando |= (uint16_t) dac_sinal << 11;
    comando |= (uint16_t) clock << 13;


    if(DEBUG_FLAG==1||DEBUG_FLAG==2||DEBUG_FLAG==3) qInfo() << "Comando serial criado: " << (uint16_t) comando; // DEBUG 1 2 3
}

// Envia o comando e recebe os dados
void MainWindow::serial_start(){

    uint16_t i_label; // Label inteiro

    std::string s_label; // String de label

    std::string dados; // String de dados recebidos

    std::string parametros; // String de parametros recebidos

    // TEST - VALORES PARA O POTENCIOMETRO
    //comando_t[2] = (uint8_t) std::floor((ui->doubleSpinBox_set1->value()/100) * 255); // Valor percentual
    comando_t[2] = (uint8_t) ui->doubleSpinBox_set1->value(); // Valor 8 bits
    //comando_t[3] = (uint8_t) std::floor((ui->doubleSpinBox_set2->value()/100) * 255); // Valor percentual
    comando_t[3] = (uint8_t) ui->doubleSpinBox_set2->value(); // Valor 8 bits
    //comando_t[4] = (uint8_t) std::floor((ui->doubleSpinBox_set3->value()/100) * 255); // Valor percentual

    if(pex) boost::asio::write(mcu, boost::asio::buffer(&comando, 16)); // Comando para aquisição // TEST

    boost::asio::read(mcu, boost::asio::dynamic_buffer(s_label, 2)); // Label do pacote de dados

    i_label = (uint16_t)((s_label[0] << 8) + (s_label[1] & 0x00FF));

    s_label.clear(); // Necessario - PQ

    //qInfo() << "Label:" << i_label; // DEBUG

    if(i_label & 0x01){

        boost::asio::read(mcu, boost::asio::dynamic_buffer(parametros, 2*N_PAR));

        update_parametros(parametros);

    }

     x_max = (i_label >> 4);

     boost::asio::read(mcu, boost::asio::dynamic_buffer(dados, 2*x_max));

     // Potenciometros - TEST
     comando_pot = 0x02;

     comando_pot |= (uint16_t) comando_t[2] << 8;

     //qInfo() << "POT_1:" << (uint8_t) comando_t[2]; // DEBUG

     boost::asio::write(mcu, boost::asio::buffer(&comando_pot, 16));

     comando_pot = 0x03;

     comando_pot |= (uint16_t) comando_t[3] << 8;

     //qInfo() << "POT_0:" << (uint8_t) comando_t[3]; // DEBUG

     boost::asio::write(mcu, boost::asio::buffer(&comando_pot, 16));

     //


     // TEST
     uint16_t set1, set2;
     set1 = 0x0004;
     set1 |= (uint16_t) (this->ui->checkBox_3->isChecked() << 8);

     //qInfo()<<this->ui->checkBox_3->isChecked();

     //set1 |= (uint16_t) (true << 8); // TEST

     set2 = 0x0005;
     set2 |= (uint16_t) (this->ui->checkBox_4->isChecked() << 8);
     //set2 |= (uint16_t) (true << 8); // TEST

     boost::asio::write(mcu, boost::asio::buffer(&set1, 16)); // TEST
     boost::asio::write(mcu, boost::asio::buffer(&set2, 16)); // TEST
     //

     convert_dados(dados);

}

// Atualizacao dos parametros
void MainWindow::update_parametros(std::string dados){

    uint16_t dado_conv[N_PAR];
    double valor[N_PAR];

    for(int i=0; i<2*N_PAR; i+=2){
        dado_conv[i/2] = (uint16_t)((dados[i] << 8) + (dados[i+1] & 0x00FF));
        valor[i/2] = VREF * ((double) dado_conv[i/2] / 4096);
    }

    //qInfo() << (uint16_t) dado_conv[0]; // DEBUG
    //qInfo() << (uint16_t) dado_conv[1]; // DEBUG
    //qInfo() << (uint16_t) dado_conv[2]; // DEBUG

    // Buffers circulares
    param1_buffer.push_back(valor[0]);
    param2_buffer.push_back(valor[1]);
    param3_buffer.push_back(valor[2]);

    //if(param1_buffer.full()) qInfo() << "Buffer1 Cheio"; // DEBUG
    //if(param2_buffer.full()) qInfo() << "Buffer2 Cheio"; // DEBUG
    //if(param3_buffer.full()) qInfo() << "Buffer3 Cheio"; // DEBUG

    // Update UI
    ui->label_valor1->setText(QString::number(valor[0], 'f', 4));
    ui->label_valor2->setText(QString::number(valor[1], 'f', 4));
    ui->label_valor3->setText(QString::number(valor[2], 'f', 4));

}

// Conversao e processamento dos dados
void MainWindow::convert_dados(std::string dados)
{

    uint16_t dado_conv;

    uint16_t dado_ant = 0; // MOD

    float alfa = this->ui->doubleSpinBox->value(); // TEST
    inverter = this->ui->checkBox->checkState(); // TEST

    if (primeira_exec)
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

        if (primeira_exec){
            if(inverter==true){
                this->y_data.append(4096 - (double) dado_conv);
            }
            else{
                this->y_data.append((double) dado_conv);
            }
            this->x_data.append((double) index/2+1);
        }

        else
        {

            if(inverter==true){
                this->y_data.replace(index/2, 4096 - (double) dado_conv);
            }
            else{
                this->y_data.replace(index/2, (double) dado_conv);
            }
            this->x_data.replace(index/2, (double) index/2+1);
         }

        dado_ant = dado_conv;

    }

}

// Plot
void MainWindow::plot(){

    // Valores para os eixos
    int y_min = 0;
    int y_max = 4096;
    int x_min = 1;
    int graf_sel = this->ui->comboBox_graf_sel->currentIndex(); // TEST

    double param_min = VREF+1; // TEST
    double param_max = 0; // TEST

    // Pens - TEST
    QPen pen1;
    pen1.setColor(Qt::GlobalColor::blue);

    QPen pen2;
    pen2.setColor(Qt::GlobalColor::magenta);

    QPen pen3;
    pen3.setColor(Qt::GlobalColor::red);
    //

    if (primeira_exec){

        // Configuracao do plot
        ui->plot_widget->clearGraphs();
        ui->plot_widget->addGraph();
        ui->plot_widget->addGraph(); // Parametro2 - TEST
        ui->plot_widget->addGraph(); // Parametro3 - TEST

        // TEST - OTIMIZAR
        switch(graf_sel){
            case 0:
                ui->checkBox_parametro1->hide();
                ui->checkBox_parametro2->hide();
                ui->checkBox_parametro3->hide();
                ui->plot_widget->graph(0)->setData(this->x_data, this->y_data);
                ui->plot_widget->xAxis->setRange(x_min, x_max);
                ui->plot_widget->yAxis->setRange(y_min, y_max);
            break;

            case 1:
                ui->checkBox_parametro1->show();
                ui->checkBox_parametro2->show();
                ui->checkBox_parametro3->show();
                x_data_param.clear();
                y_data_param1.clear();
                y_data_param2.clear();
                y_data_param3.clear();
                param_min = VREF+1;
                param_max = 0;

                // Criacao do vetor de indices
                for(int i=0; i<(int)param1_buffer.size(); i++){
                    this->x_data_param.append(i); // TEST
                }

                // Plot parametro1
                if(this->ui->checkBox_parametro1->isChecked()){

                    for(int i=0; i<(int)param1_buffer.size(); i++){
                        this->y_data_param1.append(param1_buffer[i]); // TEST
                        if(this->y_data_param1[i] > param_max) param_max = y_data_param1[i];
                        if(this->y_data_param1[i] < param_min) param_min = y_data_param1[i];
                    }
                    ui->plot_widget->graph(0)->setData(this->x_data_param, this->y_data_param1);
                    ui->plot_widget->graph(0)->setPen(pen1);
                }

                // Plot parametro2
                if(this->ui->checkBox_parametro2->isChecked()){

                    for(int i=0; i<(int)param2_buffer.size(); i++){
                        this->y_data_param2.append(param2_buffer[i]); // TEST
                        if(this->y_data_param2[i] > param_max) param_max = y_data_param2[i];
                        if(this->y_data_param2[i] < param_min) param_min = y_data_param2[i];
                    }
                    ui->plot_widget->graph(1)->setData(this->x_data_param, this->y_data_param2);
                    ui->plot_widget->graph(1)->setPen(pen2);
                }

                // Plot parametro3
                if(this->ui->checkBox_parametro3->isChecked()){

                    for(int i=0; i<(int)param3_buffer.size(); i++){
                        this->y_data_param3.append(param3_buffer[i]); // TEST
                        if(this->y_data_param3[i] > param_max) param_max = y_data_param3[i];
                        if(this->y_data_param3[i] < param_min) param_min = y_data_param3[i];
                    }
                    ui->plot_widget->graph(2)->setData(this->x_data_param, this->y_data_param3);
                }

                ui->plot_widget->xAxis->setRange(0, (int)param1_buffer.size()-1);
                ui->plot_widget->yAxis->setRange(param_min-0.01, param_max+0.01);
                ui->plot_widget->graph(2)->setPen(pen3);

            break;
            }
             //

        // Configuracoes graficas
        ui->plot_widget->graph(0)->setAntialiased(true);
        ui->plot_widget->graph(0)->setAdaptiveSampling(true);
        /*
        if(DEBUG_FLAG==1||DEBUG_FLAG==2)
        {
            qInfo() << "---Plot: ";
            qInfo() << "X minimo: " << x_min;
            qInfo() << "X maximo: " << x_max;
            qInfo() << "Y minimo: " << y_min;
            qInfo() << "Y maximo: " << y_max;
        }
        */
    }

    // Rotina de atualizacao do plot
    ui->plot_widget->replot();

}

// Fechar a porta serial
void MainWindow::serial_close(){
    timer.disconnect(&timer, SIGNAL(timeout()), this, SLOT(plot_update())); // Desativa o timer de atualizacao do grafico

    // Send break serial
    tcsendbreak(mcu.native_handle(), 0);
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Microcontrolador resetado"; // DEBUG 1 2

    mcu.close(); // Fecha a porta serial
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Porta serial fechada"; // DEBUG 1 2

}
