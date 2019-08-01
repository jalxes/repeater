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
    : Plugin(3, 2, 2) // 3 parameters, 2 programs, 2 states
  {
    fParams.numberLastBars = 4.0f;
    fParams.eventGroup = 0.0f;
    fParams.repeat = false;
  }

  enum Parameters
  {
    paramNumberLastBars,
    paramEventGroup,
    paramRepeat,
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
    }
  }

  /**
     Set the name of the program @a index.
     This function will be called once, shortly after the plugin is created.
   */
  void initProgramName(uint32_t index, String& programName) override
  {
    switch (index) {
      case 0:
        programName = "Default";
        break;
      case 1:
        programName = "Custom";
        break;
    }
  }

  /**
     Set the state key and default value of @a index.
     This function will be called once, shortly after the plugin is created.
   */
  void initState(uint32_t index,
                 String& stateKey,
                 String& defaultStateValue) override
  {
    switch (index) {
      case 0:
        stateKey = "top-left";
        break;
      case 1:
        stateKey = "top-center";
        break;
    }

    defaultStateValue = "false";
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
      case paramRepeat:
        return fParams.repeat ? 1.0f : 0.0f;
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
    }
  }
  /**
     Load a program.
     The host may call this function from any context, including realtime
     processing.
   */
  void loadProgram(uint32_t index) override
  {
    switch (index) {
      case 0:
        fParams.numberLastBars = 4.0f;
        fParams.eventGroup = 0.0f;
        fParams.repeat = false;

        break;
      case 1:
        fParams.numberLastBars = 2.0f;
        fParams.eventGroup = 0.0f;
        fParams.repeat = false;

        break;
    }
  }

  /**
     Get the value of an internal state.
     The host may call this function from any non-realtime context.
   */
  String getState(const char* key) const override
  {
    static const String sTrue("true");
    static const String sFalse("false");

    // check which block changed
    // /**/ if (std::strcmp(key, "top-left") == 0)
    //     return fParamGrid[0] ? sTrue : sFalse;
    // else if (std::strcmp(key, "top-center") == 0)
    //     return fParamGrid[1] ? sTrue : sFalse;

    return sFalse;
  }

  /**
     Change an internal state.
   */
  void setState(const char* key, const char* value) override
  {
    const bool valueOnOff = (std::strcmp(value, "true") == 0);

    // // check which block changed
    // /**/ if (std::strcmp(key, "top-left") == 0)
    //     fParamGrid[0] = valueOnOff;
    // else if (std::strcmp(key, "top-center") == 0)
    //     fParamGrid[1] = valueOnOff;
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
    curEventIndex = 0;
  }

  void run(const float**,
           float**,
           uint32_t,
           const MidiEvent* midiEvents,
           uint32_t midiEventCount) override
  {
    const TimePosition& timePos(getTimePosition());
    if (timePosBar != timePos.bbt.bar) {
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

    if (!fParams.repeat || lastEvents.empty() || !timePos.bbt.valid ||
        !timePos.playing)
      return;

    if (timePosBar != timePos.bbt.bar) {
      curEventIndex++;
      std::cout << curEventIndex << ">=" << lastEvents.size() << "\n";
      if (curEventIndex > (lastEvents.size() - 1)) {
        curEventIndex = 0;
        for (auto& event : lastEvents.at(curEventIndex).events)
          event.played = false;
      }
    }

    for (auto& event : lastEvents.at(curEventIndex).events) {
      if (event.time.bbt.beat != timePos.bbt.beat)
        continue;

      int32_t divisor = (timePos.bbt.ticksPerBeat / 10);
      if ((event.time.bbt.tick / divisor) != (timePos.bbt.tick / divisor))
        continue;
      if (event.played)
        continue;

      event.played = true;
      writeMidiEvent(event.event);
    }

    if (timePosBar != timePos.bbt.bar)
      timePosBar = timePos.bbt.bar;
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
