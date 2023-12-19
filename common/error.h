//#pragma once
//
//#include "common/macros.h"
//
//#include <source_location>
//#include <format>
//#include <exception>
//#include <memory>
//#include <string>
//#include <string_view>
//#include <type_traits>
//
//GUNGNIR_NAMESPACE_OPEN_SCOPE
//
////
//// Exceptions.
////
//
//#ifdef _MSC_VER
//#pragma warning(push)
//#pragma warning(disable : 4275) // allow dllexport on classes dervied from STL
//#endif
//
///**
// * Base class for all Gungnir exceptions.
// */
//class GUNGNIR_API Exception : public std::exception
//{
//public:
//    Exception() noexcept {}
//    Exception(std::string_view what) : mpWhat(std::make_shared<std::string>(what)) {}
//    Exception(const Exception& other) noexcept { mpWhat = other.mpWhat; }
//    virtual ~Exception() override {}
//    virtual const char* what() const noexcept override { return mpWhat ? mpWhat->c_str() : ""; }
//
//protected:
//    // Message is stored as a reference counted string in order to allow copy constructor to be noexcept.
//    std::shared_ptr<std::string> mpWhat;
//};
//
//#ifdef _MSC_VER
//#pragma warning(pop)
//#endif
//
///**
// * Exception to be thrown when an error happens at runtime.
// */
//class GUNGNIR_API RuntimeError : public Exception
//{
//public:
//    RuntimeError() noexcept {}
//    RuntimeError(std::string_view what) : Exception(what) {}
//    RuntimeError(const RuntimeError& other) noexcept { mpWhat = other.mpWhat; }
//    virtual ~RuntimeError() override {}
//};
//
///**
// * Exception to be thrown on GUNGNIR_ASSERT.
// */
//class GUNGNIR_API AssertionError : public Exception
//{
//public:
//    AssertionError() noexcept {}
//    AssertionError(std::string_view what) : Exception(what) {}
//    AssertionError(const AssertionError& other) noexcept { mpWhat = other.mpWhat; }
//    virtual ~AssertionError() override {}
//};
//
////
//// Exception helpers.
////
//
///// Throw a RuntimeError exception.
///// If ErrorDiagnosticFlags::AppendStackTrace is set, a stack trace will be appended to the exception message.
///// If ErrorDiagnosticFlags::BreakOnThrow is set, the debugger will be broken into (if attached).
//[[noreturn]] GUNGNIR_API void throwException(const std::source_location& loc, std::string_view msg);
//
//namespace detail
//{
///// Overload to allow GUNGNIR_THROW to be called with a message only.
//[[noreturn]] inline void throwException(const std::source_location& loc, std::string_view msg)
//{
//    ::gungnir::throwException(loc, msg);
//}
//
///// Overload to allow GUNGNIR_THROW to be called with a format string and arguments.
//template<typename... Args>
//[[noreturn]] inline void throwException(const std::source_location& loc, fmt::format_string<Args...> fmt, Args&&... args)
//{
//    ::gungnir::throwException(loc, fmt::format(fmt, std::forward<Args>(args)...));
//}
//} // namespace detail
//GUNGNIR_NAMESPACE_CLOSE_SCOPE // namespace Gungnir
//
///// Helper for throwing a RuntimeError exception.
///// Accepts either a string or a format string and arguments:
///// GUNGNIR_THROW("This is an error message.");
///// GUNGNIR_THROW("Expected {} items, got {}.", expectedCount, actualCount);
//#define GUNGNIR_THROW(...) ::gungnir::detail::throwException(std::source_location::current(), __VA_ARGS__)
//
///// Helper for throwing a RuntimeError exception if condition isn't met.
///// Accepts either a string or a format string and arguments.
///// GUNGNIR_CHECK(device != nullptr, "Device is null.");
///// GUNGNIR_CHECK(count % 3 == 0, "Count must be a multiple of 3, got {}.", count);
//#define GUNGNIR_CHECK(cond, ...)        \
//    do                                 \
//    {                                  \
//        if (!(cond))                   \
//            GUNGNIR_THROW(__VA_ARGS__); \
//    } while (0)
//
///// Helper for marking unimplemented functions.
//#define GUNGNIR_UNIMPLEMENTED() GUNGNIR_THROW("Unimplemented")
//
///// Helper for marking unreachable code.
//#define GUNGNIR_UNREACHABLE() GUNGNIR_THROW("Unreachable")
//
////
//// Assertions.
////
//
//GUNGNIR_NAMESPACE_OPEN_SCOPE
///// Report an assertion.
///// If ErrorDiagnosticFlags::AppendStackTrace is set, a stack trace will be appended to the exception message.
///// If ErrorDiagnosticFlags::BreakOnAssert is set, the debugger will be broken into (if attached).
//[[noreturn]] GUNGNIR_API void reportAssertion(const std::source_location& loc, std::string_view cond, std::string_view msg = {});
//
//namespace detail
//{
///// Overload to allow GUNGNIR_ASSERT to be called without a message.
//[[noreturn]] inline void reportAssertion(const std::source_location& loc, std::string_view cond)
//{
//    ::gungnir::reportAssertion(loc, cond);
//}
//
///// Overload to allow GUNGNIR_ASSERT to be called with a message only.
//[[noreturn]] inline void reportAssertion(const std::source_location& loc, std::string_view cond, std::string_view msg)
//{
//    ::gungnir::reportAssertion(loc, cond, msg);
//}
//
///// Overload to allow GUNGNIR_ASSERT to be called with a format string and arguments.
//template<typename... Args>
//[[noreturn]] inline void reportAssertion(
//    const std::source_location& loc,
//    std::string_view cond,
//    fmt::format_string<Args...> fmt,
//    Args&&... args
//)
//{
//    ::gungnir::reportAssertion(loc, cond, fmt::format(fmt, std::forward<Args>(args)...));
//}
//} // namespace detail
//GUNGNIR_NAMESPACE_CLOSE_SCOPE // namespace Gungnir
//
//#if GUNGNIR_ENABLE_ASSERTS
//
///// Helper for asserting a condition.
///// Accepts either only the condition, the condition with a string or the condition with a format string and arguments:
///// GUNGNIR_ASSERT(device != nullptr);
///// GUNGNIR_ASSERT(device != nullptr, "Device is null.");
///// GUNGNIR_ASSERT(count % 3 == 0, "Count must be a multiple of 3, got {}.", count);
//#define GUNGNIR_ASSERT(cond, ...)                                                                   \
//    if (!(cond))                                                                                   \
//    {                                                                                              \
//        ::gungnir::detail::reportAssertion(std::source_location::current(), #cond, ##__VA_ARGS__); \
//    }
//
///// Helper for asserting a binary comparison between two variables.
///// Automatically prints the compared values.
//#define GUNGNIR_ASSERT_OP(a, b, OP)                                                                                                       \
//    if (!(a OP b))                                                                                                                       \
//    {                                                                                                                                    \
//        ::gungnir::detail::reportAssertion(std::source_location::current(), fmt::format("{} {} {} ({} {} {})", #a, #OP, #b, a, #OP, b)); \
//    }
//
//#define GUNGNIR_ASSERT_EQ(a, b) GUNGNIR_ASSERT_OP(a, b, ==)
//#define GUNGNIR_ASSERT_NE(a, b) GUNGNIR_ASSERT_OP(a, b, !=)
//#define GUNGNIR_ASSERT_GE(a, b) GUNGNIR_ASSERT_OP(a, b, >=)
//#define GUNGNIR_ASSERT_GT(a, b) GUNGNIR_ASSERT_OP(a, b, >)
//#define GUNGNIR_ASSERT_LE(a, b) GUNGNIR_ASSERT_OP(a, b, <=)
//#define GUNGNIR_ASSERT_LT(a, b) GUNGNIR_ASSERT_OP(a, b, <)
//
//#else // GUNGNIR_ENABLE_ASSERTS
//
//#define GUNGNIR_ASSERT(cond, ...) \
//    {}
//#define GUNGNIR_ASSERT_OP(a, b, OP) \
//    {}
//#define GUNGNIR_ASSERT_EQ(a, b) GUNGNIR_ASSERT_OP(a, b, ==)
//#define GUNGNIR_ASSERT_NE(a, b) GUNGNIR_ASSERT_OP(a, b, !=)
//#define GUNGNIR_ASSERT_GE(a, b) GUNGNIR_ASSERT_OP(a, b, >=)
//#define GUNGNIR_ASSERT_GT(a, b) GUNGNIR_ASSERT_OP(a, b, >)
//#define GUNGNIR_ASSERT_LE(a, b) GUNGNIR_ASSERT_OP(a, b, <=)
//#define GUNGNIR_ASSERT_LT(a, b) GUNGNIR_ASSERT_OP(a, b, <)
//
//#endif // GUNGNIR_ENABLE_ASSERTS
//
////
//// Error reporting.
////
//
//GUNGNIR_NAMESPACE_OPEN_SCOPE
//
///// Flags controlling the error diagnostic behavior.
//enum class ErrorDiagnosticFlags
//{
//    None = 0,
//    /// Break into debugger (if attached) when calling GUNGNIR_THROW.
//    BreakOnThrow,
//    /// Break into debugger (if attached) when calling GUNGNIR_ASSERT.
//    BreakOnAssert,
//    /// Append a stack trace to the exception error message when using GUNGNIR_THROW and GUNGNIR_ASSERT.
//    AppendStackTrace = 2,
//    /// Show a message box when reporting errors using the reportError() functions.
//    ShowMessageBoxOnError = 4,
//};
//GUNGNIR_ENUM_CLASS_OPERATORS(ErrorDiagnosticFlags);
//
///// Set the global error diagnostic flags.
//GUNGNIR_API void setErrorDiagnosticFlags(ErrorDiagnosticFlags flags);
//
///// Get the global error diagnostic flags.
//GUNGNIR_API ErrorDiagnosticFlags getErrorDiagnosticFlags();
//
///**
// * Report an error by logging it and optionally showing a message box.
// * The message box is only shown if ErrorDiagnosticFlags::ShowMessageBoxOnError is set.
// * @param msg Error message.
// */
//GUNGNIR_API void reportErrorAndContinue(std::string_view msg);
//
///**
// * Report an error by logging it and optionally showing a message box with the option to abort or retry.
// * The message box is only shown if ErrorDiagnosticFlags::ShowMessageBoxOnError is set.
// * If not message box is shown, the function always returns false (i.e. no retry).
// * @param msg Error message.
// * @return Returns true if the user chose to retry.
// */
//GUNGNIR_API bool reportErrorAndAllowRetry(std::string_view msg);
//
///**
// * Report a fatal error.
// *
// * The following actions are taken:
// * - The error message is logged together with a stack trace.
// * - If ErrorDiagnosticFlags::ShowMessageBoxOnError is set:
// *   - A message box with the error + stack trace is shown.
// *   - If a debugger is attached there is a button to break into the debugger.
// * - If ErrorDiagnosticFlags::ShowMessageBoxOnError is not set:
// *   - If a debugger is attached there it is broken into.
// * - The application is immediately terminated (std::quick_exit(1)).
// * @param msg Error message.
// */
//[[noreturn]] GUNGNIR_API void reportFatalErrorAndTerminate(std::string_view msg);
//
///// Helper to run a callback and catch/report all exceptions.
///// This is typically used in main() to guard the entire application.
//template<typename CallbackT, typename ResultT = int>
//int catchAndReportAllExceptions(CallbackT callback, ResultT errorResult = 1)
//{
//    ResultT result = errorResult;
//    try
//    {
//        result = callback();
//    }
//    catch (const std::exception& e)
//    {
//        reportErrorAndContinue(std::string("Caught an exception:\n\n") + e.what());
//    }
//    catch (...)
//    {
//        reportErrorAndContinue("Caught an exception of unknown type!");
//    }
//    return result;
//}
//
//GUNGNIR_NAMESPACE_CLOSE_SCOPE
