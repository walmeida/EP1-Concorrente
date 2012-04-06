#ifndef TERRENO_H_
#define TERRENO_H_

// Essas macros representam os tipos de solo sobre o qual o ciclista pode estar

// Plano
#define PLANO 0
// Subida
#define SUBIDA 1
// Descida
#define DESCIDA 2

// Plano com Checkpoint
#define PLANOCP 3
// Subida com Checkpoint
#define SUBIDACP 4
// Descida com Checkpoint
#define DESCIDACP 5

// Trecho com checkpoint é igual ao Trecho + 3 xD
#define CP 3 

typedef int Terreno;

#endif
