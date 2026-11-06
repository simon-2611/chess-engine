#pragma once

#include <exception>

namespace chess {

// Thrown from QtUI::requestHumanMove when the GUI cancels a blocking human
// input wait (undo / new game / resign / quit). Caught by QtGameRunner so the
// existing UserInterface signature can stay blocking and unchanged.
class InputCancelled : public std::exception {
public:
    enum class Reason {
        Undo,
        NewGame,
        Resign,
        Quit
    };

    explicit InputCancelled(Reason reason) : reason_(reason) {}

    Reason reason() const noexcept { return reason_; }

    const char* what() const noexcept override {
        switch (reason_) {
            case Reason::Undo: return "input cancelled: undo";
            case Reason::NewGame: return "input cancelled: new game";
            case Reason::Resign: return "input cancelled: resign";
            case Reason::Quit: return "input cancelled: quit";
        }
        return "input cancelled";
    }

private:
    Reason reason_;
};

} // namespace chess
