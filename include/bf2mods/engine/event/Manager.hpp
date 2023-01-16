//
// Created by block on 7/28/2022.
//

#pragma once

namespace event {
	class Manager {
	   public:
		void update();
		void drawInfo();
		bool isPlayCancel() const;
	};
} // namespace event