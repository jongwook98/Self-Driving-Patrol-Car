/*
 * struct.h
 *
 *  Created on: Mar 10, 2022
 *      Author: kimjs
 */

#ifndef __STRUCT_H__
#define __STRUCT_H__

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        uint8_t rsvd : 5;
        uint8_t blinker : 1;
        uint8_t turn_onoff : 1;
        uint8_t move_onoff : 1;
    } st_flag;

    union un_flag {
        uint8_t all;
        st_flag bit;
    };

    typedef struct
    {
        uint8_t start;
        uint8_t mode;
        union un_flag flag;
        uint8_t angle;
        uint8_t stop;
    } st_protocol;

#ifdef __cplusplus
}
#endif

#endif /* __STRUCT_H__ */
