//
// Created by saint on 21/02/2018.
//

#include<iostream>


#ifndef TAREAED_REGISTER_H
#define TAREAED_REGISTER_H

struct stRegistro {
    char valido;  // Campo que indica si el registro es valido S->Válido, N->Inválido
    char nombre[34];
    char apellido[2][34];
    char telefono[10];
};

struct stIndice {
    char telefono[10];
    long indice;
};

int Menu();
void Capturar(stRegistro &reg);
void EliminarRetornoLinea(char *cad);
void Leer(FILE *fa, stRegistro &reg, char *telefono);
void Insertar(FILE *fa, stRegistro &reg);
void Mostrar(stRegistro &reg);
void ListarPorTelefonos(FILE *fa);
void ListarNatural(FILE *fa);
void ReconstruirIndices(FILE *fa);
// Funciones para ordenar el fichero de índices:
void Intercambia(FILE *fa, long iz, long de);
char *LeeCampo(FILE *fa, long n, char *buf);
void QuickSort(FILE *fa, long inicio, long final);


#endif //TAREAED_REGISTER_H
