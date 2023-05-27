#pragma once

#include "UpdatableModule.hpp"

namespace xenomods {
    struct AppletRedirect : public xenomods::UpdatableModule {
        struct ConfigState {
            bool useRecentGame;
        };

        static ConfigState Config;

        void Initialize() override;
        static bool IsMeliaRecentGame();
    };
}
