#ifndef _nivel_1a_h_
#define _nivel_1a_h_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "nivel1.h"


typedef int (*tpfNotificacionDesdeNivel1a) (const BYTE *, int, const BYTE *, int, struct timeval *);

int InicializarNivel1a(int Tipo1, int Tipo2, tpfNotificacionDesdeNivel1a Funcion, int Timeout);

int FinalizarNivel1a(void);

int EnviarDatagramaNivel1a(const BYTE *Direccion_Destino, int Tamano, const BYTE *Mensaje, int Tipo);

int RecibirDatagramaNivel1a(BYTE *Direccion_Remitente, BYTE *Mensaje, int *Tipo, struct timeval* time);

int definirDireccionDestino(char *dir,BYTE *destino);

unsigned long CalculaCRC(BYTE *mensaje, unsigned longitud);

int NotificacionDesdeNivel1a(int tamano, const BYTE *datos, struct timeval *tiempo);



#endif
