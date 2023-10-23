#ifndef __NG_IMAGES_H__
#define __NG_IMAGES_H__

#include "nema_utils.h"

#include "ng_utils.h"

#ifndef EXTERN
#define EXTERN extern
#define REMEMBER_TO_UNDEFINE_EXTERN
#endif

EXTERN img_obj_t _AmbiqSmartwatch_ContactScreen_390x390_tsc4;
EXTERN img_obj_t _AmbiqSmartwatch_MainScreen_390x390_tsc4;
EXTERN img_obj_t _AmbiqSmartwatch_WatchFace_390x390_tsc4;
EXTERN img_obj_t _HoursHand_390x390_tsc6a;
EXTERN img_obj_t _MinutesHand_390x390_tsc6a;
EXTERN img_obj_t _SecondsHand_390x390_tsc6a;

void load_image_assets(void);

#ifdef REMEMBER_TO_UNDEFINE_EXTERN
#undef EXTERN
#undef REMEMBER_TO_UNDEFINE_EXTERN
#endif

#endif  //__NG_IMAGES_H__ 