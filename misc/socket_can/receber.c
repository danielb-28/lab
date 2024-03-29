#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <string.h>

#define CAN_NAME "can1"

int main(int argc, char **argv){

	if(argc != 2){
	       puts("Numero invalido de argumentos\n\rFromato: numero_pacotes");
       		return 1;
 	}		

	int n_pacotes = atoi(argv[1]);

	if(n_pacotes < 1){
		puts("Numero invalido de pacotes");
		return 1;
	}

	int fd; // file_desc
	struct ifreq can_ifreq; // ifreq
	struct can_frame dado; // data frame
	struct sockaddr_can can_addr; // endereco

	int bytes_recebidos;

	// file_desc
	if ((fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) // Retorna o file_desc 
	{
		perror("Erro na criacao do socket");
		return 1;
	}
	puts("Socket criado");

	// ifr_name
	strcpy(can_ifreq.ifr_name, CAN_NAME);

	// indice da interface
	ioctl(fd, SIOCGIFINDEX, &can_ifreq); // retorna o indice em can_ifreq.ifr_ifindex
	
	// addr
	memset(&can_addr, 0, sizeof(can_addr));
	can_addr.can_family = AF_CAN;
	can_addr.can_ifindex = can_ifreq.ifr_ifindex;

	// bind
	if (bind(fd, (struct sockaddr *)&can_addr, sizeof(can_addr)) < 0) {
		perror("Erro no bind");
		return 1;
	}
	puts("Bind feito");

	// Recebimento dos dados
	puts("Aguardando dados...");
	int cnt = 0;
	while(cnt < n_pacotes){
		bytes_recebidos = read(fd, &dado, sizeof(struct can_frame));
	 	if (bytes_recebidos < 0) {
			perror("Erro no recebimento");
			return 1;
		}
	
		printf("0x%03X [%d] ", dado.can_id, dado.can_dlc);
	
		for (int i = 0; i < dado.can_dlc; i++)
			printf("%02X ", dado.data[i]);
		
		puts("\n\r");
		cnt++;
	}

	// Fecha o socket
	if (close(fd) < 0) {
		perror("Erro ao fechar o socket");
		return 1;
	}

	return 0;
}
