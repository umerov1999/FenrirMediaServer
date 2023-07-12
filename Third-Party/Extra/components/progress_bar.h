// Copyright 2018 bmc::labs GmbH.
// Author: Florian Eich <florian@bmc-labs.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef PROGRESS_INCLUDE_PROGRESS_BAR_H_
#define PROGRESS_INCLUDE_PROGRESS_BAR_H_

#include <chrono>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <iomanip>
#include <ostream>
#include <sstream>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#define WINAPI_CLEAN

using namespace std::chrono_literals;

namespace bmc {
	constexpr const std::size_t MIN_LINE_LENGTH{50};
	constexpr const std::size_t TIME_OUTPUT_LENGTH{20};
	constexpr const std::size_t MISC_SYMBOLS_LENGTH{7};

	class progress_bar
	{
		friend std::ostream& operator<<(std::ostream& os, progress_bar& pb);

	public:
		progress_bar(std::uint64_t min,
			std::uint64_t max,
			std::size_t width,
			bool        auto_inc,
			bool        show_time)
			: min_(min),
			max_(max),
			current_(min),
			width_(width),
			auto_inc_(auto_inc),
			show_time_(show_time),
			timestamp_(std::chrono::high_resolution_clock::now()) {
			if (max_ <= min_) {
				throw std::range_error("max must be greater than min");
			}
			if (!width_) {
				throw std::invalid_argument("width cannot be 0% of terminal");
			}

#if defined(_WIN32) && defined(WINAPI_CLEAN)
			lastPos = GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE));
#endif
		}

		std::uint64_t size() const noexcept { return max_ - min_; }

		progress_bar& operator++() noexcept {
			if (current_ < max_) { ++current_; }
			return *this;
		}

		progress_bar operator++(int) noexcept {
			if (current_ < max_) {
				auto tmp{ *this };
				++current_;
				return tmp;
			}
			return *this;
		}

		progress_bar& operator--() noexcept {
			if (current_ > min_) { --current_; }
			return *this;
		}

		progress_bar operator--(int) noexcept {
			if (current_ > min_) {
				auto tmp{ *this };
				--current_;
				return tmp;
			}
			return *this;
		}

		void step(std::size_t steps = 1) noexcept {
			current_ = clamp(current_ + steps);
		}

		void set(std::uint64_t value) noexcept { current_ = clamp(value); }

		std::string disp() noexcept {
			if (auto_inc_) { step(); }

			auto line_length{ termwidth_() * width_ / 100 };
			if (line_length < MIN_LINE_LENGTH) {
				show_time_ = false;
			}
			else {
				line_length -= TIME_OUTPUT_LENGTH;
			}
			auto bar_length{ line_length - MISC_SYMBOLS_LENGTH };

			auto progress{
			  current_ == max_ ? 100.0 : (current_ - min_) / ((max_ - min_) / 100.0) };

			auto position{ progress == 100.0 ? bar_length
											: static_cast<std::size_t>(std::round(
												progress * (bar_length / 100.0))) };

			std::stringstream ss;
			ss << '[';
			for (std::size_t i{0}; i <= bar_length; ++i) {
				if (i < position) {
					ss << '-';
				}
				else if (i == position) {
					ss << '>';
				}
				else {
					ss << ' ';
				}
			}
			ss << "] " << std::setw(3) << static_cast<std::size_t>(progress) << '%';

			if (show_time_) {
				std::chrono::duration<double> elapsed =
					std::chrono::high_resolution_clock::now() - timestamp_;

				auto minutes = static_cast<std::size_t>(elapsed.count()) / 60;

				ss << " - " << std::setw(2) << minutes << ":" << std::right
					<< std::setfill('0') << std::fixed << std::setprecision(2)
					<< std::setw(5) << elapsed.count() << " elapsed";
			}

			return ss.str();
		}

	private:
		std::uint64_t clamp(std::uint64_t input) const noexcept {
			if (input < min_) { return min_; }
			if (input > max_) { return max_; }
			return input;
		}

		std::size_t termwidth_() const noexcept {
#ifdef _WIN32
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
			return info.srWindow.Right - info.srWindow.Left + 1;
#else
			struct winsize w;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
			return w.ws_col;
#endif
		}

#if defined(_WIN32) && defined(WINAPI_CLEAN)
		COORD getLastPos() {
			return lastPos;
		}
#endif

	private:
		const std::uint64_t                                           min_;
		const std::uint64_t                                           max_;
		std::uint64_t                                                 current_;
		const std::size_t                                           width_;
		bool                                                        auto_inc_;
		bool                                                        show_time_;
		std::chrono::time_point<std::chrono::high_resolution_clock> timestamp_;
#if defined(_WIN32) && defined(WINAPI_CLEAN)
		COORD lastPos;

		static COORD GetConsoleCursorPosition(HANDLE hConsoleOutput) {
			CONSOLE_SCREEN_BUFFER_INFO cbsi;
			if (GetConsoleScreenBufferInfo(hConsoleOutput, &cbsi)) {
				return cbsi.dwCursorPosition;
			}
			else {
				COORD invalid = { 0, 0 };
				return invalid;
			}
		}
#endif
	};

	inline std::ostream& operator<<(std::ostream& os, bmc::progress_bar& pb) {
#if defined(_WIN32) && defined(WINAPI_CLEAN)
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		COORD tmpPos = progress_bar::GetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE));
		DWORD cCharsWritten = 0;
		DWORD dwConSize = (DWORD)pb.termwidth_() * (tmpPos.Y - pb.getLastPos().Y);
		FillConsoleOutputCharacterW(hConsole, (TCHAR)' ', dwConSize, pb.getLastPos(), &cCharsWritten);
		SetConsoleCursorPosition(hConsole, pb.getLastPos());

		os << pb.disp() << '\n';
#else
		os << "\33[2K\033[A\33[2K\r" << pb.disp() << '\n';
#endif
		return os;
	}

}  // namespace bmc

#endif  // PROGRESS_INCLUDE_PROGRESS_BAR_H_