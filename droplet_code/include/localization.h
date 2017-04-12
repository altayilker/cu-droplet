/** \file *********************************************************************
 * Low level sensing functions using IR channels.
 * Note that there is no IR communication code in this file.
 *****************************************************************************/
#pragma once

#include <droplet_init.h>

#define BOT_MEAS_MSG_DUR		72
#define LOCALIZATION_DUR (RNB_DUR + BOT_MEAS_MSG_DUR*2)

#define POS_CALC_DEBUG_MODE
#define POS_MSG_DEBUG_MODE
#define MY_POS_DEBUG_MODE
//#define COVAR_DEBUG_MODE

#ifdef POS_CALC_DEBUG_MODE
#define POS_CALC_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define POS_CALC_DEBUG_PRINT(format, ...)
#endif

#ifdef POS_MSG_DEBUG_MODE
#define POS_MSG_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define POS_MSG_DEBUG_PRINT(format, ...)
#endif

#ifdef MY_POS_DEBUG_MODE
#define MY_POS_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define MY_POS_DEBUG_PRINT(format, ...)
#endif

#define UNDF	((int16_t)0x8000)
#define POS_DEFINED(pos) ((((pos)->x)!=UNDF)&&(((pos)->y)!=UNDF)&&(((pos)->o)!=UNDF))

#define BOT_MEAS_MSG_FLAG		'X'

typedef union flex_byte_union{
	uint16_t u;
	int16_t d;
}FlexByte;

typedef FlexByte DensePosCovar[6];

typedef struct bot_pos_struct{
	int16_t x;
	int16_t y;
	int16_t o;
} BotPos;

typedef struct bot_meas_msg_struct{
	BotPos pos; //6 bytes
	DensePosCovar covar; //12 bytes
	char flag;
}BotMeasMsg;

typedef struct bot_meas_msg_node_struct{
	BotMeasMsg msg;
	id_t tgt;
	uint16_t range;
	uint8_t dirMask;
	uint8_t numTries;
}BotMeasMsgNode;

typedef struct bot_meas_struct{
	id_t id;
	uint16_t r;
	int16_t b;
	int16_t h;
} BotMeas;

BotPos myPos;
DensePosCovar myPosCovar;
uint8_t		seedFlag;

void	localization_init();
void	processMeasurement(id_t id, uint16_t r, int16_t b, int16_t h);
void	handleBotMeasMsg(BotMeasMsg* msg, id_t senderID __attribute__ ((unused)));
void	getPosColor(uint8_t* r, uint8_t* g, uint8_t* b);
void	printPosCovar(DensePosCovar* denseP);


inline uint8_t dirFromAngle(int16_t angle){
	return abs((angle - (angle>0 ? 360 : 0))/60);
}