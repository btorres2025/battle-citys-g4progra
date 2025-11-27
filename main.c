#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include "mapa.h"          
#include "visualizacion.h" 
#include "logica.h"       

#define FILAS 13
#define COLUMNAS 13
#define MAX_TURNOS 300

// Muestra informacion de estado (HUD)
void printHUD(int turn, Tanque t1, Tanque t2) {
    printf("Turn: %d\n", turn);
    printf("T3 HP: %d   T4 HP: %d\n", t1.vida, t2.vida); // T3=player1, T4=player2
    printf("T3 dir: %d   T4 dir: %d\n", t1.dir, t2.dir);

    if (turn % 2 == 1) printf("\x1b[32mTurn of T3 (Jugador 1)\x1b[0m\n");
    else printf("\x1b[31mTurn of T4 (Jugador 2)\x1b[0m\n");

    printf("Select: (u)p, (d)own, (l)eft, (r)ight, (s)hoot\n");
}

int main() {
    srand(time(NULL)); // Inicializar semilla aleatoria con el operador

    // PARTE 1: Carga del Mapa
    char *archivo = "mapa.txt";
    // Leemos usando memoria dinamica (malloc/calloc)
    int **mapa = leerArchivo(archivo, FILAS, COLUMNAS);

    // Si falla la carga, generamos otro con uso procedural
    if (mapa == NULL) {
        printf("Generando mapa aleatorio...\n");
        mapa = generarMapaValido(FILAS, COLUMNAS);
        almacenarMapa(mapa, FILAS, COLUMNAS, archivo);
    }

    // Inicialización de lógica de juego
    Tanque t1, t2;
    inicializarTanques(mapa, FILAS, COLUMNAS, &t1, &t2);
    int ganador = 0;

    // PARTE 2: Loop de la Partida
    for(int turn = 1; turn <= MAX_TURNOS; turn++) {
        char move;
        
        visualizarMapa(mapa, FILAS, COLUMNAS); // Render del mapa
        printHUD(turn, t1, t2);
        
        // Input del player
        scanf(" %c", &move);
        
        // Lógica de control/mov
        if(move == 's' || move == 'S') {
            if(turn % 2 == 1)
                disparar(mapa, FILAS, COLUMNAS, t1.fila, t1.col, t1.dir, 1, &t1, &t2);
            else
                disparar(mapa, FILAS, COLUMNAS, t2.fila, t2.col, t2.dir, 2, &t1, &t2);
        }
        else {
            // Movimiento (actualiza mapa y structs)
            moverTanque(mapa, FILAS, COLUMNAS, &t1, &t2, turn, move);
        }

        // Verificar fin de juego
        ganador = verificarGanador(t1, t2);
        if (ganador != 0) {
            visualizarMapa(mapa, FILAS, COLUMNAS);
            if(ganador == 1) printf("\n--- GANA EL JUGADOR 1 (T3) ---\n");
            else printf("\n--- GANA EL JUGADOR 2 (T4) ---\n");
            break;
        }
    }

    // Mensaje de empate si se acaban los turnos
    if(ganador == 0) {
        visualizarMapa(mapa, FILAS, COLUMNAS);
        if(t1.vida > t2.vida) printf("\nGANA EL JUGADOR 1 (T3)\n");
        else if(t2.vida > t1.vida) printf("\nGANA EL JUGADOR 2 (T4)\n");
        else printf("\nEMPATE\n");
    }

    // --- FASE 3: Limpieza ---
    liberaMemoria(mapa, FILAS); // Liberar RAM
    return 0;

}
