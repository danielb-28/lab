#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

typedef uint8_t __u8; // TEST

// Constantes
const QString temp_path = "/tmp/";

#define BAUD 460800 // Baud rate

#define CAN_NAME "can1"

#define N_PAR 3 // Numero de parametros monitorados

#define DEBUG_FLAG 1 // Ativa o DEBUG (1 - Mensagens de acoes e erros)

#define FPS 1 // Ativa a contagem de FPS

#define VREF 3.32 // Tensao de referencia para o ADC

// Serial
boost::asio::io_service io; // Contexto
boost::asio::serial_port mcu(io); // Porta

QTimer timer; // Timer para update

uint16_t comando; // Comando serial // MOD
char comando_can[2]; // MOD

// Globais
bool pex; // Flag de primeira execucao

int x_max = 0; // Valor maximo de amostras

bool inverter = false; // Inverter plot // MOD - N PRECISA SER GLOBAL

int x_max_set = 0; // DEBUG

// Buffers circulares para os parametros
boost::circular_buffer<double> param1_buffer(50);
boost::circular_buffer<double> param2_buffer(50);
boost::circular_buffer<double> param3_buffer(50);

// Tempo para monitorar o fps
auto t1 = std::chrono::steady_clock::now(); // MOD - Tornar atributo
auto t2 = std::chrono::steady_clock::now(); // MOD - Tornar atributo

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setCentralWidget(ui->horizontalFrame); // Set widget central

    // OpenGL qcustomplot
    ui->plot_widget->setOpenGl(true);

    // Graficos
    ui->plot_widget->addGraph(); // Sinal/parametro1
    ui->plot_widget->addGraph(); // Parametro2
    ui->plot_widget->addGraph(); // Parametro3

    // Selecao grafico parametro - Hide
    ui->checkBox_parametro1->hide(); // Parametro 1
    ui->checkBox_parametro2->hide(); // Parametro 2
    ui->checkBox_parametro3->hide(); // Parametro 3

    // Botoes - Connect
    connect(ui->bt_inicio, SIGNAL(released()), this, SLOT(bt_inicio_click()));
    connect(ui->bt_parar, SIGNAL(released()), this, SLOT(bt_parar_click()));
    connect(ui->bt_salvar, SIGNAL(released()), this, SLOT(bt_salvar_click()));

    // Botoes - Enable/Disable
    ui->bt_inicio->setEnabled(true);
    ui->bt_parar->setEnabled(false);
    ui->bt_salvar->setEnabled(true); 

    // Serial - Listar Portas
    Q_FOREACH(QSerialPortInfo porta_ava, QSerialPortInfo::availablePorts())
    {
            ui->comboBox_porta->addItem(porta_ava.portName());
    }

}

MainWindow::~MainWindow()
{
    this->serial_close(); // Fecha a porta serial e reinicia o mcu

    delete ui;
}

// Inicio
void MainWindow::bt_inicio_click()
{

    pex = true; // Flag de primeira execucao

    // Timer update
    connect(&timer, SIGNAL(timeout()), this, SLOT(plot_update()));
    //timer.setInterval(33.3); // 30 hz
    //timer.setInterval(10); // 100 hz
    timer.setInterval(1); // 1 khz

    // Botoes - Enable/Disable
    ui->bt_inicio->setEnabled(false);
    ui->bt_parar->setEnabled(true);
    ui->bt_salvar->setEnabled(false);

    // ComboBox - Enable/Disable
    ui->comboBox_porta->setEnabled(false);
    ui->comboBox_amostras->setEnabled(false);
    ui->comboBox_clock->setEnabled(false);
    ui->comboBox_subamostragem->setEnabled(false);
    ui->comboBox_sinaldac->setEnabled(false);

    // Rotina de inicio
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "===Inicio"; // DEBUG 1 2
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Abrindo Porta..."; // DEBUG 1 2

    this->serial_open(); // Abre porta serial
    this->can_init();

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Configurando Serial..."; // DEBUG 1 2

    this->serial_config(); // Configura porta serial

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Aquisicao..."; // DEBUG 1 2

    this->serial_start(); // Aquisicao dos dados

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Plot..."; // DEBUG 1 2

    this->plot(); // Plota os dados

    pex=false; // Flag de primeira execucao

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
    ui->comboBox_subamostragem->setEnabled(true);
    ui->comboBox_sinaldac->setEnabled(true);

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "---Fechando porta serial..."; // DEBUG 1 2

    // Fechar porta serial
    this->serial_close();
}

// Salvar grafico
void MainWindow::bt_salvar_click(){

    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Salvando dados e parametros..."; // DEBUG 1 2

    // Salvar dados - Pasta tmp - Alterar path para /tmp
    this->write_dados(); // Salva o grafico de sinal
    this->write_parametros(); // Salva o grafico de parametros

}

// Salva os parametros
void MainWindow::write_parametros(){

    QString path_parametros = temp_path + "parametros.txt";

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

    QString path_dados = temp_path + "plot.txt";

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


    tcflush(mcu.native_handle(), 0); // Flush

}


int MainWindow::can_init()
{

	// file_desc
	if ((can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) // Retorna o file_desc 
	{
		qInfo() << "Erro na criacao do socket";
		return 1;
	}

	qInfo() << "Socket CAN Criado";
	
	// ifr_name
	strcpy(can_ifreq.ifr_name, CAN_NAME);
	
	// indice da interface
	ioctl(can_fd, SIOCGIFINDEX, &can_ifreq); // retorna o indice em can_ifreq.ifr_ifindex
	
	// addr
	memset(&can_addr, 0, sizeof(can_addr));
	can_addr.can_family = AF_CAN;
	can_addr.can_ifindex = can_ifreq.ifr_ifindex;
	
	// bind
	if (bind(can_fd, (struct sockaddr* )&can_addr, sizeof(can_addr)) < 0) {
		qInfo() << "Erro no bind";
		return 1;
	}

	qInfo() << "Bind Realizado";

	return 0;
}


int MainWindow::can_end()
{
	// Fecha o socket
    if (::close(can_fd) < 0) {
		qInfo() << "Erro ao fechar o socket";
		return 1;
	}

	qInfo() << "Socket CAN Fechado";
}
// Rotina para criar o comando serial
void MainWindow::serial_config()
{

    uint16_t clock = ui->comboBox_clock->currentIndex(); // Get clock selecionado
    int amostras = ui->comboBox_amostras->currentIndex(); // Get amostra selecionada
    bool dac_sinal = ui->comboBox_sinaldac->currentIndex(); // Sinal gerado pelo DAC
    uint8_t subsmp = (uint8_t) ui->comboBox_subamostragem->currentText().toInt() - 1; // Subamostragem

    ui->label_amostras->setText(ui->comboBox_amostras->currentText()); // Indica o numero de amostras
    ui->label_clock->setText(ui->comboBox_clock->currentText()); // Indica o clock

    x_max = ui->comboBox_amostras->currentText().toInt(); // Get tamanho do eixo horizontal - Desatualizado

    n_comandos.push_back(Comando_serial(0x01)); // TEST

    // Criacao do comando serial - MOD
    comando = 0x01;
    comando |= (uint16_t) subsmp << 4;
    comando |= (uint16_t) amostras << 8;
    comando |= (uint16_t) dac_sinal << 11;
    comando |= (uint16_t) clock << 13;

    // Criacao do comando CAN - TEST
    comando_can[0] = 0x01;
    comando_can[0] |= (char) subsmp << 4;
    comando_can[1] = (char) amostras;
    comando_can[1] |= (char) dac_sinal << 3;
    comando_can[1] |= (char) clock << 5;

    x_max_set = x_max; // DEBUG

    if(DEBUG_FLAG==1||DEBUG_FLAG==2||DEBUG_FLAG==3) qInfo() << "Comando serial criado: " << (uint16_t) comando; // DEBUG 1 2 3
    if(DEBUG_FLAG==1||DEBUG_FLAG==2||DEBUG_FLAG==3) qInfo() << "Comando can criado: " << (uint8_t) comando_can[0] << "  " << (uint8_t) comando_can[1]; // DEBUG 1 2 3
}

// Envia o comando e recebe os dados
void MainWindow::serial_start(){

    uint16_t i_label; // Label inteiro

    std::string s_label; // Buffer de label

    std::string dados; // Buffer de dados recebidos

    std::vector<__u8> dados_u8;

    std::string parametros; // Buffer de parametros recebidos

    char comando_t[6]; // Comandos para potenciometros

    uint16_t comando_pot; // Buffer potenciometros

    // Dados can
    struct can_frame frame; // data frame

    frame.can_id = 0x001;
    frame.can_dlc = 2;
    frame.data[0] = comando_can[0];
    frame.data[1] = comando_can[1];

    // Filtro can
    struct can_filter filtro_can;
    filtro_can.can_id   = 0x002;
    filtro_can.can_mask = 0xFFFF;

    int bytes_recebidos = -1;

    setsockopt(can_fd, SOL_CAN_RAW, CAN_RAW_FILTER, &filtro_can, sizeof(filtro_can));

    // Leitura set
    //comando_t[1] = (uint8_t) std::floor((ui->doubleSpinBox_set1->value()/100) * 255); // Valor percentual
    //comando_t[1] = (uint8_t) std::floor(((VREF - ui->doubleSpinBox_set1->value())/VREF) * 255); // Valor absoluto
    comando_t[1] = (uint8_t) ui->doubleSpinBox_set1->value(); // Valor 8 bits

    //comando_t[3] = (uint8_t) std::floor((ui->doubleSpinBox_set2->value()/100) * 255); // Valor percentual
    //comando_t[3] = (uint8_t) std::floor(((VREF - ui->doubleSpinBox_set2->value())/VREF) * 255); // Valor absoluto
    comando_t[3] = (uint8_t) ui->doubleSpinBox_set2->value(); // Valor 8 bits

    // Aquisicao
    /*if(pex){

        boost::asio::write(mcu, boost::asio::buffer(&comando, 2)); // Comando para aquisição
        boost::asio::write(mcu, boost::asio::buffer(&comando, 2)); // Comando para aquisição
    }*/

    //boost::asio::write(mcu, boost::asio::buffer(&comando, 2)); // Comando para aquisição
    //boost::asio::write(mcu, boost::asio::buffer(&comando, 2)); // Comando para aquisição
    
    // Envio CAN
    if (write(can_fd, &frame, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
    	qInfo() << "Erro no envio dos dados can";
    }
    qInfo() << "Dados can enviados";

    //boost::asio::read(mcu, boost::asio::dynamic_buffer(s_label, 2)); // Label do pacote de dados

    // Recebimento CAN - Label
    bytes_recebidos = ::read(can_fd, &frame, sizeof(struct can_frame));
        if (bytes_recebidos < 0) {
            qInfo() << "Erro no recebimento can";
    }
    s_label[0] = frame.data[0];
    s_label[1] = frame.data[1];

    i_label = (uint16_t)((s_label[0] << 8) + (s_label[1] & 0x00FF));

    qInfo() << "Label Recebido: " << i_label ; // DEBUG

    s_label.clear(); // Necessario - PQ

    if(i_label & 0x01){ // Leitura parametros

        // Recebimento CAN - Parametros
        frame.can_dlc = 2*N_PAR;
        int bytes_recebidos = ::read(can_fd, &frame, sizeof(struct can_frame));
            if (bytes_recebidos < 0) {
                qInfo() << "Erro no recebimento can - parametros";
        }

        //sprintf(parametros, "%s", (std::string*) frame.data);
        //boost::asio::read(mcu, boost::asio::dynamic_buffer(parametros, 2*N_PAR));

        //update_parametros(parametros);
        //update_parametros(frame.data);

    }


    x_max = (i_label >> 4); // Numero de amostras que serao recebidas

    //qInfo() << "Numero Amostras Label: " << x_max ; // DEBUG

    /*
    if(x_max != x_max_set){
        qInfo() << "Numero de amostras diferente do esperado: " << x_max_set;
        x_max = x_max_set;
    }
    */

    //boost::asio::read(mcu, boost::asio::dynamic_buffer(dados, 2*x_max)); // Recebimento das amostras

    // Recebimento CAN - Dados
    int cnt = 0;
    int n_pacotes = 2*x_max; // remover 2
    while(cnt < n_pacotes/2){
        bytes_recebidos = ::read(can_fd, &frame, sizeof(struct can_frame));
            if (bytes_recebidos < 0) {
                qInfo() << "Erro no recebimento can - dados";
        }

        for(int i = 0; i < frame.can_dlc; i++){
            dados_u8.push_back(frame.data[i]);
            qInfo() << "frame " << cnt <<  " - " << i << "   " << frame.data[i] << "   " << dados_u8.back();
        }
        cnt++;
    }


     qInfo() << "Leitura Amostras can - OK " << cnt ; // DEBUG

     // Potenciometros
     /*
     comando_t[0] = 0x02; // Label pot1

     comando_pot = comando_t[0] | (uint16_t) comando_t[1] << 8; // Comando pot1

     boost::asio::write(mcu, boost::asio::buffer(&comando_pot, 2)); // Envio comando pot1
     boost::asio::write(mcu, boost::asio::buffer(&comando_pot, 2)); // Envio comando pot1

     comando_t[2] = 0x03; // label pot2

     comando_pot = comando_t[2] | (uint16_t) comando_t[3] << 8; // Comando pot2

     boost::asio::write(mcu, boost::asio::buffer(&comando_pot, 2)); // Envio comando pot2
     boost::asio::write(mcu, boost::asio::buffer(&comando_pot, 2)); // Envio comando pot2


     // Locks
     uint16_t lock1, lock2;

     lock1 = 0x0004; // Label lock1
     lock1 |= (uint16_t) (this->ui->checkBox_lock1->isChecked() << 8); // Valor lock1

     lock2 = 0x0005; // Label lock2
     lock2 |= (uint16_t) (this->ui->checkBox_lock2->isChecked() << 8); // Valor lock2

     boost::asio::write(mcu, boost::asio::buffer(&lock1, 2)); // Envio lock1
     boost::asio::write(mcu, boost::asio::buffer(&lock1, 2)); // Envio lock1

     boost::asio::write(mcu, boost::asio::buffer(&lock2, 2)); // Envio lock2
     boost::asio::write(mcu, boost::asio::buffer(&lock2, 2)); // Envio lock2
     //
     */

     convert_dados(dados_u8); // Conversao dos dados

}

// Atualizacao dos parametros
void MainWindow::update_parametros(std::string dados){

    uint16_t dado_conv[N_PAR];
    double valor[N_PAR];

    for(int i=0; i<2*N_PAR; i+=2){
        dado_conv[i/2] = (uint16_t)((dados[i] << 8) + (dados[i+1] & 0x00FF));
        valor[i/2] = VREF * ((double) dado_conv[i/2] / 4096);
    }

    // Buffers circulares
    param1_buffer.push_back(valor[0]);
    param2_buffer.push_back(valor[1]);
    param3_buffer.push_back(valor[2]);

    // Update UI
    ui->label_valor1->setText(QString::number(valor[0], 'f', 4));
    ui->label_valor2->setText(QString::number(valor[1], 'f', 4));
    ui->label_valor3->setText(QString::number(valor[2], 'f', 4));

}

// Conversao e processamento dos dados
void MainWindow::convert_dados(std::vector<__u8> dados)
{

    uint16_t dado_conv;

    uint16_t dado_ant = 0; // Dado anterior para o filtro

    float alfa = this->ui->doubleSpinBox->value(); // Alfa do filtro
    inverter = this->ui->checkBox->checkState(); // MOD - Transformar inverter em variavel local

    this->x_data.clear();
    this->y_data.clear();

    for(int index=0; index<2*x_max; index+=2)
    {

        qInfo() << "Dado para conversao: " << index; // DEBUG
        qInfo() << (uint8_t) dados[index]; // DEBUG
        qInfo() << (uint8_t) dados[index+1]; // DEBUG

        dado_conv = (uint16_t)((dados[index] << 8) + (dados[index+1] & 0x00FF));

        qInfo() << "Dado convertido: " << (uint16_t) dado_conv; // DEBUG

        // Filtro
        if(index>0)
        {

            dado_conv = dado_ant + alfa * (dado_conv - dado_ant);

        }

        if(inverter==true){
            this->y_data.append(4096 - (double) dado_conv);
        }
        else{
            this->y_data.append((double) dado_conv);
        }
        this->x_data.append((double) index/2+1);


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

// Plot
void MainWindow::plot(){

    // Valores para os eixos
    int y_min = 0;
    int y_max = 4095 + 100;
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

    // Configuracao do plot
    ui->plot_widget->clearGraphs();
    ui->plot_widget->addGraph(); // Sinal / Parametro 1
    ui->plot_widget->addGraph(); // Parametro 2
    ui->plot_widget->addGraph(); // Parametro 3

    // TEST - OTIMIZAR - REFAZER A SELECAO DE GRAFICO
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

        // Criacao do vetor de indices - O
        for(int i=0; i<(int)param1_buffer.size(); i++){
            this->x_data_param.append(i);
        }

        // Plot parametro1 - O
        if(this->ui->checkBox_parametro1->isChecked()){

            for(int i=0; i<(int)param1_buffer.size(); i++){
                this->y_data_param1.append(param1_buffer[i]);
                if(this->y_data_param1[i] > param_max) param_max = y_data_param1[i];
                if(this->y_data_param1[i] < param_min) param_min = y_data_param1[i];
            }
            ui->plot_widget->graph(0)->setData(this->x_data_param, this->y_data_param1);
            ui->plot_widget->graph(0)->setPen(pen1);
        }

        // Plot parametro2 - O
        if(this->ui->checkBox_parametro2->isChecked()){

            for(int i=0; i<(int)param2_buffer.size(); i++){
                this->y_data_param2.append(param2_buffer[i]);
                if(this->y_data_param2[i] > param_max) param_max = y_data_param2[i];
                if(this->y_data_param2[i] < param_min) param_min = y_data_param2[i];
            }
            ui->plot_widget->graph(1)->setData(this->x_data_param, this->y_data_param2);
            ui->plot_widget->graph(1)->setPen(pen2);
        }

        // Plot parametro3
        if(this->ui->checkBox_parametro3->isChecked()){

            for(int i=0; i<(int)param3_buffer.size(); i++){
                this->y_data_param3.append(param3_buffer[i]);
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
    ui->plot_widget->graph(0)->setAntialiased(false);
    ui->plot_widget->graph(0)->setAdaptiveSampling(false);

    // Rotina de atualizacao do plot
    ui->plot_widget->replot();

}

// Fechar a porta serial
void MainWindow::serial_close(){
    timer.disconnect(&timer, SIGNAL(timeout()), this, SLOT(plot_update())); // Desativa o timer de atualizacao do grafico

    tcflush(mcu.native_handle(), 0); // Flush

    tcsendbreak(mcu.native_handle(), 0); // Send break serial
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Microcontrolador resetado"; // DEBUG 1 2

    mcu.close(); // Fecha a porta serial
    if(DEBUG_FLAG==1||DEBUG_FLAG==2) qInfo() << "Porta serial fechada"; // DEBUG 1 2

}
