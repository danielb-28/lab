#include <mbed.h>
#include <LPC17xx.h>

#define CTX p29
#define CRX p30
#define CAN_FREQ 1000000
#define CAN_MSG_TAMANHO 2
#define CAN_MSG_TAMANHO_RX 2

#define BAUD 460800

#define SMP_MAX 3000

CAN can(CRX, CTX, CAN_FREQ);

void SerialCallbackRX(void);

Serial pc(USBTX, USBRX);

CANMessage can_buffer, can_buffer_rx, can_buffer_rx_2;
char data_buffer[CAN_MSG_TAMANHO]; // 1 byte de rotulo
char data_buffer_rx[CAN_MSG_TAMANHO_RX];
char data_buffer_rx_2[CAN_MSG_TAMANHO_RX];

void CanCallbackRX(void);

uint8_t smp_buffer[SMP_MAX];
int smp_cnt = 0;

int main(){
    
    pc.baud(BAUD);
    
    pc.attach(SerialCallbackRX, Serial::RxIrq);
    
    can.attach(CanCallbackRX, CAN::RxIrq);
    
    can_buffer_rx = CANMessage(2, data_buffer_rx, CAN_MSG_TAMANHO_RX);
    
    //can_buffer_rx_2 = CANMessage(1, data_buffer_rx_2, CAN_MSG_TAMANHO_RX);
    
    /*
    data_buffer[0] = 0x1; 
    data_buffer[1] = 0x0;
    
    can_buffer = CANMessage(1, data_buffer, CAN_MSG_TAMANHO);
    
    can.write(can_buffer);
    */
    
    /*
    data_buffer[0] = pc.getc(); 
    data_buffer[1] = pc.getc();
    
    //data_buffer[0] = 0x1; 
    //data_buffer[1] = 0x0;
    
    //wait(5);
    
    can_buffer = CANMessage(1, data_buffer, CAN_MSG_TAMANHO);
    
    can.write(can_buffer);
    */
    while(true){
        wait_us(1);
    }
    
}

void SerialCallbackRX(void){
    
    //pc.puts("SERIAL RX");
    
    data_buffer[0] = pc.getc(); 
    data_buffer[1] = pc.getc();
    
    wait_us(100);
    
    can_buffer = CANMessage(1, data_buffer, CAN_MSG_TAMANHO);
    
    can.write(can_buffer);
    
    return;    
}

void CanCallbackRX(void){
    
    can.read(can_buffer_rx);
    /*
    for(int i=0; i < can_buffer_tx.len; i++){
        pc.putc(can_buffer_rx.data[i]);
    }
    */
    
    pc.putc(can_buffer_rx.data[0]);
    pc.putc(can_buffer_rx.data[1]);
    
    
    /*
    uint16_t label = (uint16_t)((can_buffer_rx.data[0] << 8) + (can_buffer_rx.data[1] & 0x00FF));
    
    uint16_t smp_n = label >> 4;
    
    //if((can_buffer_rx.data[1] & 0x0F) == 0x00) pc.putc("Dado");
    //if((can_buffer_rx.data[1] & 0x0F) == 0x01) pc.putc("Par");
    
    pc.putc(can_buffer_rx.data[0]);
    pc.putc(can_buffer_rx.data[1]);
    
    switch(label & 0x01){
        case 0x00:
        
            for(int i=0; i<smp_n; i++){
            
                can.read(can_buffer_rx_2);
            
                pc.putc(can_buffer_rx_2.data[0]);
                pc.putc(can_buffer_rx_2.data[1]);
                
            }
            
            break;
        
        case 0x01:
            
            can.read(can_buffer_rx_2);
            
            pc.putc(can_buffer_rx_2.data[0]);
            pc.putc(can_buffer_rx_2.data[1]);
            
            break;    
    }
    */
    /*
    data_buffer[0] = 0x06;
    data_buffer[1] = 0x00;
    
    can_buffer = CANMessage(1, data_buffer, CAN_MSG_TAMANHO);
    
    can.write(can_buffer);
    */
    //pc.printf("\n\r%d - %d", can_buffer_rx.data[0], can_buffer_rx.data[1]);
        
    return;    
}