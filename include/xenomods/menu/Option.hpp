// Created by block on 5/28/23.

#pragma once

#include <string>
#include <xenomods/HidInput.hpp>

namespace xenomods {

	struct OptionBase {

		template<class T>
		explicit constexpr OptionBase(T& v, const std::string& name, void(*callback)())
			: value(&v), name(name), callback(callback) {
		}

		template<class T>
		explicit constexpr OptionBase(T& v, const std::string& name)
			: OptionBase(v, name, nullptr) {
		}

		virtual bool Update(HidInput* input);
		void Callback() {
			if (callback != nullptr)
				callback();
		};

		virtual std::string String() const = 0;

		void SetName(const std::string& name);

		constexpr std::string_view GetName() const {
			return name;
		}

		void SetSelected();

		constexpr bool IsSelected() const {
			return selected;
		}

	   protected:
		bool selected { false };
		std::string name {};

		template<class T>
		constexpr T& ValueAs() {
			return *std::bit_cast<T*>(value);
		}

		template<class T>
		constexpr const T& ValueAs() const {
			return *std::bit_cast<const T*>(value);
		}

	   private:
		void* value {};
		void(*callback)() {};
	};

	template<class T>
	struct Option;


	template<>
	struct Option<void> : OptionBase {
		explicit constexpr Option(const std::string& name, void(*callback)())
			: OptionBase(dummy, name, callback) {
		}

		bool Update(HidInput* input) override {
			// automatically return so we run the callback
			selected = false;
			return true;
		}

		std::string String() const override {
			return name;
		}

	   private:
		struct Dummy{} dummy;
	};

	template<class T> requires(std::is_arithmetic_v<T>)
	struct Option<T> : OptionBase {
		explicit constexpr Option(T& f, const std::string& name, void(*callback)())
			: OptionBase(f, name, callback) {
		}

		explicit constexpr Option(T& f, const std::string& name)
			: OptionBase(f, name) {
		}

		bool Update(HidInput* input) override {
			if(!OptionBase::Update(input))
				return false;

			bool changed = false;

			T& val = ValueAs<T>();

			tens = std::log10(std::abs(val));
			by2 = input->InputHeld(Keybind::MENU_NUM_MULT2);
			by10s = input->InputHeld(Keybind::MENU_NUM_TENS);

			// don't let the log return 0
			if (tens <= 0)
				tens = 1;

			if(input->InputDown(Keybind::MENU_NUM_INC)) {
				if(by2)
					val *= 2;
				else if(by10s && tens != 0)
					val += std::pow(10, tens);
				else
					val++;

				changed = true;
			} else if(input->InputDown(Keybind::MENU_NUM_DEC)) {
				if(by2)
					val /= 2;
				else if(by10s && tens != 0)
					val -= std::pow(10, tens);
				else
					val--;

				changed = true;
			}

			if (input->InputDown(Keybind::MENU_NUM_NEGATE)) {
				val = -val;
				changed = true;
			}

			return changed;
		}

		std::string String() const override {
			std::string extra;

			if (selected) {
				if (!by2 && !by10s) {
					extra = " \x81\x7D""1"; // +- 1
				}
				else if (by2) {
					extra = " \x81\x7E/\x81\x80""2"; // x/div 2
				}
				else if (by10s) {
					// 0x7D is }, so it gets mad when trying to emit it on its own
					extra = fmt::format(" {}{}", "\x81\x7D", std::pow(10, tens)); // +- 10^tens
				}
			}

			return fmt::format("{}: {}{}", name, OptionBase::ValueAs<T>(), extra);
		}

	   private:
		int tens;
		bool by2;
		bool by10s;
	};

	template<>
	struct Option<bool> : OptionBase {
		explicit constexpr Option(bool& f, const std::string& name, void(*callback)())
			: OptionBase(f, name, callback) {
		}

		explicit constexpr Option(bool& f, const std::string& name)
			: OptionBase(f, name) {
		}

		bool Update(HidInput* input) override {
			if(!OptionBase::Update(input))
				return false;

			bool changed = false;

			bool& val = ValueAs<bool>();

			if (input->InputDown(Keybind::MENU_NUM_INC) || input->InputDown(Keybind::MENU_NUM_DEC) || input->InputDown(Keybind::MENU_NUM_NEGATE)) {
				val = !val;
				changed = true;
			}

			return changed;
		}

		std::string String() const override {
			return fmt::format("{}: {}", name, ValueAs<bool>());
		}
	};


} // namespace xenomods