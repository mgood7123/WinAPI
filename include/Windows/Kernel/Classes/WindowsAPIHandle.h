//
// Created by konek on 7/18/2019.
//

#ifndef MEDIA_PLAYER_PRO_WINDOWSAPICLASSESHANDLE_H
#define MEDIA_PLAYER_PRO_WINDOWSAPICLASSESHANDLE_H

#include "WindowsAPIObject.h"

typedef class Handle {
    public:
        Handle();

        ~Handle();

        bool invalidated;
        Object *object;
} Handle;

#endif //MEDIA_PLAYER_PRO_WINDOWSAPICLASSESHANDLE_H