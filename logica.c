/* changes hechos
funciones moveTank o shoot no recibían casi nada entre paréntesis y ahora tienen todos los punteros
implementa el uso de la flecha como operador para mover el tanque real y no una copia (jodia el mapa)
moveTank verificaba colisiones, pero a veces no actualizaba la matriz
printMap dibujaba los tanques "por encima" basándose en sus coordenadas
cambios de lógica un poco más detallados, por ahí tengo los docs en el historial
traducción de código y comentarios agregados
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "logica.h"

// Escanea el mapa para configurar los estados iniciales de los jugadores
void inicializarTanques(int **mapa, int filas, int cols, Tanque *t1, Tanque *t2) {
    t1->vida = 3; t1->dir = 0;
    t2->vida = 3; t2->dir = 0;
    for (int i = 0; i < filas; i++) {
        for (int j = 0; j < cols; j++) {
            if (mapa[i][j] == 3) { t1->fila = i; t1->col = j; }
            else if (mapa[i][j] == 4) { t2->fila = i; t2->col = j; }
        }
    }
}

// Función placeRandomPos reubicada para guia
Tanque reubicarTanque(int **mapa, int filas, int cols, Tanque t) {
    Tanque mTank = t;
    int placed = 0;
    
    // Borramos la posición antigua del mapa visual para evitar fantasmas
    if(mapa[mTank.fila][mTank.col] == 3 || mapa[mTank.fila][mTank.col] == 4) 
        mapa[mTank.fila][mTank.col] = 0;

    while(!placed) {
        // Intentamos coordenadas aleatorias hasta encontrar espacios vacío (0)
        for(int i = 0; i < 2; i++) {
            switch(i) {
                case 0: mTank.fila = rand() % filas; break;
                case 1: mTank.col = rand() % cols;
            }
            if(mapa[mTank.fila][mTank.col] == 0) placed = 1;
        }
    }
    // Nota: La actualizacion visual del nuevo ID se hace fuera o se asume aqui
    return mTank;
}

// movimiento por turnos (moveTank), se agregaron id's creados
// Controla la posición del jugador y evita atravesar paredes.
void moverTanque(int **mapa, int filas, int cols, Tanque *t1, Tanque *t2, int turn, char moveDir) {
    Tanque out;
    Tanque con; // Tanque CONtrario (para colisiones entre ellos)
    int idOut, idCon;

    // Determinar turno del jugador
    if(turn % 2 == 1) {
        out = *t1; con = *t2; idOut = 3; idCon = 4;
    } else {
        out = *t2; con = *t1; idOut = 4; idCon = 3;
    }

    // Calcular movimiento tentativo según tecla (previsualzarlo/predict como dicen los lolos)
    switch (moveDir) {
        case 'U': case 'u':
            out.dir = 0;
            // Verifica limites superiores y que la celda sea 0 (vacia)
            if (out.fila - 1 >= 0 && mapa[out.fila - 1][out.col] == 0) {
                // Verifica no chocar con el otro tanque
                if(out.fila - 1 == con.fila && out.col == con.col) break;
                else out.fila--;
            }
            break;

        case 'D': case 'd':
            out.dir = 1;
            if (out.fila + 1 < filas && mapa[out.fila + 1][out.col] == 0) {
                if(out.fila + 1 == con.fila && out.col == con.col) break;
                else out.fila++;
            }
            break;
    
        case 'L': case 'l':
            out.dir = 2;
            if (out.col - 1 >= 0 && mapa[out.fila][out.col - 1] == 0) {
                if(out.fila == con.fila && out.col - 1 == con.col) break;
                else out.col--;
            }
            break;
    
        case 'R': case 'r':
            out.dir = 3;
            if (out.col + 1 < cols && mapa[out.fila][out.col + 1] == 0) {
                if(out.fila == con.fila && out.col + 1 == con.col) break;
                else out.col++;
            }
            break;
        default: break;
    }

    // Actualizar Matriz Visual
    if(turn % 2 == 1) mapa[t1->fila][t1->col] = 0; // Borrar rastro Tank1
    else mapa[t2->fila][t2->col] = 0;              // Borrar rastro Tank2

    mapa[out.fila][out.col] = idOut; // nueva pos

    // Guardar el nuevo estado en la struct original
    if(turn % 2 == 1) *t1 = out;
    else *t2 = out;
}

// Lógica de Disparo (shoot)
void disparar(int **mapa, int filas, int cols, int sRow, int sCol, int sDir, int shooter, Tanque *t1, Tanque *t2) {
    struct shoot s;
    s.row = sRow;
    s.col = sCol;
    
    while(1) {
        // bala avanzar, guerra avanzar
        switch (sDir) {
            case 0: s.row--; break; // Arriba
            case 1: s.row++; break; // Abajo
            case 2: s.col--; break; // Izquierda
            case 3: s.col++; break; // Derecha
        }

        // Si sale del mapa, la bala desaparece
        if (s.row < 0 || s.row >= filas || s.col < 0 || s.col >= cols) break;

        // Interacción con el entorno según material
        if(mapa[s.row][s.col] == 2) break; // Acero: Bala rebota/desaparece
        else if(mapa[s.row][s.col] == 1) { // Ladrillo: Se rompe
            mapa[s.row][s.col] = 0;
            break;
        }
        else {
            // Logica de impacto a Jugadores
            if(shooter == 1) { // Dispara player1 (Target T2)
                if(s.row == t2->fila && s.col == t2->col) {
                    t2->vida--;
                    // Respawn: Buscar nueva spawn y actualizar mapa
                    Tanque nuevoT2 = reubicarTanque(mapa, filas, cols, *t2);
                    mapa[t2->fila][t2->col] = 0; // viejo desaparece
                    *t2 = nuevoT2;
                    mapa[t2->fila][t2->col] = 4; // nuevo aparece
                    break;
                }
            }
            if(shooter == 2) { // Dispara player2 (Target T1)
                if(s.row == t1->fila && s.col == t1->col) {
                    t1->vida--;
                    Tanque nuevoT1 = reubicarTanque(mapa, filas, cols, *t1);
                    mapa[t1->fila][t1->col] = 0;
                    *t1 = nuevoT1;
                    mapa[t1->fila][t1->col] = 3;
                    break;
                }
            }
        }
    }
}

// Verifica condición de victoria por vidas
int verificarGanador(Tanque t1, Tanque t2) {
    if (t1.vida <= 0) return 2; // Gana J2
    if (t2.vida <= 0) return 1; // Gana J1
    return 0;

}
