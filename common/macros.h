#pragma once

/**
 * Namespace.
 */
#define GUNGNIR_NAMESPACE_OPEN_SCOPE namespace gungnir {
#define GUNGNIR_NAMESPACE_CLOSE_SCOPE }

/**
 * Compilers.
 */
#define GUNGNIR_COMPILER_MSVC 1
#define GUNGNIR_COMPILER_CLANG 2
#define GUNGNIR_COMPILER_GCC 3

/**
 * Determine the compiler in use.
 * http://sourceforge.net/p/predef/wiki/Compilers/
 */
#ifndef GUNGNIR_COMPILER
#if defined(_MSC_VER)
#define GUNGNIR_COMPILER GUNGNIR_COMPILER_MSVC
#elif defined(__clang__)
#define GUNGNIR_COMPILER GUNGNIR_COMPILER_CLANG
#elif defined(__GNUC__)
#define GUNGNIR_COMPILER GUNGNIR_COMPILER_GCC
#else
#error "Unsupported compiler"
#endif
#endif // GUNGNIR_COMPILER

#define GUNGNIR_MSVC (GUNGNIR_COMPILER == GUNGNIR_COMPILER_MSVC)
#define GUNGNIR_CLANG (GUNGNIR_COMPILER == GUNGNIR_COMPILER_CLANG)
#define GUNGNIR_GCC (GUNGNIR_COMPILER == GUNGNIR_COMPILER_GCC)

/**
 * Platforms.
 */
#define GUNGNIR_PLATFORM_WINDOWS 1
#define GUNGNIR_PLATFORM_LINUX 2

/**
 * Determine the target platform in use.
 * http://sourceforge.net/p/predef/wiki/OperatingSystems/
 */
#ifndef GUNGNIR_PLATFORM
#if defined(_WIN64)
#define GUNGNIR_PLATFORM GUNGNIR_PLATFORM_WINDOWS
#elif defined(__linux__)
#define GUNGNIR_PLATFORM GUNGNIR_PLATFORM_LINUX
#else
#error "Unsupported target platform"
#endif
#endif // GUNGNIR_PLATFORM

#define GUNGNIR_WINDOWS (GUNGNIR_PLATFORM == GUNGNIR_PLATFORM_WINDOWS)
#define GUNGNIR_LINUX (GUNGNIR_PLATFORM == GUNGNIR_PLATFORM_LINUX)

/**
 * D3D12 Agility SDK.
 */
#if GUNGNIR_HAS_D3D12_AGILITY_SDK
#define GUNGNIR_D3D12_AGILITY_SDK_VERSION 4
#define GUNGNIR_D3D12_AGILITY_SDK_PATH ".\\D3D12\\"
// To enable the D3D12 Agility SDK, this macro needs to be added to the main source file of the executable.
#define GUNGNIR_EXPORT_D3D12_AGILITY_SDK                                                                 \
    extern "C"                                                                                          \
    {                                                                                                   \
        GUNGNIR_API_EXPORT extern const unsigned int D3D12SDKVersion = GUNGNIR_D3D12_AGILITY_SDK_VERSION; \
    }                                                                                                   \
    extern "C"                                                                                          \
    {                                                                                                   \
        GUNGNIR_API_EXPORT extern const char* D3D12SDKPath = GUNGNIR_D3D12_AGILITY_SDK_PATH;              \
    }
#else
#define GUNGNIR_EXPORT_D3D12_AGILITY_SDK
#endif

/**
 * Define for checking if NVAPI is available.
 */
#define GUNGNIR_NVAPI_AVAILABLE (GUNGNIR_HAS_D3D12 && GUNGNIR_HAS_NVAPI)

/**
 * Shared library (DLL) export and import.
 */
#if GUNGNIR_WINDOWS
#define GUNGNIR_API_EXPORT __declspec(dllexport)
#define GUNGNIR_API_IMPORT __declspec(dllimport)
#elif GUNGNIR_LINUX
#define GUNGNIR_API_EXPORT __attribute__((visibility("default")))
#define GUNGNIR_API_IMPORT
#endif

#ifdef GUNGNIR_DLL
#define GUNGNIR_API GUNGNIR_API_EXPORT
#else // GUNGNIR_DLL
#define GUNGNIR_API GUNGNIR_API_IMPORT
#endif // GUNGNIR_DLL

/**
 * Force inline.
 */
#if GUNGNIR_MSVC
#define GUNGNIR_FORCEINLINE __forceinline
#elif GUNGNIR_CLANG | GUNGNIR_GCC
#define GUNGNIR_FORCEINLINE __attribute__((always_inline))
#endif

/**
 * Preprocessor stringification.
 */
#define GUNGNIR_STRINGIZE(a) #a
#define GUNGNIR_CONCAT_STRINGS_(a, b) a##b
#define GUNGNIR_CONCAT_STRINGS(a, b) GUNGNIR_CONCAT_STRINGS_(a, b)

/**
 * Implement logical operators on a class enum for making it usable as a flags enum.
 */
// clang-format off
#define GUNGNIR_ENUM_CLASS_OPERATORS(e_) \
    inline e_ operator& (e_ a, e_ b) { return static_cast<e_>(static_cast<int>(a)& static_cast<int>(b)); } \
    inline e_ operator| (e_ a, e_ b) { return static_cast<e_>(static_cast<int>(a)| static_cast<int>(b)); } \
    inline e_& operator|= (e_& a, e_ b) { a = a | b; return a; }; \
    inline e_& operator&= (e_& a, e_ b) { a = a & b; return a; }; \
    inline e_  operator~ (e_ a) { return static_cast<e_>(~static_cast<int>(a)); } \
    inline bool is_set(e_ val, e_ flag) { return (val & flag) != static_cast<e_>(0); } \
    inline void flip_bit(e_& val, e_ flag) { val = is_set(val, flag) ? (val & (~flag)) : (val | flag); }
// clang-format on
