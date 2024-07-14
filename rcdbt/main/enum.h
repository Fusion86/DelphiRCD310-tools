#pragma once

// clang-format off
#define __ENUM_KV_DECLARE(K, V) K = V,
#define __ENUM_SWITCHCASE(K, V) case K: return #K;

// Does nothing. Can be used to place comments inside an X macro.
#define ENUM_COMMENT(x)

#define __ENUM_TOSTRING_FUNCNAME(typename) const char *typename##_to_string(typename x)
// clang-format on

#define __ENUM_DEFINE_TOSTRING_FUNC(typename, enum_x)               \
    __ENUM_TOSTRING_FUNCNAME(typename)                              \
    {                                                               \
        switch (x)                                                  \
        {                                                           \
            enum_x(__ENUM_SWITCHCASE) default : return "<invalid>"; \
        }                                                           \
    }

// Declare an enum and the enum_to_string() function.
#define ENUM_DECLARE(typename, enum_x) \
    typedef enum typename##_t          \
    {                                  \
        enum_x(__ENUM_KV_DECLARE)      \
    }                                  \
    typename;                          \
    __ENUM_TOSTRING_FUNCNAME(typename);

// Defines the enum_to_string() function.
#define ENUM_DEFINE(typename, enum_x) __ENUM_DEFINE_TOSTRING_FUNC(typename, enum_x)
