// vim:ft=c
/****************************************************************************** 
  declaration de differents types et macros
******************************************************************************/
 
#ifndef _TYPES_MACROS_H_
#define _TYPES_MACROS_H_

#include <stdio.h>
#include <stdlib.h>

/* type entier positif */
typedef unsigned int UINT;

/* macro affichant le message _m_ et s'arretant */
#define ERREUR_FATALE(_m_) \
{ \
fprintf(stderr, "%s\n", _m_); \
exit(1); \
}

#endif
