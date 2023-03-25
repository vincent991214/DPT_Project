#ifndef DATAFORMAT_H
#define DATAFORMAT_H

#define HD_START    0x5AA5
#define HD_STOP     0xA55A

typedef struct{
    short T1;
    short T2;
    short T3;
    short Tb;
    char  pulses;   // 1, 2, 5, 8 pulse
    char  repeat;   // 1: repetitive run 0:single run
}t_Param;

typedef struct{
    short header;
    t_Param param;
}t_Buf;

typedef union{
    char data[32];
    t_Buf buf;
}Buf_u;

#endif // DATAFORMAT_H
