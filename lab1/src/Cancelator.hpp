#pragma once

#include <condition_variable>
#include <optional>
#include <thread>
#include <mutex>

#ifdef __linux__
#include <unistd.h>
#include <termios.h>
#endif

namespace lab {

    /**
     *  @brief Waits that invokes callback after specific key press in other thread
     */
    class SimpleKeyCancelator {

    public:
        using Key = int;

        explicit SimpleKeyCancelator (Key key) noexcept;

        SimpleKeyCancelator (SimpleKeyCancelator&& other) noexcept;

        /**
         *  @brief Starts checking for key press in separate thread
         */
        auto start_monitoring(std::condition_variable& cv, std::mutex& mut) -> void;

        /**
         *  @brief Stops checking for key press (only after start_monitoring invoked)
         */
        auto stop_monitoring() noexcept -> bool;

        auto key() const noexcept -> Key;

        /**
         *  @return True if cancelation key pressed
         */
        [[nodiscard]]
        auto canceled() const noexcept -> bool;

    private:
        Key _key;
        std::optional<std::jthread> _thread;
        bool _canceled = false;
        termios _terminal;
    };
}