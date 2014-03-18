//
// dispatch.h
// ~~~~~~~~~~
// Schedule a function to run now if possible, otherwise later.
//
// Copyright (c) 2014 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef EXECUTORS_EXPERIMENTAL_BITS_DISPATCH_H
#define EXECUTORS_EXPERIMENTAL_BITS_DISPATCH_H

#include <experimental/bits/invoker.h>

namespace std {
namespace experimental {

template <class _CompletionToken>
auto dispatch(_CompletionToken&& __token)
{
  async_completion<_CompletionToken, void()> __completion(__token);

  auto __completion_executor(make_executor(__completion.handler));
  __completion_executor.dispatch(std::move(__completion.handler));

  return __completion.result.get();
}

template <class _Func, class _CompletionToken>
auto dispatch(_Func&& __f, _CompletionToken&& __token)
{
  typedef typename decay<_Func>::type _DecayFunc;
  typedef __signature_t<_DecayFunc> _FuncSignature;
  typedef __result_t<_FuncSignature> _Result;
  typedef typename decay<_Result>::type _DecayResult;
  typedef __make_signature_t<void, _Result> _HandlerSignature;
  typedef handler_type_t<_CompletionToken, _HandlerSignature> _Handler;

  async_completion<_CompletionToken, _HandlerSignature> __completion(__token);

  auto __completion_executor(make_executor(__completion.handler));
  (dispatch)(__invoker<_DecayFunc, _Handler, _FuncSignature>{forward<_Func>(__f),
    std::move(__completion.handler), __completion_executor.make_work()});

  return __completion.result.get();
}

template <class _Executor, class _Func, class _CompletionToken>
auto dispatch(_Executor&& __e, _Func&& __f, _CompletionToken&& __token)
{
  return (dispatch)(__e.wrap(forward<_Func>(__f)), forward<_CompletionToken>(__token));
}

} // namespace experimental
} // namespace std

#endif
