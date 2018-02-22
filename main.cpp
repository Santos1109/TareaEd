#include <iostream>
#include <cstring>
#include "Register.h"

using namespace std;

int main() {

    stRegistro reg;
    FILE *fa;
    int opcion;
    char telefono[10];
    fa = fopen("indices.dat", "r+b");          // Este modo permite leer y escribir
    if(!fa) fa = fopen("indices.dat", "w+b");  // si el fichero no existe, lo crea.

    do {
        opcion = Menu();
        switch(opcion) {
            case '1': // Insertar registro
                Capturar(reg);
                Insertar(fa, reg);
                break;
            case '2': // Buscar registro
                printf("Buscar registro: ");
                do {
                    fgets(telefono, 10, stdin);
                    EliminarRetornoLinea(telefono);
                } while(strlen(telefono) < 1);
                Leer(fa, reg, telefono);
                Mostrar(reg);
                break;
            case '3': // Indicar archivo
                printf("Indicando archivo: ");
                ReconstruirIndices(fa);
                break;
            case '4': // Mostrar todo por orden de teléfonos
                ListarPorTelefonos(fa);
                break;
            case '5': // Mostrar todo por orden natural
                ListarNatural(fa);
                break;
        }
    } while(opcion != '0');
    fclose(fa);
    return 0;
}

// Muestra un menú con las opciones disponibles y captura una opción del usuario
int Menu()
{
    char resp[20];

    do {
        printf("MENU PRINCIPAL\n");
        printf("--------------\n\n");
        printf("1- Insertar registro\n");
        printf("2- Buscar registro\n");
        printf("3- Reindicar archivo\n");
        printf("4- Listar por orden de teléfonos\n");
        printf("5- Listar por orden natural\n");
        printf("0- Salir\n");
        fgets(resp, 20, stdin);
    } while(resp[0] < '0' && resp[0] > '5');
    return resp[0];
}

// Permite que el usuario introduzca un registro por pantalla
void Capturar(stRegistro &reg)
{
    int i;
    char numero[6];
    printf("Leer registro:\n\n");
    reg.valido = 'S';
    printf("Nombre: ");
    fgets(reg.nombre, 34, stdin);
    EliminarRetornoLinea(reg.nombre);
    printf("Primer apellido: ");
    fgets(reg.apellido[0], 34, stdin);
    EliminarRetornoLinea(reg.apellido[0]);
    printf("Segundo apellido: ");
    fgets(reg.apellido[1], 34, stdin);
    EliminarRetornoLinea(reg.apellido[1]);
    printf("Teléfono: ");
    fgets(reg.telefono, 10, stdin);
    EliminarRetornoLinea(reg.telefono);
}

// Elimina los caracteres de retorno de línea al final de cadena
void EliminarRetornoLinea(char *cad)
{
    int i;
    for(i = strlen(cad)-1; i >= 0 && cad[i] < ' '; i--) cad[i] = 0;
}

// Muestra un registro en pantalla, si no está marcado como borrado
void Mostrar(stRegistro &reg)
{
    int i;

    if(reg.valido == 'S') {
        printf("Nombre: %s %s %s\n", reg.nombre, reg.apellido[0], reg.apellido[1]);
        printf("Número de teléfono: %s\n", reg.telefono);
    }

}

// Lee el registro desde el fichero de datos con el teléfono dado
void Leer(FILE *fa, stRegistro &reg, char *telefono)
{
    FILE *fi;
    stIndice ind;
    long inf, sup, n, nRegs;

    fi = fopen("indices.ind", "rb");
    fseek(fi, 0, SEEK_END);
    nRegs = ftell(fi)/sizeof(stIndice);
    // Búsqueda binaria:
    inf = 0;
    sup = nRegs-1;
    do {
        n = inf+(sup-inf)/2;
        fseek(fi, n*sizeof(stIndice), SEEK_SET);
        fread(&ind, sizeof(stIndice), 1, fi);
        if(strcmp(ind.telefono, telefono) < 0) inf = n+1;
        else sup = n-1;
    } while(inf <= sup && strcmp(ind.telefono, telefono));
    // Si se encontró el teléfono, lee el registro, si no muestra mensaje.
    if(!strcmp(ind.telefono, telefono)) {
        fseek(fa, ind.indice*sizeof(stRegistro), SEEK_SET);
        fread(&reg, sizeof(stRegistro), 1, fa);
    }
    else {
        reg.valido = 'N';
        printf("Registro no encontrado\n");
    }
    fclose(fi);
}

// Añade un registro al archivo de datos y reconstruye los índices
void Insertar(FILE *fa, stRegistro &reg)
{
    // Insertar al final:
    fseek(fa, 0, SEEK_END);
    fwrite(&reg, sizeof(stRegistro), 1, fa);
    ReconstruirIndices(fa);
}

// Lista todos los registros ordenados por el número de teléfono
void ListarPorTelefonos(FILE *fa)
{
    FILE *fi;
    stIndice ind;
    stRegistro reg;

    system("cls");
    fi = fopen("indices.ind", "rb");
    while(fread(&ind, sizeof(stIndice), 1, fi)) {
        fseek(fa, ind.indice*sizeof(stRegistro), SEEK_SET);
        fread(&reg, sizeof(stRegistro), 1, fa);
        printf("%s %s %s %s\n", reg.nombre, reg.apellido[0],
               reg.apellido[1], reg.telefono);
    }
    fclose(fi);
    system("PAUSE");
}

// Lista todos los registros del archivo de datos por el orden en que se
// insertaron.
void ListarNatural(FILE *fa)
{
    stRegistro reg;

    rewind(fa);
    system("cls");
    while(fread(&reg, sizeof(stRegistro), 1, fa))
        printf("%s %s %s %s\n", reg.nombre, reg.apellido[0],
               reg.apellido[1], reg.telefono);
    system("PAUSE");
}

// Reconstruye el archivo de índices
void ReconstruirIndices(FILE *fa)
{
    long n=0;
    FILE *fi;
    stRegistro reg;
    stIndice ind;

    // Crea el fichero de índices a partir del archivo de datos:
    fi = fopen("indices.ind", "w+b");
    rewind(fa);
    while(fread(&reg, sizeof(stRegistro), 1, fa)) {
        strcpy(ind.telefono, reg.telefono);
        ind.indice = n++;
        fwrite(&ind, sizeof(stIndice), 1, fi);
    }
    // Ordena usando el algoritmo Quicksort:
    QuickSort(fi, 0, n-1);
    fclose(fi);
}

// Implementación del algoritmo Quicksort para fichero de índices
void QuickSort(FILE *fi, long inicio, long final)
{
    long iz, de;//Izquierda, Derecha
    char mitad[10];
    static char cad[10];

    iz = inicio;
    de = final;
    strcpy(mitad, LeeCampo(fi, (iz+de)/2, cad));
    do {
        while(strcmp(LeeCampo(fi, iz, cad), mitad) < 0 && iz < final) iz++;
        while(strcmp(mitad, LeeCampo(fi, de, cad)) < 0 && de > inicio) de--;
        if(iz < de) Intercambia(fi, iz, de);
        if(iz <= de) {
            iz++;
            de--;
        }
    } while(iz <= de);
    if(inicio < de) QuickSort(fi, inicio, de);
    if(iz < final) QuickSort(fi, iz, final);
}

char *LeeCampo(FILE *fi, long n, char *buf)
{
    stIndice ind;

    fseek(fi, n*sizeof(stIndice), SEEK_SET);
    fread(&ind, sizeof(stIndice), 1, fi);
    strcpy(buf, ind.telefono);
    return buf;
}

void Intercambia(FILE *fi, long iz, long de)
{
    stIndice reg1, reg2;

    fseek(fi, iz*sizeof(stIndice), SEEK_SET);
    fread(&reg1, sizeof(stIndice), 1, fi);
    fseek(fi, de*sizeof(stIndice), SEEK_SET);
    fread(&reg2, sizeof(stIndice), 1, fi);
    fseek(fi, iz*sizeof(stIndice), SEEK_SET);
    fwrite(&reg2, sizeof(stIndice), 1, fi);
    fseek(fi, de*sizeof(stIndice), SEEK_SET);
    fwrite(&reg1, sizeof(stIndice), 1, fi);
}