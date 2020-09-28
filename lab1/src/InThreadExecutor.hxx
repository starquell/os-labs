#pragma once

#include "InThreadExecutor.hpp"

#include <boost/callable_traits/args.hpp>
#include <iostream>

template <typename Func>
inline lab::InThreadExecutor<Func>::InThreadExecutor(Func func) noexcept
    : _func{std::move(func)}
{}

template<typename Func>
auto lab::InThreadExecutor<Func>::wait() noexcept -> void
{
    if (_thread && _thread->joinable()) {
        _thread->join();
    }
}

template<typename Func>
auto lab::InThreadExecutor<Func>::stop()  -> void
{
    if (_thread && running()) {
        _running = false;
        _thread->detach();
        _thread = std::nullopt;
    }
}

template <typename Func>
auto lab::InThreadExecutor<Func>::running() const noexcept -> bool
{
    return _running;
}

template<typename Func>
auto lab::InThreadExecutor<Func>::run(auto... args) noexcept -> void
{
    _running = true;
    try {
        _thread = std::jthread{[&, ...args = std::move(args)] {
            try {
                auto result = _func(std::forward<decltype(args)>(args)...);
                if (_running) {
                    _running = false;
                    this->success_handler()(result);
                }
            }
            catch (const std::exception &err) {
                if (_running) {
                    _running = false;
                    this->fail_handler()(err.what());
                }
            }
        }};
    }
    catch (const std::system_error& err) {
        if (_running) {
            _running = false;
            this->fail_handler()("System is unable to run this function");
        }
    }
}

template<typename Func>
lab::InThreadExecutor<Func>::InThreadExecutor(const lab::InThreadExecutor<Func> &other)
    : _func{other._func},
      _running {other.running()},
      _thread{}
{}

template <typename Func>
auto lab::InThreadExecutor<Func>::operator=(const InThreadExecutor<Func> &other) -> lab::InThreadExecutor<Func>&
{
   _func = other._func;
   _running = other.running();
   _thread = decltype(_thread) {};

   return *this;
}
