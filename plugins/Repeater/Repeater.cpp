/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"
#include <iostream>
#include <vector>

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

/**
  Simple plugin to demonstrate state usage (including UI).
  The plugin will be treated as an effect, but it will not change the host
  audio.
 */
class Repeater : public Plugin
{
public:
  Repeater()
    : Plugin(kParamsCount, 0, 0) // parameters, programs, states
  {
    std::memset(fParams, 0, sizeof(float) * kParamsCount);

    fParams[kNumberLastBars] = 4.0f;
  }

protected:
  /* --------------------------------------------------------------------------------------------------------
   * Information */

  /**
     Get the plugin label.
     A plugin label follows the same rules as Parameter::symbol, with the
     exception that it can start with numbers.
   */
  const char* getLabel() const override { return "Repeater"; }

  /**
     Get an extensive comment/description about the plugin.
   */
  const char* getDescription() const override { return "repeater man."; }

  /**
     Get the plugin author/maker.
   */
  const char* getMaker() const override { return "jalxes"; }

  /**
     Get the plugin homepage.
   */
  const char* getHomePage() const override
  {
    return "https://github.com/jalxes/repeater";
  }

  /**
     Get the plugin license name (a single line of text).
     For commercial plugins this should return some short copyright information.
   */
  const char* getLicense() const override { return "APACHE"; }

  /**
     Get the plugin version, in hexadecimal.
   */
  uint32_t getVersion() const override { return d_version(1, 0, 0); }

  /**
     Get the plugin unique Id.
     This value is used by LADSPA, DSSI and VST plugin formats.
   */
  int64_t getUniqueId() const override { return d_cconst('d', 'S', 't', 's'); }

  /* --------------------------------------------------------------------------------------------------------
   * Init */

  /**
     This plugin has no parameters..
   */
  void initParameter(uint32_t index, Parameter& parameter) override
  {
    parameter.hints = kParameterIsAutomable;
    parameter.ranges.def = 0.0f;
    parameter.ranges.min = 0.0f;
    parameter.ranges.max = 64.0f;

    /**
       Set the (unique) parameter name.
     */
    switch (index) {
      case kNumberLastBars:
        parameter.name = "numberLastBars";
        parameter.symbol = "numberLastBars";
        parameter.hints |= kParameterIsInteger;
        parameter.ranges.max = 64.0f;
        break;
      case kInitBar:
        parameter.name = "initBar";
        parameter.symbol = "initBar";
        parameter.hints |= kParameterIsInteger;
        break;
      case kEventGroup:
        parameter.name = "eventGroup";
        parameter.symbol = "eventGroup";
        parameter.hints |= kParameterIsInteger;
        parameter.ranges.max = 16.0f;
        break;
      case kRepeat:
        parameter.name = "repeat";
        parameter.symbol = "repeat";
        parameter.hints |= kParameterIsBoolean;
        parameter.ranges.max = 1.0f;
        break;
      case kClearAll:
        parameter.name = "clearAll";
        parameter.symbol = "clearAll";
        parameter.hints |= kParameterIsBoolean;
        parameter.ranges.max = 1.0f;
        break;
      case kClearLast:
        parameter.name = "clearLast";
        parameter.symbol = "clearLast";
        parameter.hints |= kParameterIsBoolean;
        parameter.ranges.max = 1.0f;
        break;
      case kCurEventIndex:
        parameter.name = "curEventIndex";
        parameter.symbol = "curEventIndex";
        parameter.hints |= kParameterIsInteger | kParameterIsOutput;
        parameter.ranges.max = 64.0f;
        break;
      case kTimePosBar:
        parameter.name = "timePosBar";
        parameter.symbol = "timePosBar";
        parameter.hints |= kParameterIsInteger | kParameterIsOutput;
        break;
    }
  }

  /* --------------------------------------------------------------------------------------------------------
   * Internal data */

  float getParameterValue(uint32_t index) const override
  {
    if (index > kParamsCount)
      return 0.0f;

    return fParams[index];
  }

  void setParameterValue(uint32_t index, float value) override
  {
    if (index > kParamsCount)
      return;

    switch (index) {
      case kCurEventIndex:
      case kTimePosBar:
        return;
      case kRepeat:
      case kClearAll:
      case kClearLast:
        if (value > 0.5f)
          value = 1.0f;
        else
          value = 0.0f;
        break;
    }

    fParams[index] = value;

    if (index == kClearAll and fParams[index] > 0.5f)
      allEventsByBar.clear();
    if (index == kClearLast and fParams[index] > 0.5f)
      lastEvents.clear();

    resetEvents();
  }

  /* --------------------------------------------------------------------------------------------------------
   * Process */

  void resetEvents()
  {
    if (allEventsByBar.size() < fParams[kNumberLastBars]) {
      lastEvents = allEventsByBar;
      return;
    }

    float init = fParams[kInitBar];
    if (init < 1)
      init = allEventsByBar.size() - fParams[kNumberLastBars] - 1;

    float end = init + fParams[kNumberLastBars];

    lastEvents = std::vector<EventByBar>(allEventsByBar.begin() + init,
                                         allEventsByBar.begin() + end);
  }

  void run(const float**,
           float**,
           uint32_t,
           const MidiEvent* midiEvents,
           uint32_t midiEventCount) override
  {
    const TimePosition& timePos(getTimePosition());
    bool newBar = fParams[kTimePosBar] != timePos.bbt.bar;

    if (newBar) {
      fParams[kTimePosBar] = timePos.bbt.bar;

      allEventsByBar.emplace_back(eventByBar);

      std::vector<EventWithTime> events;
      eventByBar = EventByBar{ events, timePos.bbt.bar };
    }

    for (uint32_t i = 0; i < midiEventCount; ++i) {
      eventByBar.events.emplace_back(
        EventWithTime{ false, midiEvents[i], timePos });

      writeMidiEvent(midiEvents[i]);
    }
    if (midiEventCount > 0) {
      resetEvents();
      return;
    }

    if (!timePos.bbt.valid || !timePos.playing)
      return;

    if (newBar) {
      fParams[kCurEventIndex]++;
      if (fParams[kCurEventIndex] >= (lastEvents.size()))
        fParams[kCurEventIndex] = 0;
    }

    if (!fParams[kRepeat] || lastEvents.empty())
      return;

    for (auto& event : lastEvents.at(fParams[kCurEventIndex]).events) {
      if (event.time.bbt.beat != timePos.bbt.beat)
        continue;

      int32_t divisor =
        (timePos.bbt.ticksPerBeat / (5 * timePos.bbt.beatsPerBar));
      if ((event.time.bbt.tick / divisor) != (timePos.bbt.tick / divisor))
        continue;
      if (event.played)
        continue;

      event.played = true;
      writeMidiEvent(event.event);
    }

    if (newBar) {
      for (auto& event : lastEvents.at(fParams[kCurEventIndex]).events)
        event.played = false;
    }
  }

  // -------------------------------------------------------------------------------------------------------

private:
 float fParams[kParamsCount];

  struct EventWithTime
  {
    bool played;
    MidiEvent event;
    TimePosition time;
  };
  struct EventByBar
  {
    std::vector<EventWithTime> events;
    int32_t bar;
  } eventByBar;

  std::vector<EventByBar> lastEvents;
  std::vector<EventByBar> allEventsByBar;

  /**
     Set our plugin class as non-copyable and add a leak detector just in
     case.
   */
  DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Repeater)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin*
createPlugin()
{
  return new Repeater();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
