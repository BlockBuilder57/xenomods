// Created by block on 5/28/23.

#pragma once

#include <string>
#include <xenomods/HidInput.hpp>

namespace xenomods {

	struct OptionBase {
		virtual bool Update(HidInput* input) {
			if (input->InputUpStrict(Keybind::MENU_BACK)) {
				selected = false;
				return false;
			}

			return true;
		};

		template<class T>
		explicit constexpr OptionBase(T& v, const std::string& name)
			: value(&v), name(name) {
		}

		virtual std::string String() const { return "nothing?"; };

		void SetName(const std::string& name) {
			this->name = name;
		}

		constexpr std::string_view GetName() const {
			return name;
		}

		void SetSelected() {
			selected = true;
		}
		bool IsSelected() {
			return selected;
		}

	   protected:
		bool selected { false };
		std::string name;

		template<class T>
		constexpr T& ValueAs() {
			return *std::bit_cast<T*>(value);
		}

		template<class T>
		constexpr const T& ValueAs() const {
			return *std::bit_cast<const T*>(value);
		}

	   private:
		void* value;
	};

	template<class T>
	struct Option;


	template<>
	struct Option<float> : OptionBase {
		explicit constexpr Option(float& f, const std::string& name)
			: OptionBase(f, name) {
		}

		bool Update(HidInput* input) override;
		std::string String() const override;

	   private:
		int tens;
		bool by2;
		bool by10s;
	};


} // namespace xenomods