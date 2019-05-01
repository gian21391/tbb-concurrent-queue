/*
    Copyright (c) 2005-2019 Intel Corporation

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.




*/

#ifndef TBB_CONCURRENT_QUEUE_TBB_MISC_IMPL_H
#define TBB_CONCURRENT_QUEUE_TBB_MISC_IMPL_H

// Source file for miscellaneous entities that are infrequently referenced by
// an executing program.

#include "../tbb_stddef.h"
//#include "tbb_assert_impl.h" // Out-of-line TBB assertion handling routines are instantiated here.
#include "../tbb_exception.h"
#include "../tbb_machine.h"
#include "tbb_misc.h"
#include "tbb_version.h"

#include <cstdio>
#include <cstdlib>
#include <stdexcept>
#include <cstring>

#if _WIN32||_WIN64
#include "tbb/machine/windows_api.h"
#endif

#define __TBB_STD_RETHROW_EXCEPTION_POSSIBLY_BROKEN                             \
    (__GLIBCXX__ && __TBB_GLIBCXX_VERSION>=40700 && __TBB_GLIBCXX_VERSION<60000 \
     && TBB_USE_EXCEPTIONS && !TBB_USE_CAPTURED_EXCEPTION)

#if __TBB_STD_RETHROW_EXCEPTION_POSSIBLY_BROKEN
// GCC ABI declarations necessary for a workaround
#include <cxxabi.h>
#endif

namespace tbb::internal
{

#if TBB_USE_EXCEPTIONS
#define DO_THROW(exc, init_args) throw exc init_args;
#else /* !TBB_USE_EXCEPTIONS */
#define PRINT_ERROR_AND_ABORT(exc_name, msg) \
        fprintf (stderr, "Exception %s with message %s would've been thrown, "  \
            "if exception handling were not disabled. Aborting.\n", exc_name, msg); \
        fflush(stderr); \
        std::abort();
    #define DO_THROW(exc, init_args) PRINT_ERROR_AND_ABORT(#exc, #init_args)
#endif /* !TBB_USE_EXCEPTIONS */


/* The "what" should be fairly short, not more than about 128 characters.
   Because we control all the call sites to handle_perror, it is pointless
   to bullet-proof it for very long strings.

   Design note: ADR put this routine off to the side in tbb_misc.cpp instead of
   Task.cpp because the throw generates a pathetic lot of code, and ADR wanted
   this large chunk of code to be placed on a cold page. */
void handle_perror( int error_code, const char* what ) {
  char buf[256];
#if _MSC_VER
#define snprintf _snprintf
#endif
  int written = snprintf(buf, sizeof(buf), "%s: %s", what, strerror( error_code ));
  // On overflow, the returned value exceeds sizeof(buf) (for GLIBC) or is negative (for MSVC).
  __TBB_ASSERT_EX( written>0 && written<(int)sizeof(buf), "Error description is too long" );
  // Ensure that buffer ends in terminator.
  buf[sizeof(buf)-1] = 0;
#if TBB_USE_EXCEPTIONS
  throw std::runtime_error(buf);
#else
  PRINT_ERROR_AND_ABORT( "runtime_error", buf);
#endif /* !TBB_USE_EXCEPTIONS */
}

#if _WIN32||_WIN64
void handle_win_error( int error_code ) {
    char buf[512];
#if !__TBB_WIN8UI_SUPPORT
    FormatMessageA( FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                    NULL, error_code, 0, buf, sizeof(buf), NULL );
#else
//TODO: update with right replacement for FormatMessageA
    sprintf_s((char*)&buf, 512, "error code %d", error_code);
#endif
#if TBB_USE_EXCEPTIONS
    throw std::runtime_error(buf);
#else
    PRINT_ERROR_AND_ABORT( "runtime_error", buf);
#endif /* !TBB_USE_EXCEPTIONS */
}
#endif // _WIN32||_WIN64

#if __TBB_STD_RETHROW_EXCEPTION_POSSIBLY_BROKEN
// Runtime detection and workaround for the GCC bug 62258.
// The problem is that std::rethrow_exception() does not increment a counter
// of active exceptions, causing std::uncaught_exception() to return a wrong value.
// The code is created after, and roughly reflects, the workaround
// at https://gcc.gnu.org/bugzilla/attachment.cgi?id=34683

void fix_broken_rethrow() {
    struct gcc_eh_data {
        void *       caughtExceptions;
        unsigned int uncaughtExceptions;
    };
    gcc_eh_data* eh_data = punned_cast<gcc_eh_data*>( abi::__cxa_get_globals() );
    ++eh_data->uncaughtExceptions;
}

bool gcc_rethrow_exception_broken() {
    bool is_broken;
    __TBB_ASSERT( !std::uncaught_exception(),
        "gcc_rethrow_exception_broken() must not be called when an exception is active" );
    try {
        // Throw, catch, and rethrow an exception
        try {
            throw __TBB_GLIBCXX_VERSION;
        } catch(...) {
            std::rethrow_exception( std::current_exception() );
        }
    } catch(...) {
        // Check the bug presence
        is_broken = std::uncaught_exception();
    }
    if( is_broken ) fix_broken_rethrow();
    __TBB_ASSERT( !std::uncaught_exception(), NULL );
    return is_broken;
}
#else
void fix_broken_rethrow() {}
bool gcc_rethrow_exception_broken() { return false; }
#endif /* __TBB_STD_RETHROW_EXCEPTION_POSSIBLY_BROKEN */

/** The leading "\0" is here so that applying "strings" to the binary delivers a clean result. */
static const char VersionString[] = "\0"
TBB_VERSION_STRINGS;

static bool PrintVersionFlag = false;

void PrintVersion()
{
  PrintVersionFlag = true;
  fputs(VersionString + 1, stderr);
}

void PrintExtraVersionInfo(const char *category, const char *format, ...)
{
  if ( PrintVersionFlag )
  {
    char str[1024];
    memset(str, 0, 1024);
    va_list args;
    va_start(args, format);
    // Note: correct vsnprintf definition obtained from tbb_assert_impl.h
    vsnprintf(str, 1024 - 1, format, args);
    va_end(args);
    fprintf(stderr, "TBB: %s\t%s\n", category, str);
  }
}

void PrintRMLVersionInfo(void *arg, const char *server_info)
{
  PrintExtraVersionInfo(server_info, (const char *) arg);
}

//! check for transaction support.
#if _MSC_VER
#include <intrin.h> // for __cpuid
#endif
bool cpu_has_speculation() {
#if __TBB_TSX_AVAILABLE
#if (__INTEL_COMPILER || __GNUC__ || _MSC_VER || __SUNPRO_CC)
  bool result = false;
  const int rtm_ebx_mask = 1<<11;
#if _MSC_VER
  int info[4] = {0,0,0,0};
    const int reg_ebx = 1;
    __cpuidex(info, 7, 0);
    result = (info[reg_ebx] & rtm_ebx_mask)!=0;
#elif __GNUC__ || __SUNPRO_CC
  int32_t reg_ebx = 0;
  int32_t reg_eax = 7;
  int32_t reg_ecx = 0;
  __asm__ __volatile__ ( "movl %%ebx, %%esi\n"
                         "cpuid\n"
                         "movl %%ebx, %0\n"
                         "movl %%esi, %%ebx\n"
  : "=a"(reg_ebx) : "0" (reg_eax), "c" (reg_ecx) : "esi",
#if __TBB_x86_64
  "ebx",
#endif
  "edx"
  );
  result = (reg_ebx & rtm_ebx_mask)!=0 ;
#endif
  return result;
#else
#error Speculation detection not enabled for compiler
#endif /* __INTEL_COMPILER || __GNUC__ || _MSC_VER */
#else  /* __TBB_TSX_AVAILABLE */
  return false;
#endif /* __TBB_TSX_AVAILABLE */
}

}

#endif //TBB_CONCURRENT_QUEUE_TBB_MISC_IMPL_H
