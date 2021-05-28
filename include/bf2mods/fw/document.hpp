#pragma once

#include "input_manager.hpp"

namespace fw {

class Document {
public:
  InputManager &getInputManager() const;
};

} // namespace fw
