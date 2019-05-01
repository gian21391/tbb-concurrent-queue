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

#ifndef TBB_CONCURRENT_QUEUE_TBB_EXCEPTION_IMPL_H
#define TBB_CONCURRENT_QUEUE_TBB_EXCEPTION_IMPL_H

#include <stdexcept>

namespace tbb {

const char* bad_last_alloc::what() const throw() { return "bad allocation in previous or concurrent attempt"; }
const char* improper_lock::what() const throw() { return "attempted recursive lock on critical section or non-recursive mutex"; }
const char* user_abort::what() const throw() { return "User-initiated abort has terminated this operation"; }
const char* invalid_multiple_scheduling::what() const throw() { return "The same task_handle object cannot be executed more than once"; }
const char* missing_wait::what() const throw() { return "wait() was not called on the structured_task_group"; }

}

namespace tbb {
namespace internal {

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

void throw_exception_v4(exception_id eid)
{
  __TBB_ASSERT (eid > 0 && eid < eid_max, "Unknown exception ID");
  switch ( eid )
  {
    case eid_bad_alloc:
      DO_THROW(std::bad_alloc, ());
    case eid_bad_last_alloc:
      DO_THROW(bad_last_alloc, ());
    case eid_nonpositive_step:
      DO_THROW(std::invalid_argument, ("Step must be positive"));
    case eid_out_of_range:
      DO_THROW(std::out_of_range, ("Index out of requested size range"));
    case eid_segment_range_error:
      DO_THROW(std::range_error, ("Index out of allocated segment slots"));
    case eid_index_range_error:
      DO_THROW(std::range_error, ("Index is not allocated"));
    case eid_missing_wait:
      DO_THROW(missing_wait, ());
    case eid_invalid_multiple_scheduling:
      DO_THROW(invalid_multiple_scheduling, ());
    case eid_improper_lock:
      DO_THROW(improper_lock, ());
    case eid_possible_deadlock:
      DO_THROW(std::runtime_error, ("Resource deadlock would occur"));
    case eid_operation_not_permitted:
      DO_THROW(std::runtime_error, ("Operation not permitted"));
    case eid_condvar_wait_failed:
      DO_THROW(std::runtime_error, ("Wait on condition variable failed"));
    case eid_invalid_load_factor:
      DO_THROW(std::out_of_range, ("Invalid hash load factor"));
    case eid_reserved:
      DO_THROW(std::out_of_range, ("[backward compatibility] Invalid number of buckets"));
    case eid_invalid_swap:
      DO_THROW(std::invalid_argument, ("swap() is invalid on non-equal allocators"));
    case eid_reservation_length_error:
      DO_THROW(std::length_error, ("reservation size exceeds permitted max size"));
    case eid_invalid_key:
      DO_THROW(std::out_of_range, ("invalid key"));
    case eid_user_abort:
      DO_THROW(user_abort, ());
    case eid_bad_tagged_msg_cast:
      DO_THROW(std::runtime_error, ("Illegal tagged_msg cast"));
#if __TBB_SUPPORTS_WORKERS_WAITING_IN_TERMINATE
      case eid_blocking_thread_join_impossible: DO_THROW(std::runtime_error, ("Blocking terminate failed") );
#endif
    default:
      break;
  }
#if !TBB_USE_EXCEPTIONS && __APPLE__
  out_of_range e1("");
    length_error e2("");
    range_error e3("");
    invalid_argument e4("");
#endif /* !TBB_USE_EXCEPTIONS && __APPLE__ */
}

}
}

#endif //TBB_CONCURRENT_QUEUE_TBB_EXCEPTION_IMPL_H
