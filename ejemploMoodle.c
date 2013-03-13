#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "nivel1.h"

#define MAX 256

BYTE EtherType[ETH_TLEN*2]; // Memoria para 2 Ethertypes, de longitud ETH_TLEN int nEtherType, rc;
int Notificacion (int longitud, const BYTE * trama) { 
    // Proceso a realizar
    // Notifica sempre que receber uma Trama
}

/*
int gestiona_trama_ethernet(int tamano, const BYTE *trama) {
        int gestiona_trama_ethernet(int tamano, const BYTE *trama,struct timeval *tv) {
                BYTE buffer[ETH_FRAME_MAX];

                if (RecibirTramaNivel1(buffer) != tamano) {
                        if (RecibirTramaNivel1(buffer,NULL) != tamano) {
                                fprintf(stderr, "Error: fallo en nivel1 recibiendo trama\n");
                                return -1;
                        }
                        return 0;
                }
        }
}
 */

int main(int argc, char** argv) {
    char buffer[MAX];
    int rc;
    // Preparar array EtherType para recibir tramas de tipos 2098 y 843F EtherType[0]=0x20; EtherType[1]=0x98; EtherType[2]=0x84; EtherType[3]=0x3F; nEtherType=2;
    // Se recibirán 2 Ethertypes distintos
    // Llamar a la función de iniciación. rc = IniciarNivel1(nEtherType,EtherType, Notificacion, 0);
    
    //rc = IniciarNivel1( int num_types, const BYTE *ether_types,tpfNotificacionDesdeNivel1 func_notificacion, int timer_intr);
    
    
    /**
     * A função envia a rede uma Trama completa.
     * @param len = tamanho da trama
     * @param *data = Puntero a trama que se quer enviar
     */
    int EnviarTramaNivel1(int len, const BYTE *data);
    
    
    /**
     * Copia o buffer passado como parametro uma trama recebina por Nivel 1.
     * @param *buffer = ponteiro para onde deseja Level1 copie a trama recebida
     * @param *timestamp = ponteiro onde Level1 copie a marca temporal de chegada da trama
     * @return 0 = Não recebeu trama || ETH_ERROR = Falha na operação || >0 = Tamanho da trama recebida
     */
    int RecibirTramaNivel1 (BYTE *buffer, struct timeval *timestamp);
    
    
    /**
     * Espera uma nova trama para Leitura.s
     * @param t = tempo maximo que se realiza a espera (milisegundos)
     * @return ETH_OK = Operação correta || ETH_ERROR = Trama recebida mas n adimitida || ETH_TIMEOUT = Saída por timeout
     */
    int EsperaNuevaTrama (int t);
    
    
    /**
     * Chamar somente uma vez ao finalizar o programa.
     * @return ETH_OK = Operação Correta || ETH_ERROR = Falha na operação
     */ 
    int FinalizarNivel1(void);

    return (EXIT_SUCCESS);
}

