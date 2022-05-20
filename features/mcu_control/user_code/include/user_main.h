
#ifndef __U_MAIN_H__
#define __U_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <math.h>

#include "main.h"

#include "control_flow.h"

#include "user_struct.h"
#include "user_tim.h"
#include "user_usart.h"

void main_init(void);
void main_while(void);

#ifdef __cplusplus
}
#endif

#endif /* __U_MAIN_H__ */

