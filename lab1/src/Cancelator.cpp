#include "Cancelator.hpp"

#ifdef __linux__
#include <fcntl.h>
#endif

namespace {

#ifdef __linux__

    auto replace_terminal() noexcept -> termios
    {
        termios old_tio{};      // for storing settings from old terminal
        tcgetattr(STDIN_FILENO, &old_tio);  // save old terminal

        termios new_tio{old_tio};
        new_tio.c_lflag &= (~ICANON & ~ECHO);// disable canonical mode (buffered i/o) and local echo

        const int flags = fcntl(STDOUT_FILENO, F_GETFL);
        fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
        tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);  /* set the new settings */

        return old_tio;
    }

    auto set_terminal(const termios &terminal) -> void
    {
        const int flags = fcntl(STDOUT_FILENO, F_GETFL);
        fcntl(STDIN_FILENO, F_SETFL, flags | ~O_NONBLOCK);
        tcsetattr(STDIN_FILENO, TCSANOW, &terminal);
    }

#endif
}

namespace lab {

    SimpleKeyCancelator::SimpleKeyCancelator(SimpleKeyCancelator::Key key) noexcept
            : _key {key}
    {}

    SimpleKeyCancelator::SimpleKeyCancelator(SimpleKeyCancelator &&other) noexcept
            : _key {other._key},
              _thread {std::nullopt},
              _canceled {false}
    {}

    auto SimpleKeyCancelator::start_monitoring(std::condition_variable& cv, std::mutex& mut) -> void
    {
        if (_thread) {
            return;
        }
#ifdef __linux__
        _terminal = replace_terminal();
#endif
        _thread = std::jthread{[&] (std::stop_token stop) {

            while (true) {
                if (getchar() == _key) {
                    {
                        std::scoped_lock lock{mut};
                        _canceled = true;
                    }
                    cv.notify_one();
                    break;
                }
                if (stop.stop_requested()) {
                    break;
                }
                std::this_thread::yield();
            }
#ifdef __linux__
            set_terminal(_terminal);
#endif
        }};
    }

    auto SimpleKeyCancelator::stop_monitoring() noexcept -> bool {
        if (_thread) {
            _canceled = _thread->request_stop();
            _thread->detach();
#ifdef __linux__
            set_terminal(_terminal);
#endif
            return _canceled;
        }
        return false;
    }

    auto SimpleKeyCancelator::canceled() const noexcept -> bool {
        return _canceled;
    }

    auto SimpleKeyCancelator::key() const noexcept -> SimpleKeyCancelator::Key
    {
        return _key;
    }
}