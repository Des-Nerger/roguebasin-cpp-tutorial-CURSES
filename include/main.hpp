#pragma once

#include <Engine.hpp>
#include <assert.h>

#define ok(result) assert(OK == (result))

extern Engine engine;

template <class Lambda> class AtScopeExit {
  Lambda& m_lambda;
public:
  AtScopeExit(Lambda& action) : m_lambda(action) {}
  ~AtScopeExit() { m_lambda(); }
};

#define defer_INTERNAL2(lname, aname, ...) \
    auto lname = [&]() { __VA_ARGS__; }; \
    AtScopeExit<decltype(lname)> aname(lname);

#define defer_TOKENPASTE(x, y) defer_ ## x ## y

#define defer_INTERNAL1(ctr, ...) \
    defer_INTERNAL2(defer_TOKENPASTE(func_, ctr), \
                   defer_TOKENPASTE(instance_, ctr), __VA_ARGS__)

#define defer(...) defer_INTERNAL1(__COUNTER__, __VA_ARGS__)
