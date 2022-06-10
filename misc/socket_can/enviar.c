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

	int fd; // file_desc
	struct ifreq can_ifreq; // ifreq
	struct can_frame dado; // data frame
	struct sockaddr_can can_addr; // endereco

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

	// Envio dos dados
	dado.can_id = 0x002;
	dado.can_dlc = 2;
	char str_dados[2] = {0x41, 0x42};
	sprintf(dado.data, "%s", str_dados);

	if (write(fd, &dado, sizeof(struct can_frame)) != sizeof(struct can_frame)) {
		perror("Erro no envio dos dados");
		return 1;
	}
	puts("Dados enviados");

	// Fecha o socket
	if (close(fd) < 0) {
		perror("Erro ao fechar o socket");
		return 1;
	}

	return 0;
}
