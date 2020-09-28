#pragma once

/////// Variant with Boost.Process and unnamed pipes, not implemented fully

/*
#include "Utils.hpp"

#include <functional>
#include <filesystem>
#include <string_view>

#include <boost/process.hpp>
#include <boost/asio/io_service.hpp>

namespace fs = std::filesystem;
namespace bp = boost::process;

namespace lab {

    template <typename ResultType>
    class ProcessRunner {
    public:

        using SuccessHandler = std::function<void(ResultType)>;
        using FailHandler = std::function<void(std::string_view)>;

        explicit ProcessRunner(fs::path path) noexcept;

        auto run (auto&&... args) noexcept -> void;

        auto on_success (const SuccessHandler &callback) -> void;
        auto on_success (SuccessHandler&& callback) noexcept -> void;

        auto on_fail (const FailHandler& handler) -> void;
        auto on_fail (FailHandler&& handler) noexcept -> void;

        auto wait() noexcept -> void;
        auto stop() noexcept -> void;

        ~ProcessRunner() noexcept;

    private:
        void handle_success(const auto& error, const std::size_t bytes);
        void handle_exit (const int code, const std::error_code& err);
        void handle_error (auto& executor, const std::error_code& err)
    private:
        fs::path _path;
        SuccessHandler _on_success;
        FailHandler _on_fail;

        struct ExecutionControl {
            static inline boost::asio::io_context context{};
            bp::async_pipe pipe {context};
            std::optional<bp::child> proc;
            std::array <std::byte, sizeof(ResultType)> buf {};
            bool data_read = false;
        };

        ExecutionControl _execution;
    };
}

#include "ProcessRunner.hxx"