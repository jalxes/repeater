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
    : Plugin(7, 0, 0) // parameters, programs, states
  {
    fParams.numberLastBars = 4.0f;
    fParams.eventGroup = 0.0f;
    fParams.repeat = false;
    fParams.clearAll = false;
    fParams.clearLast = false;
    fParams.seila = false;
    fParams.curEventIndex = 0.0f;
  }

  enum Parameters
  {
    paramNumberLastBars,
    paramEventGroup,
    paramRepeat,
    paramClearAll,
    paramClearLast,
    paramSeila,
    paramCurEventIndex,
  };

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
    /**
       Set the (unique) parameter name.
     */
    switch (index) {
      case paramNumberLastBars:
        parameter.name = "number last bars";
        parameter.symbol = "numberLastBars";
        parameter.hints = kParameterIsAutomable | kParameterIsInteger;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 64.0f;
        break;
      case paramEventGroup:
        parameter.name = "event group";
        parameter.symbol = "eventGroup";
        parameter.hints = kParameterIsAutomable | kParameterIsInteger;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 16.0f;
        break;
      case paramRepeat:
        parameter.name = "repeat";
        parameter.symbol = "repeat";
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;
      case paramClearAll:
        parameter.name = "clearAll";
        parameter.symbol = "clearAll";
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;
      case paramClearLast:
        parameter.name = "clearLast";
        parameter.symbol = "clearLast";
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;
      case paramSeila:
        parameter.name = "seila";
        parameter.symbol = "seila";
        parameter.hints = kParameterIsAutomable | kParameterIsBoolean;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 1.0f;
        break;
      case paramCurEventIndex:
        parameter.name = "curEventIndex";
        parameter.symbol = "curEventIndex";
        parameter.hints = kParameterIsInteger;
        parameter.ranges.def = 0.0f;
        parameter.ranges.min = 0.0f;
        parameter.ranges.max = 64.0f;
        break;
    }
  }

  /* --------------------------------------------------------------------------------------------------------
   * Internal data */

  float getParameterValue(uint32_t index) const override
  {
    switch (index) {
      case paramNumberLastBars:
        return fParams.numberLastBars;
      case paramEventGroup:
        return fParams.eventGroup;
      case paramCurEventIndex:
        return fParams.curEventIndex;
      case paramRepeat:
        return fParams.repeat ? 1.0f : 0.0f;
      case paramClearAll:
        return fParams.clearAll ? 1.0f : 0.0f;
      case paramClearLast:
        return fParams.clearLast ? 1.0f : 0.0f;
      case paramSeila:
        return fParams.seila ? 1.0f : 0.0f;
    }
    return 0.0f;
  }

  void setParameterValue(uint32_t index, float value) override
  {
    switch (index) {
      case paramNumberLastBars:
        fParams.numberLastBars = value;
        break;
      case paramEventGroup:
        fParams.eventGroup = value;
        break;
      case paramRepeat: {
        const bool repeat = (value > 0.5f);
        if (fParams.repeat != repeat) {
          fParams.repeat = repeat;
        }
      } break;
      case paramClearAll: {
        const bool clearAll = (value > 0.5f);
        if (fParams.clearAll != clearAll) {
          if (clearAll)
            allEventsByBar.clear();
          fParams.clearAll = clearAll;
        }
      } break;
      case paramClearLast: {
        const bool clearLast = (value > 0.5f);
        if (fParams.clearLast != clearLast) {
          if (clearLast)
            lastEvents.clear();
          fParams.clearLast = clearLast;
        }
      } break;
      case paramSeila: {
        const bool seila = (value > 0.5f);
        if (fParams.seila != seila) {
          fParams.seila = seila;
        }
      } break;
      case paramCurEventIndex: {
        if (value > fParams.numberLastBars)
          value = fParams.numberLastBars - 1;
        fParams.curEventIndex = value;
        curEventIndex = value;
        break;
      }
    }
  }

  /* --------------------------------------------------------------------------------------------------------
   * Process */

  void resetEvents()
  {
    if (allEventsByBar.size() > fParams.numberLastBars)
      lastEvents = std::vector<EventByBar>(
        allEventsByBar.end() - fParams.numberLastBars, allEventsByBar.end());
    else
      lastEvents = allEventsByBar;
  }

  void run(const float**,
           float**,
           uint32_t,
           const MidiEvent* midiEvents,
           uint32_t midiEventCount) override
  {
    const TimePosition& timePos(getTimePosition());
    bool newBar = timePosBar != timePos.bbt.bar;

    if (newBar) {
      timePosBar = timePos.bbt.bar;
      std::cout << std::endl;
      std::cout << "timePosBar " << timePosBar;

      if (!eventByBar.events.empty()) {
        allEventsByBar.emplace_back(eventByBar);
        resetEvents();
      }

      std::vector<EventWithTime> events;
      eventByBar = EventByBar{ events, timePos.bbt.bar };
    }

    for (uint32_t i = 0; i < midiEventCount; ++i) {
      eventByBar.events.emplace_back(
        EventWithTime{ false, midiEvents[i], timePos });

      writeMidiEvent(midiEvents[i]);
    }
    if (midiEventCount > 0)
      return;

    if (!timePos.bbt.valid || !timePos.playing)
      return;

    if (newBar) {
      curEventIndex++;
      if (curEventIndex >= (lastEvents.size() - 1)) {
        std::cout << std::endl;
        std::cout << "OI"
                  << "\n";
        curEventIndex = 0;
      }
      std::cout << std::endl;
      fParams.curEventIndex = curEventIndex;
      std::cout << "curEventIndex " << curEventIndex;
      std::cout << std::endl;
    }

    if (!fParams.repeat || lastEvents.empty())
      return;

    for (auto& event : lastEvents.at(curEventIndex).events) {
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
      for (auto& event : lastEvents.at(curEventIndex).events)
        event.played = false;
    }
  }

  // -------------------------------------------------------------------------------------------------------

private:
  /**
     Our parameters used to display the grid on/off states.
   */
  struct ParamValues
  {
    float numberLastBars;
    float eventGroup;
    bool repeat;
    bool clearAll;
    bool clearLast;
    bool seila;
    float curEventIndex;
  } fParams;
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

  int32_t timePosBar;
  std::vector<EventByBar> lastEvents;
  u_long curEventIndex = 0;
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
