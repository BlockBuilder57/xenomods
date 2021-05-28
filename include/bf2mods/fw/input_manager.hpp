#pragma once

#include "../mm/math_types.hpp"

namespace fw {

enum class InputStatus { DOWN, UP, HELD, RELEASED };

class InputManager {
public:
  bool testKeyBoardStatus(fw::InputStatus inputStatus,
                          char const *keyStr) const;
  bool testMouseStatus(fw::InputStatus inputStatus, char const *keyStr) const;
  mm::Pnt *getMouseMovement() const;
};

} // namespace fw
