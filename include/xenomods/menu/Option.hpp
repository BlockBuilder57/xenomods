// Created by block on 5/28/23.

#pragma once

#include <string>
#include <xenomods/HidInput.hpp>

namespace xenomods {

	/**
	 * Base class for menu options.
	 */
	struct OptionBase {
		template<class T>
		explicit constexpr OptionBase(T& v, const std::string& name, void (*callback)())
			: value(&v), name(name), callback(callback) {
		}

		template<class T>
		explicit constexpr OptionBase(T& v, const std::string& name)
			: OptionBase(v, name, nullptr) {
		}

		/**
		 * Called by the menu system when the option is selected.
		 *
		 * \param input controller input used by the Menu
		 * \return true if the provided callback should be called; false otherwise
		 */
		virtual bool Update(HidInput* input);

		void Callback() {
			if(callback != nullptr)
				callback();
		}

		/**
		 *
		 * \return A string representation of the option. Usually shows the option's value and any other controls.
		 */
		virtual std::string String() const = 0;

		void SetName(const std::string& name);

		constexpr std::string_view GetName() const {
			return name;
		}

		void SetSelected();

		constexpr bool IsSelected() const {
			return selected;
		}

		void SetValuePtr(void* v) {
			value = v;
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
		void (*callback)() {};
	};

	template<class T>
	struct Option;

	template<>
	struct Option<void> : OptionBase {
		explicit constexpr Option(const std::string& name, void (*callback)())
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
		struct Dummy {
		} dummy;
	};

	template<class T>
		requires(std::is_arithmetic_v<T>)
	struct Option<T> : OptionBase {
		explicit constexpr Option(T& f, const std::string& name, void (*callback)())
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

			by2 = input->InputHeld(Keybind::MENU_NUM_MULT2);

			if (input->InputDown(Keybind::MENU_NUM_NEXT_TENS)) {
				tens++;

				if (tens > std::numeric_limits<T>::digits10)
					tens = 0;
			}

			if(input->InputDown(Keybind::MENU_NUM_INC)) {
				if(by2)
					val *= 2;
				else
					val += std::pow(10, tens);

				changed = true;
			} else if(input->InputDown(Keybind::MENU_NUM_DEC)) {
				if(by2)
					val /= 2;
				else
					val -= std::pow(10, tens);

				changed = true;
			} else if(input->InputDown(Keybind::MENU_NUM_NEGATE)) {
				val = -val;
				changed = true;
			} else if(input->InputDown(Keybind::MENU_NUM_SET0)) {
				val = 0;
				changed = true;
			}

			return changed;
		}

		std::string String() const override {
			std::string extra;

			if(selected) {
				if(by2) {
					extra = "\x81\x7E/\x81\x80" "2"; // x/div 2
				} else {
					// 0x7D is }, so it gets mad when trying to emit it on its own
					extra = fmt::format("{}{}", "\x81\x7D", std::pow(10, tens)); // +- 10^tens
				}
			}

			return fmt::format("{}: {}{}", name, OptionBase::ValueAs<T>(), extra);
		}

	   private:
		int tens {};
		bool by2 {};
	};

	template<class T>
		requires(std::is_enum_v<T>)
	struct Option<T> : OptionBase {
		explicit constexpr Option(T& f, const std::string& name, void (*callback)())
			: OptionBase(f, name, callback) {
		}

		explicit constexpr Option(T& f, const std::string& name)
			: OptionBase(f, name) {
		}

		bool Update(HidInput* input) override {
			if(!OptionBase::Update(input))
				return false;

			bool changed = false;

			std::underlying_type_t<T>& val = reinterpret_cast<std::underlying_type_t<T>&>(ValueAs<T>());

			if(input->InputDown(Keybind::MENU_NUM_INC)) {
				val++;
				changed = true;
			} else if(input->InputDown(Keybind::MENU_NUM_DEC)) {
				val--;
				changed = true;
			}

			return changed;
		}

		std::string String() const override {
			std::string extra;

			if (selected)
				extra = "\x81\x7D""1";

			return fmt::format("{}: {}{}", name, OptionBase::ValueAs<T>(), extra);
		}
	};

	template<>
	struct Option<bool> : OptionBase {
		explicit constexpr Option(bool& f, const std::string& name, void (*callback)())
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

			if(input->InputDown(Keybind::MENU_NUM_INC) || input->InputDown(Keybind::MENU_NUM_DEC) || input->InputDown(Keybind::MENU_NUM_NEGATE)) {
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