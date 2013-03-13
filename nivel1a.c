#include "nivel1a.h"

BYTE DireccionEstacion[ETH_ALEN];
int Nivel1aIniciado = -1;
tpfNotificacionDesdeNivel1a funcionNotificacion;

int InicializarNivel1a(int Tipo1, int Tipo2, tpfNotificacionDesdeNivel1a Funcion, int Timeout){
    int codigo; //retorno de las funciones...
    int nEtherType;
    BYTE EtherType[ETH_TLEN*2];

    if(Tipo1 == 1 && Tipo2 == 0){ //recepción de tramas solo del tipo 1
        nEtherType = 1;
        EtherType[0]=0x20;
        EtherType[1]=0x98;
    }else if(Tipo1 == 0 && Tipo2 == 1){ //recepción de tramas solo del tipo 2
        nEtherType = 1;
        EtherType[0]=0x84;
        EtherType[1]=0x3F;

    }else if(Tipo1 == 1 && Tipo2 == 1){
        nEtherType = 2;     //recepción de tramas solo del tipo 1 y 2
        EtherType[0]=0x20;
        EtherType[1]=0x98;
        EtherType[2]=0x84;
        EtherType[3]=0x3F;
    }else
        nEtherType = 0; //ninguma trama a recebir

    //Inicia el nivel inferior de la interfaz (nivel 1) --tipos de trama que pode receber: 1 ou 2
    codigo = IniciarNivel1(nEtherType,EtherType,NotificacionDesdeNivel1a,0);

    if(codigo == ETH_ERROR){
        printf("\nError iniciar nivel1...\n");
        return ETH_ERROR;
    }

    //Determina la dirección de la estación en la que estamos trabajando --armazenar isso em uma var
    codigo = ObtenerDirMAC(DireccionEstacion);

    if(codigo == ETH_ERROR){
        printf("\nError obtener nuestra direccion MAC...\n");
        return ETH_ERROR;
    }

    //Activa una variable global que indique que el nivel 1a ha sido inicializado correctamente
    Nivel1aIniciado = 1;
    funcionNotificacion = Funcion;
    return ETH_OK;
}

int FinalizarNivel1a(void){
    //Esta función sólo llamará a la función FinalizarNivel1 si Nivel1aIniciado es igual a 1
    int codigo;

    if(Nivel1aIniciado == 1){
        codigo = FinalizarNivel1();
        if(codigo == ETH_OK){
            Nivel1aIniciado = 0;
            return ETH_OK;
        }
    }

    printf("Error en nivel 1a...\n");
    return ETH_ERROR;
}

int EnviarDatagramaNivel1a(const BYTE *Direccion_Destino, int Tamano, const BYTE *Mensaje, int Tipo){
    int codigo;
    BYTE trama[ETH_FRAME_MAX];
    int tamanoTrama = 0;
    BYTE EtherType[ETH_TLEN];
    int i;

    if(Nivel1aIniciado != 1){
        printf("\nNivel 1a no inicia...\n");
        return ETH_ERROR;
    }

    memcpy(trama,Direccion_Destino,ETH_ALEN*sizeof(BYTE));
    tamanoTrama += ETH_ALEN;
    memcpy(trama+tamanoTrama,DireccionEstacion,ETH_ALEN*sizeof(BYTE));
    tamanoTrama += ETH_ALEN;

    if(Tipo == 1){
        EtherType[0]=0x20;
        EtherType[1]=0x98;
    }else if(Tipo == 2){
        EtherType[0]=0x84;
        EtherType[1]=0x3F;
    }else{
        printf("\nTipo no conocido...");
        return ETH_ERROR;
    }

    memcpy(trama+tamanoTrama,EtherType,ETH_TLEN*sizeof(BYTE));
    tamanoTrama += ETH_TLEN;
    memcpy(trama+tamanoTrama,Mensaje,Tamano);
    tamanoTrama += Tamano;

    unsigned long CRC = CalculaCRC(trama,Tamano);

    memcpy(trama+tamanoTrama,&CRC,sizeof(unsigned long));
    tamanoTrama += sizeof(unsigned long);

    //trama menor que el minimo?
    if (tamanoTrama < ETH_FRAME_MIN)
    {
      for (i=0; i < ETH_FRAME_MIN-tamanoTrama; i++)
        trama[tamanoTrama+i]= '\0';
      tamanoTrama = ETH_FRAME_MIN;
    }

    codigo = EnviarTramaNivel1(tamanoTrama, trama);

    if(codigo == ETH_OK){
        return ETH_OK;
    }else{
        printf("\nErro enviar trama al nivel1...");
    }

    return ETH_ERROR;
}

int RecibirDatagramaNivel1a(BYTE *Direccion_Remitente, BYTE *Mensaje, int *Tipo, struct timeval* time){
    if(Nivel1aIniciado != 1){
        printf("\nNivel 1a no iniciado... no se puede recibir!!\n");
        return ETH_OK;
    }

    BYTE buffer[ETH_DATA_MAX];
    BYTE EtherType[ETH_TLEN];
    int tipo_trama;
    int codigo;
    int tamanoMensaje;

    codigo = RecibirTramaNivel1 (buffer, time);

    if(codigo <= 0){
        return codigo;
    }

    tamanoMensaje = codigo - ETH_HLEN - 4; //4 ->> CRC enviado por el nivel1a

    memcpy(Direccion_Remitente,buffer+ETH_ALEN,ETH_ALEN*sizeof(BYTE));
    memcpy(Mensaje,buffer+ETH_HLEN,tamanoMensaje*sizeof(BYTE));

    //Tipo
    memcpy(EtherType,buffer+2*ETH_ALEN,ETH_ALEN*sizeof(BYTE));

    if(EtherType[0] == 0x20)
        *Tipo = 1;
    else if(EtherType[0] == 0x84)
        *Tipo = 2;
    else *Tipo == -1; //no conocido...

    return tamanoMensaje;

}

int definirDireccionDestino(char *dir,BYTE *destino) //basado en el rc_funcs.c
{
      int  i;
      char *tail;

      if ((dir == NULL) || (strlen(dir) != 17))
      {
        printf("Dirección destino no valida.\n");
        return(-1);
      }


      /* Comprobar que la variable dir apunta a una dirección ethernet correcta*/
      for (i=0; i<17; i++)
      {
            if ((i+1)%3 == 0)
            {
              /* En esta posición debe haber ':'*/
              if (dir[i]!=':')
              {
                printf("Direccion de destino es incorrecta. Componentes no separados por : en %d\n", i);
                return(-1);
              }
            }
            else
            {
              /* En esta posición debe haber un dígito hexadecimal */
              if (NULL == strchr("0123456789abcdefABCDEF", dir[i]))
              {
                printf("Direccion de destino DIR_DESTINO incorrecta. Dígito no hexadecimal en posición %d.\n", i);
                return(-1);
              }
            }
        }

                  /* Parse de la dirección destino. */
        for (i=0; i<6; i++)
          {
            for (; *dir == ':'; dir++);     	 /* Salta los ':'                */
            destino[i] = strtol(dir, &tail, 16); /* Convierte un byte            */
            if (dir == tail)
            {
              /* Error en la conversión */
              printf("Error en la obtencion de la dirección destino.\n");
              return(-1);
            }
            dir = tail; 	   		/* Apuntar al siguiente carácter */
          }


}


unsigned long CalculaCRC(BYTE *mensaje, unsigned longitud)
{
   /* polinomio generador */
   char polinomio[] = {1,0,0,0,0,1,0,0,0,0,0,0,1,0,0,0};

   /* variables */
   unsigned long resultado = -1;
   char ctl;
   int i,j,k;

   /* bucle principal para el calculo del CRC */
   for(i = 0; i < longitud; i++)
   {
      for(j = 0; j < 8; j++)
      {
	 ctl = (resultado & 1) ^ ((mensaje[i] >> j) & 1);
	 resultado = (resultado & ~1) >> 1;

	 for(k = 0; k < sizeof(polinomio); k++)
	 {
           if(ctl && polinomio[sizeof(polinomio) - 1 - k])
	      resultado ^= (unsigned long) 1 << k;
	 }
      }
   }

   /* pone a cero los bits del resultado no ocupados por el CRC          */
   for(i = sizeof(polinomio); i < sizeof(resultado) * 8; i++)
	  resultado |= (unsigned long) 1 << i;

   /* devuelve el CRC de la secuencia                                    */
   return ~resultado;
}



int NotificacionDesdeNivel1a(int tamano, const BYTE *datos, struct timeval *tiempo)
{
  /* Comprobar tamaño de trama recibida. */
  if(tamano == 0)
  {
    /* Llamada por timer.                                                  */
    printf("Nivel1a: LLamada a función de notificación por timer\n");
    return(ETH_OK);
  }
  else
  {
    /* Recibida trama. Se debe decidir si la trama se debe recibir o no.   */
    /* Aquí se debe introducir el código para ver si se acepta o no.       */
    /* En caso de aceptación, retorna 0                                    */

    /* *****************   MIRANDO CRC         *****************/
    unsigned long CRC = CalculaCRC((BYTE *)datos,(unsigned)tamano);
    unsigned long CRC_mensaje;

    memcpy(&CRC_mensaje,datos+(tamano-4),sizeof(unsigned long));

    //if(CRC != CRC_mensaje)
      //  return -1;
    //}else
    //    printf("\nCRC OK\n");

   /*if(CRC != 0)
	return -1;*/


    BYTE Direccion_Origem[ETH_ALEN];
    BYTE EtherType[ETH_TLEN];
    BYTE Mensaje[ETH_DATA_MAX-4]; //-4 ->> CRC -> Mensage + CRC;
    int tamanoMensage;
    int i;

    memcpy(Direccion_Origem,datos+ETH_ALEN,ETH_ALEN*sizeof(BYTE));
    memcpy(EtherType,datos+(2*ETH_ALEN),ETH_TLEN*sizeof(BYTE));

    int Tipo;
    if(EtherType[0] == 0x20)
        Tipo = 1;
    else if(EtherType[0] == 0x84)
        Tipo = 2;
    else Tipo == -1; //no conocido...


    tamanoMensage = tamano - ETH_HLEN - 4; //4 -->> Tamanño campo CRC

    memcpy(Mensaje,datos+ETH_HLEN,tamanoMensage);

    funcionNotificacion(Direccion_Origem,tamanoMensage,Mensaje,Tipo,0);
    //funcionNotificacion


    return(ETH_OK);
    /* En caso de no aceptación, debe retornar ETH_ERROR                   */
    /* return(ETH_ERROR); */
  }
}











