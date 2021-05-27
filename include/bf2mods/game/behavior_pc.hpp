#pragma once

#include "../fw/update_info.hpp"
#include "../mm/math_types.hpp"
#include "../utils.hpp"

namespace game {

class BehaviorPc {
   public:
    struct PACKED WarpSettings {
        mm::Vec3 pos;
        INSERT_PADDING_BYTES(0x20 - 0xC);
        int32_t isWarpAfter;
        INSERT_PADDING_BYTES(2);
    };
    STATIC_ASSERT_SIZE(WarpSettings, 0x26);

    int getFormationIdx() const;
    bool getLeaderPos(mm::Vec3&) const;

    void setWarp(game::BehaviorPc::WarpSettings const&);

    void update(fw::UpdateInfo const&);
};

}  // namespace game
