#ifndef TERRENO_H_
#define TERRENO_H_

/* Essas macros representam os tipos de solo
 * sobre o qual o ciclista pode estar
 */

/* Plano */
#define PLANO 0
/* Subida */
#define SUBIDA 1
/* Descida */
#define DESCIDA 2

/* Plano com Checkpoint */
#define PLANOCP 4
/* Subida com Checkpoint */
#define SUBIDACP 5
/* Descida com Checkpoint */
#define DESCIDACP 6

/* Trecho com checkpoint é igual ao Trecho + 4 xD */
#define CP 4 
#define TIPO_TERR_MASK 3

typedef int Terreno;

#endif
