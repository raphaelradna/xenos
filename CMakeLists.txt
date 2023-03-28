cmake_minimum_required(VERSION 3.15)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(APP_NAME Xenos)

project(${APP_NAME}_APP VERSION 0.1)

# Some checks to see what platform we are on
if (WIN32)
    set(WINDOWS TRUE)
endif(WIN32)

if(APPLE)
    set(APPLE TRUE)
endif(APPLE)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif(UNIX AND NOT APPLE)

# Set what formats we want to target
list(APPEND BUILD_TYPES VST3)
if (APPLE)
    list(APPEND BUILD_TYPES AU)
endif(APPLE)
if(LINUX)
    list(APPEND BUILD_TYPES LV2)
endif(LINUX)

add_subdirectory(JUCE)       

set_directory_properties(PROPERTIES
JUCE_COMPANY_COPYRIGHT "GPLv3"
JUCE_COMPANY_NAME "Raphael Radna"
JUCE_COMPANY_WEBSITE "www.raphaelradna.com")

juce_add_plugin(${APP_NAME}
    # VERSION ...                               # Set this if the plugin version is different to the project version
    # ICON_BIG ...                              # ICON_* arguments specify a path to an image file to use as an icon for the Standalone
    # ICON_SMALL ...
    # EDITOR_WANTS_KEYBOARD_FOCUS TRUE/FALSE    # Does the editor need keyboard focus?
    COMPANY_NAME "Raphael Radna"                          
    IS_SYNTH True                               # Is this a synth or an effect?
    NEEDS_MIDI_INPUT True                       # Does the plugin need midi input?
    # NEEDS_MIDI_OUTPUT TRUE/FALSE              # Does the plugin need midi output?
    IS_MIDI_EFFECT FALSE                        # Is this plugin a MIDI effect?
    COPY_PLUGIN_AFTER_BUILD TRUE     
    PLUGIN_MANUFACTURER_CODE RRAD         
    PLUGIN_CODE XNOS
    FORMATS ${BUILD_TYPES}        
    PRODUCT_NAME "Xenos"
    LV2URI "https://github.com/raphaelradna/xenos"
)

target_sources(${APP_NAME} PRIVATE
    Source/PluginEditor.cpp
    Source/PluginProcessor.cpp
    Source/Quantizer.cpp
    Source/RandomSource.cpp
    Source/RandomWalk.cpp
    Source/Scale.cpp
    Source/Utility.cpp
)

juce_add_binary_data(GuiAppData
SOURCES
Resources/background.png
)

target_link_libraries(${APP_NAME} PRIVATE
    GuiAppData
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_audio_utils
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

juce_generate_juce_header(${APP_NAME})

target_compile_definitions(${APP_NAME} PUBLIC
# JUCE_WEB_BROWSER and JUCE_USE_CURL would be on by default, but you might not need them.
JUCE_WEB_BROWSER=0                          # If you remove this, add `NEEDS_WEB_BROWSER TRUE` to the `juce_add_gui_app` call
JUCE_USE_CURL=0                             # If you remove this, add `NEEDS_CURL TRUE` to the `juce_add_gui_app` call
JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:${APP_NAME},JUCE_PROJECT_NAME>"
JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:${APP_NAME},JUCE_VERSION>"
JUCE_DISPLAY_SPLASH_SCREEN=0
JUCE_STRICT_REFCOUNTEDPOINTER=1
JUCE_VST3_CAN_REPLACE_VST2=0                # If true requires vst2 SDK which I don't have so it has to be false
JUCE_MODAL_LOOPS_PERMITTED=1
DONT_SET_USING_JUCE_NAMESPACE=1
JUCE_DONT_DECLARE_PROJECTINFO=1
JUCE_TARGET_HAS_BINARY_DATA=1
)

set_target_properties(${APP_NAME} PROPERTIES
BUILD_WITH_INSTALL_RPATH 1
INSTALL_NAME_DIR "@rpath"
CXX_STANDARD 11
CXX_STANDARD_REQUIRED ON
)