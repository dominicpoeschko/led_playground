#pragma once
#include "HWConfig.hpp"
///

#include <chip/pio/ws2812.hpp>
#include <kvasir/Util/FaultHandler.hpp>
#include <kvasir/Util/StackProtector.hpp>

using Clock          = HW::SystickClock;
using StackProtector = Kvasir::StackProtector<>;
using FaultHandler   = Kvasir::Fault::Handler<HW::Fault_CleanUpAction>;

struct DMAConfig {
    static constexpr auto numberOfChannels     = 1;
    static constexpr auto callbackFunctionSize = 64;
};

using DMA = Kvasir::DMA::DmaBase<DMAConfig>;

struct WS2812Config {
    static constexpr auto ClockSpeed     = HW::ClockSpeed;
    static constexpr auto LedClockSpeed  = 800000;
    static constexpr auto PioInstance    = 0;
    static constexpr auto SmInstance     = 0;
    static constexpr auto ProgrammOffset = 0;
};

using WS2812 = Kvasir::Pio::
  WS2812<Clock, HW::Pin::led, DMA, DMA::Channel::ch0, DMA::Priority::low, WS2812Config>;

using Startup = Kvasir::Startup::Startup<
  HW::ClockSettings,
  Clock,
  HW::ComBackend,
  FaultHandler,
  StackProtector,
  HW::PinConfig,
  DMA,
  WS2812>;
