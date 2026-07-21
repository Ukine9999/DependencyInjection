#pragma once

#define DI_MODULE_ANCHOR(NAME)          \
    namespace Di::Anchors               \
    {                                   \
        int DiAnchorSink_##NAME = 0;    \
        int DiAnchorTouch_##NAME()      \
        {                               \
            return DiAnchorSink_##NAME; \
        }                               \
    }

#define DI_USE_MODULE(NAME)                                                   \
    namespace Di::Anchors                                                     \
    {                                                                         \
        int DiAnchorTouch_##NAME();                                           \
    }                                                                         \
    namespace                                                                 \
    {                                                                         \
        const int DiAnchorUse_##NAME = ::Di::Anchors::DiAnchorTouch_##NAME(); \
    }
