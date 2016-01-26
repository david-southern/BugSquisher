#include <functional>

#include "TouchManager.h"
#include "GameManager.h"

namespace TouchManager
{
    void PointerEventCallback(s3ePointerEvent* pointerEvent, void *userData)
    {
        ((GameManager*)userData)->PointerEvent(pointerEvent);
    }

    void PointerMotionEventCallback(s3ePointerMotionEvent* motionEvent, void *userData)
    {
        ((GameManager*)userData)->PointerMotionEvent(motionEvent);
    }

    void Init(GameManager *mgr)
    {
        s3ePointerRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)TouchManager::PointerEventCallback, mgr);
        s3ePointerRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)TouchManager::PointerMotionEventCallback, mgr);
    }

    void Shutdown()
    {
        s3ePointerUnRegister(S3E_POINTER_BUTTON_EVENT, (s3eCallback)PointerEventCallback);
        s3ePointerUnRegister(S3E_POINTER_MOTION_EVENT, (s3eCallback)PointerMotionEventCallback);
    }
}

