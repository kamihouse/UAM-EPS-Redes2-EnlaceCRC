/* 
 * File:   envia.c
 * Author: thiago
 *
 * Created on 6 de Março de 2013, 16:15
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include "nivel1a.c"

#define MAX (256)

/**
 * Programa que envia dados por uma Trama.
 * @param argc
 * @param argv
 * @return Não importa
 */
int main(int argc,char *argv[]){
    char canal[20];
    char persona[20];
    short int longitudTexto;
    int longitudTrama;
    int count;
    BYTE trama[ETH_DATA_MAX-4]; // 4 CRC "nivel1a"

    /**
     * Verificando os dados informados pelo usuário.
     */
    if(argc != 3){
        printf("Usted debe informar los argumentos siguientes:\n"
                "- Nombre del Canal.\n"
                "- Su Nombre.\n");
        return -1;
    }
    if(strlen(canal) > 20){
        printf("Compruebe porfavor:\n"
                "- Nombre del Canal debe ser inferior a 20 caracteres.\n");
        return -1;
    }
    if(strlen(persona) > 20){
        printf("Compruebe porfavor:\n"
                "- Tu nombre debe ser inferior a 20 caracteres.\n");
        return -1;
    }

    // 20 bytes Canal, 20 bytes Persona, 2 bytes longitud dados
    longitudTrama = 42;

    strcpy(canal, argv[1]);
    strcpy(persona, argv[2]);

    /**
     * Verifica se o título do canal não ultrapassa 20 char.
     */
    for(count = strlen(canal); count < 20; count++)
        canal[count] = '\0';

    /**
     * Verifica se o título do canal não ultrapassa 20 char.
     */
    for(count = strlen(persona); count < 20; count++)
        persona[count] = '\0';

    /**
     * Adiciona os dados (Canal e Usuário) a Trama
     */
    memcpy(trama, canal, 20 * sizeof(char));
    memcpy(trama + 20 * sizeof(char), persona, 20 * sizeof(char));

    // (-1) = Não vamos receber tramas
    if(InicializarNivel1a(0,0,0,0) == -1){
        printf("\n****Error al iniciar 'nivel1a'\n");
        return -1;
    }

    char texto[MAX];
    BYTE direccionDestino[ETH_ALEN];
    definirDireccionDestino("FF:FF:FF:FF:FF:FF", direccionDestino);

    while(1){
        printf("- Canal: %s;\n- Escriba su mensaje %s : ", canal, persona);
        fgets(texto, MAX, stdin);
        
        // texto -1 para retirar o \n
        longitudTexto = strlen(texto)-1;
        texto[longitudTexto] = '\0';
        longitudTrama += longitudTexto;

        memcpy(trama + 40 * sizeof(char), &longitudTexto, sizeof(short int));
        memcpy(trama + 42 * sizeof(char) ,texto, longitudTexto * sizeof(char));

        /**
         * A função envia da rede uma Trama completa.
         * @param *Direccion_Remitente Byte - Direccion Remitente
         * @param *Mensaje BYTE - Mensaje
         * @param *tipo int - Tipo de la Trama
         * @param *timeval struct - Tiempo en segundos
         */
        EnviarDatagramaNivel1a(direccionDestino, longitudTrama, trama, 2);
        longitudTrama = 42;
    }
    return 0;
}
