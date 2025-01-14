#pragma once

//#define INIT_DEBUG_MODE
#ifdef INIT_DEBUG_MODE
#define INIT_DEBUG_PRINT(x) printf(x)
#else
#define INIT_DEBUG_PRINT(x)
#endif

#include "droplet_base.h"
#include "scheduler.h"
#include "pc_comm.h"
#include "rgb_led.h"
#include "rgb_sensor.h"
#include "power.h"
#include "i2c.h"
#include "range_algs.h"
#include "rgb_sensor.h"
#include "ir_led.h"
#include "ir_sensor.h"
#include "ir_comm.h"
#include "speaker.h"
#include "mic.h"
#include "motor.h"
#include "random.h"
#include "firefly_sync.h"
#include "serial_handler.h"
#include "matrix_utils.h"
#include "localization.h"

#define CMD_DROPLET_ID	0x8F6D

typedef struct ir_msg_struct
{
	uint32_t arrivalTime;	// Time of message receipt.
	id_t senderID;		// ID of sending robot.
	char* msg;				// The message.
	uint8_t length;			// Message length.
} irMsg;

extern void init(void);
extern void loop(void);
extern void handleMsg(irMsg* msg_struct);
extern void handleMeas(Rnb* meas);
extern uint8_t userHandleCommand(char* commandWord, char* commandArgs);
extern void userMicInterrupt(void);


void startupLightSequence(void);
uint8_t getDropletOrd(id_t id);

extern const id_t OrderedBotIDs[121];

inline id_t getIdFromOrd(uint8_t ord){
	return pgm_read_word(&OrderedBotIDs[ord]);
}