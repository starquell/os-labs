#include "Cancelator.hpp"
 namespace {

#ifdef __linux__

    auto replace_terminal() noexcept -> termios {
        termios old_tio{};      // for storing settings from old terminal
        tcgetattr(STDIN_FILENO, &old_tio);  // save old terminal

        termios new_tio{old_tio};
        new_tio.c_lflag &= (~ICANON & ~ECHO);// disable canonical mode (buffered i/o) and local echo
        tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);  /* set the new settings */

        return old_tio;
    }

    auto set_terminal(const termios &terminal) -> void {
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
              _callback {other._callback},
              _thread {std::nullopt},
              _canceled {false}
    {}

    auto SimpleKeyCancelator::on_cancel(const SimpleKeyCancelator::Callback &callback) -> void {
        _callback = callback;
    }

    auto SimpleKeyCancelator::start_monitoring() -> void
    {
        if (_thread || !_callback) {
            return;
        }
#ifdef __linux__
        _terminal = replace_terminal();
#endif
        _thread = std::jthread{[&] (std::stop_token stop) {

            while (true) {
                if (getchar() == _key) {
                    (*_callback)();
                    break;
                }
                if (stop.stop_requested()) {
                    break;
                }
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
}