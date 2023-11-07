#pragma once

#include "core/arch/defines.h"

/// \file arch/hints.h
/// Compiler hints.
///
/// \c ARCH_LIKELY(bool-expr) and \c ARCH_UNLIKELY(bool-expr) will evaluate to
/// the value of bool-expr but will also emit compiler intrinsics providing
/// hints for branch prediction if the compiler has such intrinsics.  It is
/// advised that you only use these in cases where you empirically know the
/// outcome of bool-expr to a very high degree of certainty.  For example,
/// fatal-error cases, invariants, first-time initializations, etc.

#if defined(ARCH_COMPILER_GCC) || defined(ARCH_COMPILER_CLANG)

#define ARCH_LIKELY(x) (__builtin_expect((bool)(x), true))
#define ARCH_UNLIKELY(x) (__builtin_expect((bool)(x), false))

#else

#define ARCH_LIKELY(x) (x)
#define ARCH_UNLIKELY(x) (x)

#endif
