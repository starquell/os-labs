#pragma once

#include <functional>
#include <optional>
#include <thread>

#ifdef __linux__
#include <unistd.h>
#include <termios.h>
#endif

namespace lab {

    /**
     *  @brief Concept of class that represents cancelator used for cancel running of ComputationManager
     */
    template <typename T>
    concept Cancelator = requires (T t) {
        typename T::Callback;
        t.on_cancel(std::declval<typename T::Callback>());
        t.start_monitoring();
        t.stop_monitoring();
        {t.canceled()} -> std::convertible_to<bool>;
    };

    /**
     *  @brief Waits that invokes callback after specific key press in other thread
     */
    class SimpleKeyCancelator {

    public:
        using Key = int;
        using Callback = std::function<void()>;

        explicit SimpleKeyCancelator (Key key) noexcept;

        SimpleKeyCancelator (SimpleKeyCancelator&& other) noexcept;

        /**
         *  @brief Setter for callback function that will ne invoked after key pressed
         */
        auto on_cancel (const Callback& callback) -> void;

        /**
         *  @brief Starts checking for key press in separate thread
         */
        auto start_monitoring() -> void;

        /**
         *  @brief Stops checking for key press (only after start_monitoring invoked)
         */
        auto stop_monitoring() noexcept -> bool;

        /**
         *  @return True if cancelation key pressed
         */
        [[nodiscard]]
        auto canceled() const noexcept -> bool;

    private:
        Key _key;
        std::optional<Callback> _callback;
        std::optional<std::jthread> _thread;
        std::atomic<bool> _canceled = false;
        termios _terminal;
    };
}