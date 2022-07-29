#include "mbed.h"
#include <LPC17xx.h>
#include "MODDMA.h"
#include "MODSERIAL.h" 

// CONSTANTES
#define N_PAR 3 // Numero de parametros monitorados
#define PARAM_CNT 10 //  Numero de varreduras para cada aquisicao de parametros
#define MAX_SMP 2000 // Numero maximo de amostras

// CONSTANTES SPI
#define SPI_MOSI p5
#define SPI_MISO p6
#define SPI_SCLK p7

#define SPI_CS0 p8
#define SPI_CS1 p9

// CONSTANTES DAC
#define PPC 1024 // Pontos por ciclo
#define BIAS 1 // Power Mode

// CONSTANTES SERIAL
#define BAUD 460800

// CONSTANTES CAN
#define CTX p29
#define CRX p30

#define CAN_FREQ 1000000
#define CAN_MSG_TAMANHO 2 // MOD
#define CAN_MSG_TAMANHO_TX 2 // MOD

// CONSTANTES LOCK
#define P_LOCK1 p26 // TEST - verificar pinos
#define P_LOCK2 p25 // TEST - verificar pinos

// LOCKs
DigitalOut lock1(P_LOCK1);
DigitalOut lock2(P_LOCK2);

// LEDs
DigitalOut led1(LED1); // Amostragem sinal
DigitalOut led2(LED2); // Rotina iniciada
DigitalOut led3(LED3); // Amostragem parametros
DigitalOut led4(LED4); // Erro DMA

// SPI
DigitalOut cs0(SPI_CS0); // Cs Pin 0
DigitalOut cs1(SPI_CS1); // Cs Pin 1
SPI spi(SPI_MOSI, SPI_MISO, SPI_SCLK); // mosi, miso, sclk


uint8_t spi_val[3]; // Valores para os potenciometros

// PULLDOWN ADC
DigitalOut a5(A5);

// SETUP DAC
uint32_t buffer_dac[2][PPC]; // Buffer dac
AnalogOut aout(p18); // Pino de Saida dac

// DMA
MODDMA dma;
MODDMA_Config *conf_adc, *conf_adc2, *conf_dac1, *conf_dac2; // Configuracoes

// VARIAVEL DE CONTROLE DMA 
bool dma_completo = false;

// VARIAVEIS SERIAL
Serial pc(USBTX, USBRX); // Porta serial
uint8_t serial_fixed[2]; // Salva o comando inicial de controle serial
uint16_t label = 0; // Label do pacote de dados
char serial_str[5]; // Comando de controle serial

// VARIAVEIS ADC
uint16_t smp = 0; // Numero de amostras
bool adc_completo = true; // Ciclo de amostragem e envio completo
bool gui_completo = false;
volatile uint8_t read_cnt = 0; // Contagem para a leitura de parametros
uint32_t param[N_PAR]; // Vetor de parametros
bool adc_param_pendente = false; // Controle do envio dos parametros
uint16_t smp_enviadas; // Quantidade de amostras que serao enviadas
bool env_sync = true;

// VARIAVEIS CAN
CAN can(CRX, CTX, CAN_FREQ);
CANMessage can_buffer_rx;
CANMessage can_buffer_tx;
char can_data_buffer_tx[CAN_MSG_TAMANHO_TX];
char can_smp_buffer[2*MAX_SMP]; // TEST
char can_param_buffer[2*N_PAR]; // TEST

// FUNCOES
void dma_adc_callback(void); // Completo dma adc sinal
void dma_adc_param_callback(void); // Completo dma adc parametro
void dma_dac1_callback(void); // Completo dma dac 1
void dma_dac2_callback(void); // Completo dma dac 2
void dma_erro(void);

void serialrx_callback(); // Serial recebido

void adc_setup(uint8_t); // Configuracao adc
void dac_setup(uint8_t);  // Configuracao dac
void dma_setup(uint32_t*); // Configuracao dma

void spi_setup(uint8_t bits, uint8_t mode, uint32_t freq); // Configuracao spi
void spi_pot(uint8_t id, uint8_t value); // Potenciometro spi

void adc_reset(void); // Reconfigura o adc

void canrx_callback(void); // Can recebido

int cont_global = 0;

int main() {
    
    // CONFIGURACAO SERIAL
    pc.baud(BAUD);
    pc.attach(serialrx_callback, Serial::RxIrq);
    //pc.rxBufferSetSize(4); // bytes
    
    // CONFIGURACAO SPI
    spi_setup(16, 0, 1000000); // bits, modo, freq // TEST
    
    // CONFIGURACAO CAN
    can.attach(canrx_callback, CAN::RxIrq);
    can_buffer_rx = CANMessage(1, serial_str, CAN_MSG_TAMANHO);
    //can_buffer_tx = CANMessage(1, can_data_buffer_tx, CAN_MSG_TAMANHO_TX); // TEST
    
    // CONFIGURACAO LOCKS
    lock1 = false;
    lock2 = false;
    
    // COMANDO INICIO
    while(1){
        if((serial_str[0] & 0x03) == 0x01) break;
        wait_us(0.001);   
    }
    
    serial_fixed[0] = (uint8_t) serial_str[0]; // Armazena a configuracao serial
    serial_fixed[1] = (uint8_t) serial_str[1]; // Armazana a configuracao serial
    
    led2 = true; // LED - Inicio
    
    dac_setup(serial_fixed[1]); // Configura o DAC
    
    adc_setup(serial_fixed[1]); // Configura o ADC
    
    uint32_t v[smp]; // Vetor de amostras - armazena o ADDR inteiro
    
    dma_setup(v); // Configura o DMA 
    
    led1 = true; // LED - Amostragem sinal
    
    smp_enviadas = smp / ((uint16_t) ((serial_fixed[0] >> 4) & 0x0F) + 1); // Define a quantidade de amostras que serao enviadas
    
    // Inicia o DAC
    //LPC_DAC->DACCTRL |= (3UL << 2); // Set DMA_ENA e CNT_ENA
    
    // Inicia o ADC
    LPC_ADC->ADCR |= (1UL << 16); // Ativa o ADC no modo burst
        
    // Main loop
    while(1){
        pc.puts("---\n\r");
        wait_ms(5);
            // Envio dos dados
            if (dma_completo) {
                pc.puts("Envio dos dados\n\r");
                
                dma_completo = false; // Limpa a flag de dma completo
                serial_str[0] &= ~0x03; // Limpa o trigger serial
                
                // Envio do Label
                label = (smp_enviadas << 4); // Label 
                
                if(adc_param_pendente) label |= 0x01; // Label parametros     
                
                //pc.putc((label >> 8) & 0xFF); // Envio label - Serial
                //pc.putc(label & 0xFF); // Envio label - Serial  
                
                can_data_buffer_tx[0] = (char) ((label >> 8) & 0xFF); // Envio label - CAN
                can_data_buffer_tx[1] = (char) (label & 0xFF); // Envio label - CAN
                
                can_buffer_tx = CANMessage(2, can_data_buffer_tx, CAN_MSG_TAMANHO_TX); // Envío label - CAN
                
                can.write(can_buffer_tx); // Envio label - CAN
                
                pc.printf("param pendente %d\n\r", adc_param_pendente); // DEBUG
                
                // Envio dos Parametros
                if(adc_param_pendente){
                    
                    led1 = !led1; // LED - Amostragem Sinal
                    
                    //for (int i=0; i<N_PAR; i++) {
                    int param_index =0;
                    for (int i=0; i<2*N_PAR; i+=2) {
                        
                        //pc.putc((param[i] >> 12) & 0xF); // Envio parametros - Serial
                        //pc.putc((param[i] >> 4) & 0xFF); // Envio parametros - Serial
                        
                        can_param_buffer[i] = (char) ((param[param_index] >> 12) & 0xF); // Envio parametros - CAN
                        can_param_buffer[i+1] = (char) ((param[param_index] >> 4) & 0xFF); // Envio parametros - CAN
                        param_index++;
                        
                    }
                    
                    can_buffer_tx = CANMessage(2, can_param_buffer, 2*N_PAR); // Envio parametros - CAN
                    int suc = 0;
                    while(!suc){
                        suc = can.write(can_buffer_tx); // Envio parametros - CAN
                        pc.printf("parametros enviados - %d - %d\r\n", suc, cont_global); // DEBUG
                        wait_ms(1);
                    }
                    adc_param_pendente = false;
                    
                }
                
                // Envio dos Dados
                //for (int i = 0; i < smp; i+=smp/smp_enviadas) {
                int j = 0;
                //for (int i = 0; i < 2*smp; i++) {
                for (int i = 0; i < 2*smp; i+=2) {
                    //pc.putc((v[i] >> 12) & 0xF); // Envio Serial
                    //pc.putc((v[i] >> 4) & 0xFF); // Envio Serial
                    
                    //can_data_buffer_tx[0] = ((v[i] >> 12) & 0xF); // Envio CAN
                    //can_data_buffer_tx[1] = ((v[i] >> 4) & 0xFF); // Envio CAN
                    /*
                    if(i < 50/2){
                        can_data_buffer_tx[0] = 0x07; // Envio CAN
                        can_data_buffer_tx[1] = 0xD0; // Envio CAN
                    }
                    
                    else{
                        can_data_buffer_tx[0] = 0x07; // Envio CAN
                        can_data_buffer_tx[1] = 0x00; // Envio CAN
                    }
                    */
                    /*
                    if(!env_sync){
                        can_data_buffer_tx[0] = ((v[i] >> 12) & 0xF); // Envio CAN
                        can_data_buffer_tx[1] = ((v[i] >> 4) & 0xFF); // Envio CAN    
                    }
                    
                    else{
                        can_data_buffer_tx[0] = cont_global; // Envio CAN
                        can_data_buffer_tx[1] = (char) cont_global; // Envio CAN
                    }
                    
                    //if(smp <= 500) wait_us(50000/smp);
                    wait_ms(1);
                    can_buffer_tx = CANMessage(2, can_data_buffer_tx, CAN_MSG_TAMANHO_TX); // TEST
                    
                    int suc = 0;
                    while(suc==0){
                        suc = can.write(can_buffer_tx); // Envio CAN
                        wait_us(0.1);
                    }
                    
                    pc.printf("%d enviado - %d - %d\n\r", i, suc, cont_global);
                    */
                    
                    //can_smp_buffer[i] = cont_global;
                    can_smp_buffer[i] = ((v[j] >> 12) & 0xF);
                    can_smp_buffer[i+1] = ((v[j] >> 4) & 0xFF);
                    j++;
                    
                }
                
                for(int i=0; i < 2*smp; i+=5){
                    int suc = 0;
                    can_buffer_tx = CANMessage(2, &can_smp_buffer[i], 5);
                    suc = can.write(can_buffer_tx); // Envio CAN
                    if(!suc) {
                        i-=5;
                        wait_ms(1);
                    }
                    
                    /*
                    can_buffer_tx = CANMessage(2, &can_smp_buffer[i], 5);
                    int suc = 0;
                    while(suc==0){
                        suc = can.write(can_buffer_tx); // Envio CAN
                        if(!suc) can.reset();
                        wait_us(1);
                    }
                    */
                    //pc.printf("%d enviado - %d - %d - smp - %d\r\n", i, suc, cont_global, smp); // DEBUG
                }
                
                adc_completo = true;
                if (cont_global < 16) cont_global++; // DEBUG
                else cont_global = 0; // DEBUG
            }
            
            //if(adc_completo==true && gui_completo==true) adc_reset();
        }
}
 
// CONFIGURACAO ADC
void adc_setup(uint8_t adc_config){
     
    NVIC_DisableIRQ(ADC_IRQn); // Desabilita as interrupcoes do ADC no NVIC
     
    LPC_SC->PCONP    |=  (1UL << 12); // Liga o ADC
    LPC_SC->PCLKSEL0 &= ~(3UL << 24); // Limpa os bits 
    LPC_SC->PCLKSEL0 |= (3UL << 24); // 12 MHz
    LPC_ADC->ADCR  = (1UL << 21) | ((adc_config >> 5) << 8) | (1UL << 0); // Enable, Clock, Canal 0
    
    LPC_ADC->ADINTEN = 0x100; // Habilita a flag irq para o DMA
    
    LPC_PINCON->PINSEL1 |=  (1UL << 14); // Seleciona o pino A0 
    LPC_PINCON->PINSEL1 |=  (1UL << 16); // Seleciona o pino A1
    LPC_PINCON->PINSEL1 |=  (1UL << 18); // Seleciona o pino A2
    LPC_PINCON->PINSEL3 |=  (3UL << 28); // Seleciona o pino A4
    
    // Selecao do numero de amostras
    switch(adc_config & 0x07){ // Seleciona o tamanho do vetor de amostras
        
        case 0b000:
            smp = 50;
            break;
        
        case 0b001:
            smp = 100;
            break;
    
        case 0b010:
            smp = 250;
            break;
        
        case 0b011:
            smp = 500;
            break;
        
        case 0b100:
            smp = 1000;
            break;
            
        case 0b101:
            smp = 2000;
            break;
            
        case 0b110:
            smp = 3000;
            break;
            
        case 0b111:
            smp = 4000;
            break;
            
    }

    return;
} 


// CONFIGURACAO DAC
void dac_setup(uint8_t dac_config)
{
    
    // Criacao do buffer para o DAC
    if((dac_config >> 3) & 0x01){
        
        // Dente de serra
        for (int i=0; i<PPC; i++) buffer_dac[0][i] = (1024/PPC) * i; 
        
    }
    else {
        for (int i=0; i<PPC/2; i++) // Triangular
        {
            buffer_dac[0][i] = (1024/(PPC/2)) * i;
            buffer_dac[0][PPC-1-i] = buffer_dac[0][i];
        }
    }
    
    // Criacao do registrador DACR
    for (int i = 0; i < PPC; i++) 
    {
        buffer_dac[0][i] = (BIAS << 16) | ((buffer_dac[0][i] << 6) & 0xFFC0); // Bias 1; Value
        buffer_dac[1][i] = buffer_dac[0][i]; // copia
    }
    
    // Contador para atualizacao do DAC
    LPC_DAC->DACCNTVAL = 781; // 30 Hz
    
}

// CONFIGURACAO DMA
void dma_setup(uint32_t* adress_serial){
    
    // DMA - ADC
    conf_adc = new MODDMA_Config;
    conf_adc
     ->channelNum    (MODDMA::Channel_0)
     ->srcMemAddr    (0)
     ->dstMemAddr    ((uint32_t)adress_serial) // ALTERAR
     ->transferSize  (smp)
     ->transferType  (MODDMA::p2m)
     ->srcConn       (MODDMA::ADC)
     ->attach_tc     (&dma_adc_callback)
     ->attach_err    (&dma_erro)
    ; // end conf
    
    dma.Prepare(conf_adc); // Carrega a configuracao do ADC
    
    
    // DMA - ADC - 2 
    conf_adc2 = new MODDMA_Config;
    conf_adc2
     ->channelNum    (MODDMA::Channel_3)
     ->srcMemAddr    (0)
     ->dstMemAddr    ((uint32_t) param)
     ->transferSize  (N_PAR)
     ->transferType  (MODDMA::p2m)
     ->srcConn       (MODDMA::ADC)
     ->attach_tc     (&dma_adc_param_callback)
     ->attach_err    (&dma_erro)
    ; // end conf
    
    // DMA - DAC - 1
    conf_dac1 = new MODDMA_Config;
    conf_dac1
     ->channelNum    (MODDMA::Channel_2)
     ->srcMemAddr    ((uint32_t) &buffer_dac[0])
     ->dstMemAddr    (MODDMA::DAC)
     ->transferSize  (PPC)
     ->transferType  (MODDMA::m2p) 
     ->dstConn       (MODDMA::DAC)
     ->attach_tc     (&dma_dac1_callback)
     ->attach_err    (&dma_erro)    
    ;
    
    dma.Prepare(conf_dac1); // Carrega a configuracao do DAC
    
    // DMA - DAC - 2
    conf_dac2 = new MODDMA_Config;
    conf_dac2
     ->channelNum    (MODDMA::Channel_1)
     ->srcMemAddr    ((uint32_t) &buffer_dac[0])
     ->dstMemAddr    (MODDMA::DAC) 
     ->transferSize  (PPC)
     ->transferType  (MODDMA::m2p)
     ->dstConn       (MODDMA::DAC)
     ->attach_tc     (&dma_dac2_callback)
     ->attach_err    (&dma_erro)    
    ;
}  
 
// CALLBACK DMA ADC
void dma_adc_callback(void) {
        
    LPC_ADC->ADCR &= ~(1UL << 16); // Desativa o burst
    LPC_ADC->ADINTEN = 0; // Desativa as interrupcoes
    
    //dma.Disable((MODDMA::CHANNELS)conf_adc->channelNum());
    
    // Finaliza o DMA
    MODDMA_Config *config = dma.getConfig();
    dma.haltAndWaitChannelComplete( (MODDMA::CHANNELS)config->channelNum());
    dma.Disable( (MODDMA::CHANNELS)config->channelNum() );
    if (dma.irqType() == MODDMA::TcIrq) dma.clearTcIrq();    
    if (dma.irqType() == MODDMA::ErrIrq) dma.clearErrIrq();
    
    pc.puts("dma_completo - adc\n\r"); // DEBUG
    dma_completo = true;
    return;
    
}

// CALBACK ADC PARAMETROS
void dma_adc_param_callback(void){
    
    LPC_ADC->ADCR &= ~(1UL << 16); // Desativa o burst
    LPC_ADC->ADINTEN = 0; // Desativa as interrupcoes
    
    // Finaliza o DMA
    MODDMA_Config *config = dma.getConfig();
    dma.haltAndWaitChannelComplete( (MODDMA::CHANNELS)config->channelNum());
    dma.Disable( (MODDMA::CHANNELS)config->channelNum() );
    if (dma.irqType() == MODDMA::TcIrq) dma.clearTcIrq();    
    if (dma.irqType() == MODDMA::ErrIrq) dma.clearErrIrq();
    
    led3 = !led3; // LED - Amostragem parametros
    adc_param_pendente = true;
    adc_completo = true;
    dma_completo = true;
    pc.puts("adc_param_pendente\n\r"); // DEBUG
    return;    
}

// CALLBACK DMA DAC1
void dma_dac1_callback(void) { 
        
    // Finalizacao
    MODDMA_Config *config = dma.getConfig();
    dma.Disable((MODDMA::CHANNELS)config->channelNum());   
    pc.puts("dma_dac1 - completo\n\r"); // DEBUG
    
    if(adc_completo==true && gui_completo==true) adc_reset();
    
    if (dma.irqType() == MODDMA::TcIrq) dma.clearTcIrq();
    
    // Troca para o buffer[1]
    uint32_t err = dma.Prepare(conf_dac2); // ALTERAR
    pc.printf("dma_dac2 - iniciando - %d\n\r", err); // DEBUG

    return;
     
}

// CALLBACK DMA DAC2
void dma_dac2_callback(void) { 
        
    // Finalizacao
    MODDMA_Config *config = dma.getConfig();
    dma.Disable((MODDMA::CHANNELS)config->channelNum()); 
    
    pc.puts("dma_dac2 - completo\n\r"); // DEBUG  
   
    if(adc_completo==true && gui_completo==true) adc_reset();
    
    // Troca para o buffer[0] 
    uint32_t err = dma.Prepare(conf_dac1); // ALTERAR
    
    bool err2 = dma.isActive(MODDMA::Channel_2);
    
    pc.printf("dma_dac1 - iniciando - %d\n\r", err2); // DEBUG
    
    if (dma.irqType() == MODDMA::TcIrq) dma.clearTcIrq(); 
    
    return;
}
 
// CALLBACK ERRO DMA - FUNCAO DE ERRO PARA TODAS AS TRANSFERENCIAS
void dma_erro(void) {
    
    LPC_ADC->ADCR &= ~(1UL << 16); // Desativa o burst
    LPC_ADC->ADINTEN = 0; // Desativa as interrupcoes
    
    pc.puts("erro dma"); // DEBUG
    
    //led4 = true; // LED - Erro DMA // TEST

    return;
}

// CONFIGURACAO SPI
void spi_setup(uint8_t bits, uint8_t mode, uint32_t freq){
    spi.format(bits, mode);
    spi.frequency(freq); 
    cs0 = 1;
    cs1 = 1;  
    return;
}

// CONTROLE POTENCIOMETROS SPI
void spi_pot(uint8_t id, uint8_t value){
    switch(id){
        case 0:
            cs0 = 0;
        break;
        
        case 1:
            cs1 = 0;
        break;
    }
    
    spi.write(0x1300 | (uint16_t) value);
    
    switch(id){
        case 0:
            cs0 = 1;
        break;
        
        case 1:
            cs1 = 1;
        break;
    }
    
    return;        
}

// CALBACK SERIAL RX
void serialrx_callback(){
    
    return; // TEST - Desabilita o recebimento de comandos - Serial
    /*
    if (pc.rxBufferFull()){ // Recebe o comando duplicado
        serial_str[0] = pc.getc();
        serial_str[1] = pc.getc();
        serial_str[2] = pc.getc();
        serial_str[3] = pc.getc();
        
        if ( (serial_str[0] != serial_str[2]) || (serial_str[1] != serial_str[3]) ){ // Verifica se os dois comandos sao iguais
            
            serial_str[0] = 0;
            serial_str[1] = 0;
            serial_str[2] = 0;
            serial_str[3] = 0;
            
            led4 = true;
            
            return;
        }
        
        switch((((uint8_t)serial_str[0])&0x0F)){
        
            case 0x02:
                spi_val[0] = (uint8_t) serial_str[1];
                spi_pot(0, spi_val[0]); // pot 1
                break;
            
            case 0x03:
                spi_val[1] = (uint8_t) serial_str[1];
                spi_pot(1, spi_val[1]); // pot 2
                break;
            
            case 0x04:
                lock1 = (bool) (((uint8_t) serial_str[1])&0x01);
                break;
            
            case 0x05:
                lock2 = (bool) (((uint8_t) serial_str[1])&0x01);
                break;    
            
        }
    }
    
    return; */
} 

void adc_reset(void){
    
    LPC_ADC->ADCR &= ~0x7; // Limpa select
    LPC_ADC->ADINTEN = 0x100; // Habilita a flag irq para o DMA
    adc_completo = false;
    gui_completo = false;
    
    pc.puts("adc_reset\n\r"); // DEBUG

    if(read_cnt<PARAM_CNT) { 
        read_cnt++;
        LPC_ADC->ADCR  = (1UL << 21) | ((serial_fixed[1] >> 5) << 8) | (1UL << 0); // Enable, Clock, Canal 0
        dma.Prepare(conf_adc);
        pc.puts("adc_sinal_dma\n\r"); // DEBUG
    }

    else {
        read_cnt = 0;
        LPC_ADC->ADCR  = (1UL << 21) | ((serial_fixed[1] >> 5) << 8) | (22UL << 0); // Enable, Clock, Canal 1-2-4
        dma.Prepare(conf_adc2);
        pc.puts("adc_parametros_dma\n\r"); // DEBUG
    }

    LPC_ADC->ADCR |= (1UL << 16); // ATIVA O ADC

    return;    
}

void canrx_callback(void){
    
    can.read(can_buffer_rx); // Recebimento CAN
    
    serial_str[0] = can_buffer_rx.data[0]; 
    serial_str[1] = can_buffer_rx.data[1]; 
    
    switch((((uint8_t)serial_str[0])&0x0F)) {
        
        case 0x01:
            gui_completo = true;
            pc.puts("gui_completo\n\r");
            break;
            
        case 0x02:
            spi_val[0] = (uint8_t) serial_str[1];
            spi_pot(0, spi_val[0]); // pot 1
            break;

        case 0x03:
            spi_val[1] = (uint8_t) serial_str[1];
            spi_pot(1, spi_val[1]); // pot 2
            break;

        case 0x04:
            lock1 = (bool) (((uint8_t) serial_str[1])&0x01); // lock 1
            break;

        case 0x05:
            lock2 = (bool) (((uint8_t) serial_str[1])&0x01); // lock 2
            break;
        
    }

    
    return;
}
