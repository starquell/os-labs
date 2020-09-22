#pragma once

#include "ProcessRunner.hpp"

#include <future>
#include <iostream>
#include <system_error>
#include <sstream>

#include <boost/asio/io_service.hpp>

template<typename ResultType>
inline lab::ProcessRunner<ResultType>::ProcessRunner(fs::path path) noexcept
    : _path{std::move(path)}
{}

template<typename ResultType>
inline void lab::ProcessRunner<ResultType>::on_success (const SuccessHandler& callback)
{
    _on_success = callback;
}

template<typename ResultType>
inline void lab::ProcessRunner<ResultType>::run(auto&&... args) noexcept {

    static const auto stringify = [] (const auto& x) {
            static std::stringstream ss;
            ss << x;
            const auto result = std::move(*ss.rdbuf()).str();
            ss.str(std::string{});
            return result;
    };
    try {
        _execution.proc = bp::child {fs::absolute(_path).string(),
                                     stringify(std::forward<decltype(args)>(args))...,
                                     bp::std_out > _execution.pipe};

        if (!_execution.proc->valid()) {
            _on_fail("System is unable to run this function");
            return;
        }

        boost::asio::async_read(
                _execution.pipe,
                boost::asio::buffer(_execution.buf),
                [&] (const auto& error, const std::size_t size) {
                    if (!error) {
                        ResultType result{};
                        std::memcpy(&result, _execution.buf.data(), size);
                        _on_success(result);
                    }
                    else if (error && error != boost::asio::error::eof) {
                       _on_fail(error.message());
                    }
                    _execution = ExecutionControl{};
                });

        std::async (std::launch::async, [&] {_execution.context.run();});
    }
    catch (const std::exception& err) {
        _on_fail(err.what());
    }
}

template<typename ResultType>
inline void lab::ProcessRunner<ResultType>::on_fail(const FailHandler& handler)
{
    _on_fail = handler;
}

template<typename ResultType>
inline void lab::ProcessRunner<ResultType>::stop() noexcept
{
    if (!_execution.proc) {
        return;
    }
    _execution.proc->terminate();
    _execution = ExecutionControl{};
}

template<typename ResultType>
inline lab::ProcessRunner<ResultType>::~ProcessRunner() noexcept {
    if (!_execution.proc->running()) {
        return;
    }
    stop();
    _on_fail("");
}

template<typename ResultType>
inline auto lab::ProcessRunner<ResultType>::on_success(SuccessHandler &&callback) noexcept -> void
{
    _on_success = std::move(callback);
}

template<typename ResultType>
inline auto lab::ProcessRunner<ResultType>::on_fail(FailHandler &&handler) noexcept -> void
{
    _on_fail = std::move(handler);
}

template<typename ResultType>
inline auto lab::ProcessRunner<ResultType>::wait() noexcept -> void
{
    if (!_execution.proc) {
        return;
    }
    _execution.proc->wait();
}



