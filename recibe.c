/* 
 * File:   recibe.c
 * Author: thiago
 *
 * Created on 6 de Março de 2013, 21:15
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "nivel1a.c"

#define MAX (ETH_DATA_MAX - 46)

//0 = Trama Valida; -1 Trama no valida; -10 = Variable Global (no avaliada por la funcion de notificacion)
int TramaValida = -10; 
char * canal;


/**
 * 
 * @param Direccion_Remitente
 * @param Tamano
 * @param Mensaje
 * @param Tipo
 * @param time
 * @return 0 = OK | <> 0 = Problema
 */
int funcionNotificacionAplicacion(const BYTE *Direccion_Remitente , int Tamano, const BYTE *Mensaje, int Tipo, struct timeval * time){
    char canalRecibido[20];

    if(Tipo != 2){
        TramaValida = -1;
        return -1;
    }

    memcpy(canalRecibido, Mensaje, 20 * sizeof(char));

    if(strcmp(canalRecibido, canal) == 0){
        // Não recebeu trama
        TramaValida = 0;
    } else {
        TramaValida = -1;
        return -1;
    }
    return 0;
}

/**
 * Programa que recebe dados de uma Trama.
 * @param argc
 * @param argv
 * @return Não importa
 */
int main(int argc, char *argv[]){
    BYTE direccionRemitente[ETH_ALEN];
    BYTE Mensaje[ETH_DATA_MAX];
    int tipo, count, codigo;

    struct timeval timeval, time_start_chat;
    gettimeofday(&time_start_chat, NULL);
    // O tempo deve estar em Segundos
    double seconds_start = time_start_chat.tv_sec + (time_start_chat.tv_usec / 1000000.0);

    if(argc != 2){
        printf("Usted debe informar los argumentos siguientes:\n"
                "- Nombre del Canal.\n");
        return -1;
    }

    canal = argv[1];

    if(strlen(canal) > 20){
        printf("Compruebe porfavor:\n"
                "- Nombre del Canal debe ser inferior a 20 caracteres.\n");
        return -1;
    }

    
    InicializarNivel1a(0, 1, funcionNotificacionAplicacion, 0);

    while(1){
        // 0 = Não receber nova Trama; -1 Pronto para receber nova Trama
        codigo = EsperaNuevaTrama(0);

        if(TramaValida == 0){
            /**
             * A função recebe da rede uma Trama completa.
             * @param *Direccion_Remitente BYTE - 
             * @param *Mensaje BYTE - 
             * @param *Tipo int -
             * @param time timeval* -
             * @return int
             */
            codigo = RecibirDatagramaNivel1a(direccionRemitente, Mensaje, &tipo, &timeval);
            short int longitudTexto;
            
            memcpy(&longitudTexto, Mensaje + 40 * sizeof(char), sizeof(short int));
            char persona[20];
            char mensajeTexto[MAX];

            memcpy(persona, Mensaje + 20 * sizeof(char), 20 * sizeof(char));
            memcpy(mensajeTexto, Mensaje + 42 * sizeof(char), longitudTexto);
            mensajeTexto[longitudTexto] = '\0';

            /**
             * Tiempo desde que se inicio el chat (para obtener el momento de inicialización del chat.
             * Se puede utilizar la función gettimeofday)
             */
            gettimeofday(&time_start_chat, NULL);
            double seconds = time_start_chat.tv_sec+(time_start_chat.tv_usec/1000000.0);

            printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - ");
            printf("\n- Persona.......: %s", persona);
            printf("\n- MAC...........: ");
            for (count = 0; count < ETH_ALEN; count++){
                // Verifica se tem os ":"
                if(count == (ETH_ALEN - 1))
                    printf("%02X", (unsigned char)direccionRemitente[count]);
                else
                    printf("%02X:", (unsigned char)direccionRemitente[count]);
            }
            printf("\n- Tiempo (s)....: %.2lf", (seconds - seconds_start));
            printf("\n- Mensaje.......: %s", mensajeTexto);
            printf("\n- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");

        } else {
            // Não será uma trama válida
            codigo = RecibirDatagramaNivel1a(direccionRemitente, Mensaje, &tipo, &timeval);
        }
        // Já pode receber uma nova trama
        TramaValida = -1;
    }
    
    /**
     * Chamar somente uma vez ao finalizar o programa.
     * @return ETH_OK = Operação Correta || ETH_ERROR = Falha na operação
     */ 
    FinalizarNivel1a();
}
