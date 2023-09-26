#undef EXTERN
#include "gui_tree.h"
#undef EXTERN

#include "ng_globals.h"
#include "fonts.h"
#include "images.h"

gitem_screen_t _1MainScreen = {EVENT(NULL), ng_screen_draw, GESTURES(NULL), FLAGS(0x00000036U), X(0), Y(0), W(456), H(456), ID(1), GITEM_MAIN_SCREEN , COLOR(0xff000000U) , IMAGE(NULL)};
	gitem_image_t _7MainWatchFace = {EVENT(NULL), ng_image_draw, GESTURES(NULL), FLAGS(0x00000040U), X(0), Y(0), W(455), H(455), ID(7), GITEM_IMAGE , COLOR(0xff808080U) , IMAGE(&_AmbiqSmartwatch_MainScreen_tsc4)};

gitem_screen_t _3AnalogWatchFaceScreen = {EVENT(NULL), ng_screen_draw, GESTURES(NULL), FLAGS(0x00000036U), X(0), Y(0), W(456), H(456), ID(3), GITEM_MAIN_SCREEN , COLOR(0xff000000U) , IMAGE(NULL)};
	gitem_watchface_t _4AnalogWatchFace = {EVENT(NULL), ng_watchface_draw, GESTURES(NULL), FLAGS(0x00000040U), X(0), Y(0), W(455), H(455), ID(4), GITEM_WATCH_FACE , COLOR(0xff808080U), IMAGE(&_AmbiqSmartwatch_WatchFace_tsc4), NG_GITEM(&_20HoursHand), NG_GITEM(&_19MinutesHand), NG_GITEM(&_18SecondsHand), PEN_WIDTH(1)};
		gitem_needle_t _20HoursHand = {EVENT(NULL), ng_needle_draw, GESTURES(NULL), FLAGS(0x00000040U), X(227), Y(227), W(32), H(164), ID(20), GITEM_NEEDLE , COLOR(0xff000000U) , IMAGE(&_HoursHand_tsc6a), ANGLE(0.000f), X_ROT(16), Y_ROT(148), PEN_WIDTH(1)};
		gitem_needle_t _19MinutesHand = {EVENT(NULL), ng_needle_draw, GESTURES(NULL), FLAGS(0x00000040U), X(227), Y(227), W(32), H(190), ID(19), GITEM_NEEDLE , COLOR(0xff000000U) , IMAGE(&_MinutesHand_tsc6a), ANGLE(0.000f), X_ROT(16), Y_ROT(173), PEN_WIDTH(1)};
		gitem_needle_t _18SecondsHand = {EVENT(NULL), ng_needle_draw, GESTURES(NULL), FLAGS(0x00000040U), X(227), Y(227), W(44), H(189), ID(18), GITEM_NEEDLE , COLOR(0xff000000U) , IMAGE(&_SecondsHand_tsc6a), ANGLE(0.000f), X_ROT(22), Y_ROT(173), PEN_WIDTH(1)};

gitem_screen_t _5DigitalWatchFaceScreen = {EVENT(NULL), ng_screen_draw, GESTURES(NULL), FLAGS(0x00000036U), X(0), Y(0), W(456), H(456), ID(5), GITEM_MAIN_SCREEN , COLOR(0xff000000U) , IMAGE(NULL)};
	gitem_image_t _10DigitalWatchFace = {EVENT(NULL), ng_image_draw, GESTURES(NULL), FLAGS(0x00000040U), X(0), Y(0), W(455), H(455), ID(10), GITEM_IMAGE , COLOR(0xff808080U) , IMAGE(&_AmbiqSmartwatch_WatchFace_tsc4)};
	gitem_digital_clock_t _17DigitalClock = {EVENT(NULL), ng_digital_clock_draw, GESTURES(NULL), FLAGS(0x00000080U), X(0), Y(0), W(455), H(455), ID(17), GITEM_DIGITAL_CLOCK , COLOR(0x00000000U), TEXT_COLOR(0x00e6e6e7U), &NotoSans_Regular80pt8b, NEMA_ALIGNX_CENTER | NEMA_ALIGNY_CENTER, TIME_FORMAT(NG_TIME_H_MM)};

gitem_screen_t _6ContactScreen = {EVENT(NULL), ng_screen_draw, GESTURES(NULL), FLAGS(0x00000036U), X(0), Y(0), W(456), H(456), ID(6), GITEM_MAIN_SCREEN , COLOR(0xff000000U) , IMAGE(NULL)};
	gitem_image_t _9ContactWatchFace = {EVENT(NULL), ng_image_draw, GESTURES(NULL), FLAGS(0x00000040U), X(0), Y(0), W(455), H(455), ID(9), GITEM_IMAGE , COLOR(0xff808080U) , IMAGE(&_AmbiqSmartwatch_ContactScreen_tsc4)};


//---------------------------------


//Build tree nodes
//---------------------------------
tree_node_t node_1MainScreen = { NG_GITEM(&_1MainScreen), PARENT_NODE(NULL), FIRST_CHILD_NODE(&node_7MainWatchFace), NEXT_NODE(NULL)};
	tree_node_t node_7MainWatchFace = { NG_GITEM(&_7MainWatchFace), PARENT_NODE(&node_1MainScreen), FIRST_CHILD_NODE(NULL), NEXT_NODE(NULL)};

//Build tree nodes
//---------------------------------
tree_node_t node_3AnalogWatchFaceScreen = { NG_GITEM(&_3AnalogWatchFaceScreen), PARENT_NODE(NULL), FIRST_CHILD_NODE(&node_4AnalogWatchFace), NEXT_NODE(NULL)};
	tree_node_t node_4AnalogWatchFace = { NG_GITEM(&_4AnalogWatchFace), PARENT_NODE(&node_3AnalogWatchFaceScreen), FIRST_CHILD_NODE(&node_20HoursHand), NEXT_NODE(NULL)};
		tree_node_t node_20HoursHand = { NG_GITEM(&_20HoursHand), PARENT_NODE(&node_4AnalogWatchFace), FIRST_CHILD_NODE(NULL), NEXT_NODE(&node_19MinutesHand)};
		tree_node_t node_19MinutesHand = { NG_GITEM(&_19MinutesHand), PARENT_NODE(&node_4AnalogWatchFace), FIRST_CHILD_NODE(NULL), NEXT_NODE(&node_18SecondsHand)};
		tree_node_t node_18SecondsHand = { NG_GITEM(&_18SecondsHand), PARENT_NODE(&node_4AnalogWatchFace), FIRST_CHILD_NODE(NULL), NEXT_NODE(NULL)};

//Build tree nodes
//---------------------------------
tree_node_t node_5DigitalWatchFaceScreen = { NG_GITEM(&_5DigitalWatchFaceScreen), PARENT_NODE(NULL), FIRST_CHILD_NODE(&node_10DigitalWatchFace), NEXT_NODE(NULL)};
	tree_node_t node_10DigitalWatchFace = { NG_GITEM(&_10DigitalWatchFace), PARENT_NODE(&node_5DigitalWatchFaceScreen), FIRST_CHILD_NODE(NULL), NEXT_NODE(&node_17DigitalClock)};
	tree_node_t node_17DigitalClock = { NG_GITEM(&_17DigitalClock), PARENT_NODE(&node_5DigitalWatchFaceScreen), FIRST_CHILD_NODE(NULL), NEXT_NODE(NULL)};

//Build tree nodes
//---------------------------------
tree_node_t node_6ContactScreen = { NG_GITEM(&_6ContactScreen), PARENT_NODE(NULL), FIRST_CHILD_NODE(&node_9ContactWatchFace), NEXT_NODE(NULL)};
	tree_node_t node_9ContactWatchFace = { NG_GITEM(&_9ContactWatchFace), PARENT_NODE(&node_6ContactScreen), FIRST_CHILD_NODE(NULL), NEXT_NODE(NULL)};
