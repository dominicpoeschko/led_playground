#include "ApplicationConfig.hpp"
//

#include <algorithm>
#include <cmake_git_version/version.hpp>
#include <kvasir/Util/using_literals.hpp>
#include <numeric>
#include <ranges>
#include <ws2812Pio/ws2812.hpp>

struct RGB {
    std::array<std::uint8_t, 3> data{};
    constexpr RGB() = default;
    constexpr RGB(std::uint8_t r, std::uint8_t g, std::uint8_t b) : data{g, r, b} {}

    std::uint8_t& r() { return data[1]; }
    std::uint8_t& g() { return data[0]; }
    std::uint8_t& b() { return data[2]; }
};

template<>
struct remote_fmt::formatter<RGB> {
    template<typename Printer>
    constexpr auto format(RGB const& v, Printer& p) const {
        using namespace sc::literals;
        return remote_fmt::format_to(p, "{}"_sc, v.data);
    }
};

static void limit(std::span<RGB> leds, float limit_amps) {
    auto const calc_sum = [&](std::size_t i) {
        return std::transform_reduce(
          leds.begin(),
          leds.end(),
          std::size_t{},
          std::plus{},
          [&](auto const& led) { return static_cast<std::size_t>(led.data[i]); });
    };
    auto const amps_r = (static_cast<float>(calc_sum(0)) / 256.0f) * 0.02f;
    auto const amps_g = (static_cast<float>(calc_sum(1)) / 256.0f) * 0.02f;
    auto const amps_b = (static_cast<float>(calc_sum(2)) / 256.0f) * 0.02f;

    auto const amps  = amps_r + amps_g + amps_b;
    auto const scale = limit_amps / amps;
    UC_LOG_D("{} {} {} {} {}", amps_r, amps_g, amps_b, amps, scale);

    if(1.0f > scale) {
        for(auto& vv : leds) {
            vv.data[0] = static_cast<std::uint8_t>(vv.data[0] * scale);
            vv.data[1] = static_cast<std::uint8_t>(vv.data[1] * scale);
            vv.data[2] = static_cast<std::uint8_t>(vv.data[2] * scale);
        }
    }
}

static void fill(std::span<RGB> leds, RGB const& v) {
    for(auto& vv : leds) {
        vv = v;
    }
}

static void snake_pattern(std::span<RGB> leds) {
    leds[0].data[0] = 1;
    leds[1].data[0] = 3;
    leds[2].data[0] = 6;
    leds[3].data[0] = 9;
    leds[5].data[0] = 10;
}

static void snake_pattern_run(std::span<RGB> leds) {
    std::rotate(leds.begin(), leds.begin() + 1, leds.end());
}

static void rainbow(std::span<RGB> leds) { fill(leds, RGB{1, 1, 1}); }

static RGB next_color(RGB color) {
    if(color.r() != 0) {
        if(color.g() < 255) {
            color.g() = color.g() + 5;
            return RGB{255, color.g(), 0};
        }
        if(color.r() > 0) {
            color.r() = color.r() - 5;
            return RGB{color.r(), color.g(), 0};
        }
    }
    if(color.g() != 0) {
        if(color.b() < 255) {
            color.b() = color.b() + 5;
            return RGB{color.r(), color.g(), color.b()};
        }
        if(color.b() != 0) {
            return RGB{0, 0, 5};
        }
    }
    if(color.b() != 0) {
        if(color.r() == 0) {
            return RGB{5, 0, 5};
        }
    }
    return RGB{255, 0, 0};
}

int main() {
    UC_LOG_D("{}", CMakeGitVersion::FullVersion);
    auto                            next = Clock::now();
    RGB                             color{255, 0, 0};
    Kvasir::StaticVector<RGB, 1024> leds;

    leds.resize(256);
    rainbow(leds);
    while(true) {
        if(WS2812::ready() && Clock::now() > next) {
            //std::rotate(leds.begin(), leds.end(), leds.begin());
            color = next_color(color);
            fill(leds, color);
            limit(leds, 0.5f);
            WS2812::send(std::span{leds});
            next += 1s;
            UC_LOG_D("xx");
        }
        StackProtector::handler();
        WS2812::handler();
    }
}

KVASIR_START(Startup)
