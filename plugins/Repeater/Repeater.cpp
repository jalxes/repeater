/*
 * DISTRHO Plugin Framework (DPF)
 * Copyright (C) 2012-2015 Filipe Coelho <falktx@falktx.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any purpose with
 * or without fee is hereby granted, provided that the above copyright notice and this
 * permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD
 * TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS. IN
 * NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
 * DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
 * IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "DistrhoPlugin.hpp"

START_NAMESPACE_DISTRHO

// -----------------------------------------------------------------------------------------------------------

/**
  Simple plugin to demonstrate state usage (including UI).
  The plugin will be treated as an effect, but it will not change the host audio.
 */
class Repeater : public Plugin
{
public:
    Repeater()
        : Plugin(3, 2, 2) // 3 parameters, 2 programs, 2 states
    {
        fParams.numberLastEvents = 16.0f;
        fParams.eventGroup = 0.0f;
        fParams.repeat = false;
    }
   
    enum Parameters
    {
        paramNumberLastEvents,
        paramEventGroup,
        paramRepeat,
    };



protected:
   /* --------------------------------------------------------------------------------------------------------
    * Information */

   /**
      Get the plugin label.
      A plugin label follows the same rules as Parameter::symbol, with the exception that it can start with numbers.
    */
    const char* getLabel() const override
    {
        return "Repeater";
    }

   /**
      Get an extensive comment/description about the plugin.
    */
    const char* getDescription() const override
    {
        return "repeater man.";
    }

   /**
      Get the plugin author/maker.
    */
    const char* getMaker() const override
    {
        return "jalxes";
    }

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
    const char* getLicense() const override
    {
        return "APACHE";
    }

   /**
      Get the plugin version, in hexadecimal.
    */
    uint32_t getVersion() const override
    {
        return d_version(1, 0, 0);
    }

   /**
      Get the plugin unique Id.
      This value is used by LADSPA, DSSI and VST plugin formats.
    */
    int64_t getUniqueId() const override
    {
        return d_cconst('d', 'S', 't', 's');
    }

   /* --------------------------------------------------------------------------------------------------------
    * Init */

   /**
      This plugin has no parameters..
    */
    void initParameter(uint32_t index, Parameter& parameter) override{
       /**
          Set the (unique) parameter name.
        */
        switch (index)
        {
        case paramNumberLastEvents:
            parameter.name = "number last events";
            parameter.symbol     = "numberLastEvents";
            parameter.hints      = kParameterIsAutomable|kParameterIsInteger;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 64.0f;
            break;
        case paramEventGroup:
            parameter.name = "event group";
            parameter.symbol     = "eventGroup";
            parameter.hints      = kParameterIsAutomable|kParameterIsInteger;
            parameter.ranges.def = 0.0f;
            parameter.ranges.min = 0.0f;
            parameter.ranges.max = 16.0f;
            break;
        case paramRepeat:
            parameter.name = "repeat";
            parameter.symbol     = "repeat";
            parameter.hints = kParameterIsAutomable|kParameterIsBoolean;
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
        switch (index)
        {
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
    void initState(uint32_t index, String& stateKey, String& defaultStateValue) override
    {
        switch (index)
        {
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
        switch (index)
            {
            case paramNumberLastEvents:
                return fParams.numberLastEvents;
            case paramEventGroup:
                return fParams.eventGroup;
            case paramRepeat:
                return fParams.repeat ? 1.0f : 0.0f;
            }
        return 0.0f;
    }

    void setParameterValue(uint32_t index, float value) override
    {
        switch (index)
            {
            case paramNumberLastEvents:
                fParams.numberLastEvents = value;
                break;
            case paramEventGroup:
                fParams.eventGroup = value;
                break;
            case paramRepeat: {
                const bool repeat = (value > 0.5f);
                if (fParams.repeat != repeat)
                {
                    fParams.repeat = repeat;
                }
            }   break;
        }
    }
   /**
      Load a program.
      The host may call this function from any context, including realtime processing.
    */
    void loadProgram(uint32_t index) override
    {
        switch (index)
        {
        case 0:
            fParams.numberLastEvents = 16.0f;
            fParams.eventGroup = 0.0f;
            fParams.repeat = false;
            
            break;
        case 1:
            fParams.numberLastEvents = 16.0f;
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
        static const String sTrue ("true");
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


void run(const float**, float**, uint32_t,
             const MidiEvent* midiEvents, uint32_t midiEventCount) override
    {
        for (uint32_t i=0; i<midiEventCount; ++i)
            writeMidiEvent(midiEvents[i]);
    }

    // -------------------------------------------------------------------------------------------------------

private:
   /**
      Our parameters used to display the grid on/off states.
    */
    struct ParamValues {
        float numberLastEvents;
        float eventGroup;
        bool repeat;
    } fParams;


   /**
      Set our plugin class as non-copyable and add a leak detector just in case.
    */
    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Repeater)
};

/* ------------------------------------------------------------------------------------------------------------
 * Plugin entry point, called by DPF to create a new plugin instance. */

Plugin* createPlugin()
{
    return new Repeater();
}

// -----------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
