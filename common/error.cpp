#include "common/error.h"

#include "platform/OS.h"
//#include "Utils/Logger.h"
#include <fmt/core.h>
#include <atomic>

GUNGNIR_NAMESPACE_OPEN_SCOPE

/// Global error diagnostic flags.
static ErrorDiagnosticFlags gErrorDiagnosticFlags = ErrorDiagnosticFlags::BreakOnThrow | ErrorDiagnosticFlags::BreakOnAssert;

void throwException(const std::source_location& loc, std::string_view msg)
{
   std::string fullMsg = format("{}\n\n{}:{} ({})", msg, loc.file_name(), loc.line(), loc.function_name());

   if (is_set(gErrorDiagnosticFlags, ErrorDiagnosticFlags::AppendStackTrace))
       fullMsg += format("\n\nStacktrace:\n{}", getStackTrace(1));

   if (is_set(gErrorDiagnosticFlags, ErrorDiagnosticFlags::BreakOnThrow) && isDebuggerPresent())
       debugBreak();

   throw RuntimeError(fullMsg);
}

void reportAssertion(const std::source_location& loc, std::string_view cond, std::string_view msg)
{
   std::string fullMsg = format(
       "Assertion failed: {}\n{}{}\n{}:{} ({})", cond, msg, msg.empty() ? "" : "\n", loc.file_name(), loc.line(), loc.function_name()
   );

   if (is_set(gErrorDiagnosticFlags, ErrorDiagnosticFlags::AppendStackTrace))
       fullMsg += format("\n\nStacktrace:\n{}", getStackTrace(1));

   if (is_set(gErrorDiagnosticFlags, ErrorDiagnosticFlags::BreakOnAssert) && isDebuggerPresent())
       debugBreak();

   throw AssertionError(fullMsg);
}

//
// Error handling.
//

void setErrorDiagnosticFlags(ErrorDiagnosticFlags flags)
{
   gErrorDiagnosticFlags = flags;
}

ErrorDiagnosticFlags getErrorDiagnosticFlags()
{
   return gErrorDiagnosticFlags;
}

void reportErrorAndContinue(std::string_view msg)
{
    //TODO: Add log
    //logError(msg);

   if (is_set(gErrorDiagnosticFlags, ErrorDiagnosticFlags::ShowMessageBoxOnError))
   {
       // Show message box
       msgBox("Error", std::string(msg), MsgBoxType::Ok, MsgBoxIcon::Error);
   }
}

bool reportErrorAndAllowRetry(std::string_view msg)
{
    //TODO: Add log
   //logError(msg);

   if (is_set(gErrorDiagnosticFlags, ErrorDiagnosticFlags::ShowMessageBoxOnError))
   {
       enum ButtonId
       {
           Retry,
           Abort
       };

       // Setup message box buttons
       std::vector<MsgBoxCustomButton> buttons;
       buttons.push_back({Retry, "Retry"});
       buttons.push_back({Abort, "Abort"});

       // Show message box
       auto result = msgBox("Error", std::string(msg), buttons, MsgBoxIcon::Error);
       return result == Retry;
   }

   return false;
}

[[noreturn]] void reportFatalErrorAndTerminate(std::string_view msg)
{
   // Immediately terminate on re-entry.
   static std::atomic<bool> entered;
   if (entered.exchange(true) == true)
       std::quick_exit(1);

   std::string fullMsg = format("{}\n\nStacktrace:\n{}", msg, getStackTrace(3));

    //TODO: Add log
   //logFatal(fullMsg);

   if (is_set(gErrorDiagnosticFlags, ErrorDiagnosticFlags::ShowMessageBoxOnError))
   {
       enum ButtonId
       {
           Debug,
           Abort
       };

       // Setup message box buttons
       std::vector<MsgBoxCustomButton> buttons;
       if (isDebuggerPresent())
           buttons.push_back({Debug, "Debug"});
       buttons.push_back({Abort, "Abort"});

       // Show message box
       auto result = msgBox("Gungnir Error", fullMsg, buttons, MsgBoxIcon::Error);
       if (result == Debug)
           debugBreak();
   }
   else
   {
       if (isDebuggerPresent())
           debugBreak();
   }

   std::quick_exit(1);
}

GUNGNIR_NAMESPACE_CLOSE_SCOPE // namespace Gungnir
