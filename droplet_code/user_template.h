#pragma once

#include "droplet_init.h"
#include "button.h"
#include "button_press_queue.h"

#define KEYPRESS_MSG_FLAG 'K'
#define MOUSE_MOVE_MSG_FLAG 'M'

#define MOUSE_RNB_BROADCAST_PERIOD 500

//#define KEYBOARD_DEBUG_MODE
#ifdef KEYBOARD_DEBUG_MODE
#define KEYBOARD_DEBUG_MODE(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define KEYBOARD_DEBUG_MODE(format, ...)
#endif

//#define RNB_DEBUG_MODE
#ifdef RNB_DEBUG_MODE
#define RNB_DEBUG_PRINT(format, ...) printf_P(PSTR(format), ##__VA_ARGS__)
#else
#define RNB_DEBUG_PRINT(format, ...)
#endif

#define SLOT_LENGTH_MS			397
#define SLOTS_PER_FRAME			38
#define FRAME_LENGTH_MS			(((uint32_t)SLOT_LENGTH_MS)*((uint32_t)SLOTS_PER_FRAME))
#define LOOP_DELAY_MS			17

typedef enum droplet_role{
	UNKNOWN,
	KEYBOARD,
	MOUSE	
}DropletRole;

typedef uint8_t LEDStore[3];

typedef struct mouse_move_msg_node_struct{
	MouseMoveMsg msg;
	uint8_t numTries;
}MouseMoveMsgNode;

typedef struct mouse_move_msg_struct{
	uint32_t	  time;
	int8_t		deltaX;
	int8_t		deltaY;
	uint8_t	      flag;
}MouseMoveMsg;
#define IS_MOUSE_MOVE_MSG(msgStruct) ( (msgStruct->length==sizeof(MouseMoveMsg)) && (((MouseMoveMsg*)(msgStruct->msg))->flag==MOUSE_MOVE_MSG_FLAG) )

typedef struct button_press_msg_node_struct{
	ButtonPressMsg msg;
	uint8_t numTries;
}ButtonPressMsgNode;

typedef struct button_press_msg_struct{
	ButtonPressEvent	evt;
	uint8_t			flag;
}ButtonPressMsg;
#define IS_BUTTON_PRESS_MSG(msgStruct) ( (msgStruct->length==sizeof(ButtonPressMsg)) && (((ButtonPressMsg*)(msgStruct->msg))->flag==KEYPRESS_MSG_FLAG) )

uint32_t	frameCount;
uint32_t	frameStart;
uint32_t	lastKeypress;
uint16_t	mySlot;
uint16_t	loopID;
uint8_t		isWired;
uint8_t		isShifted;
Task_t*		mouseBroadcastTask;
id_t		leftMouseID;
volatile LEDStore buttonPressBlinkLEDStore;
volatile LEDStore wiredBlinkLEDStore;
DropletRole myRole;
Button myButton;
volatile Task_t* wireSleepTask;

void		init(void);
void		loop(void);
void		handleMsg(irMsg* msg_struct);

void prepMouseMoveMsg(MouseMoveEvent* evt);
void sendMouseMoveMsg(MouseMoveMsgNode* msgNode);
void prepButtonPressMsg(ButtonPressEvent* evt);
void sendButtonPressMsg(ButtonPressMsgNode* msgNode);
void handleButtonPressMsg(ButtonPressMsg* msg);
void handleMouseMoveMsg(MouseMoveMsg* msg);
void checkPosition(void);
void restoreLED(volatile LEDStore* vals);

void wireSleep(void);


uint32_t getExponentialBackoff(uint8_t c);

static inline uint16_t getSlot(id_t id){
	return (id%(SLOTS_PER_FRAME-1));
}

inline DropletRole getRoleFromPosition(BotPos* pos){
	myButton = getKeyFromPosition(pos);
	if(myButton != BUTTON_UNKNOWN){
		return KEYBOARD;
	}
	if( (pos->x > 652) && (pos->y <= 225 ) && (pos->y > 0)){
		return MOUSE;
	}
	return UNKNOWN;
}

inline uint8_t storeAndSetLED(uint8_t r, uint8_t g, uint8_t b, volatile LEDStore* vals){
	if( (getRedLED()!=r) || (getGreenLED()!=g) || (getBlueLED()!=b) ){
		(*vals)[0] = getRedLED();
		(*vals)[1] = getGreenLED();
		(*vals)[2] = getBlueLED();
		return 1;
	}else{
		return 0;
	}
}

inline void wireTxButtonPress(Button key){
	storeAndSetLED(50, 0, 0, &wiredBlinkLEDStore);
	scheduleTask(150, (arg_func_t)restoreLED, (void*)&wiredBlinkLEDStore);
	printf("ButtonPress ");
	printf(isprint(key) ? "   '%c'\r\n" : "'\\%03hu'\r\n", key);
}

inline void wireMouseMove(int8_t deltaX, int8_t deltaY){
	storeAndSetLED(50, 50, 0, &wiredBlinkLEDStore);
	scheduleTask(150, (arg_func_t)restoreLED, (void*)&wiredBlinkLEDStore);
	printf("MouseMove % 4hd % 4hd\r\n", deltaX, deltaY);
}

inline void buildButtonPressEvent(ButtonPressEvent* evt){
	printf("PRESSED: ");
	printf(isprint(myButton) ? "   '%c'\r\n" : "'\\%03hu'\r\n", myButton);
	evt->time = getTime();
	evt->button = ( !isShifted && isupper(myButton) ) ? (myButton+32) : myButton; //convert to lowercase if appropriate.
	evt->src = getDropletID();
}