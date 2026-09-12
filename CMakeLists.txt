cmake_minimum_required(VERSION 3.22)

project(OrchGate VERSION 0.1.0)

# Build-timestamp label (see cmake/GenerateOrchGateBuildInfo.cmake): a plain
# custom target with no OUTPUT/DEPENDS tracking, so it reruns and rewrites
# the header on every single build, not just every configure or only when a
# source file happens to change.
set(OrchGateGeneratedDir "${CMAKE_BINARY_DIR}/generated")
file(MAKE_DIRECTORY "${OrchGateGeneratedDir}")

add_custom_target(OrchGateGenerateBuildInfo ALL
    COMMAND ${CMAKE_COMMAND}
        "-DOUTPUT_FILE=${OrchGateGeneratedDir}/OrchGateBuildInfo.h"
        -P "${CMAKE_CURRENT_SOURCE_DIR}/cmake/GenerateOrchGateBuildInfo.cmake"
    BYPRODUCTS "${OrchGateGeneratedDir}/OrchGateBuildInfo.h"
    COMMENT "Stamping OrchGate build timestamp"
)

# Path to your JUCE folder.
add_subdirectory("C:/JUCE/JUCE" JUCE)

juce_add_plugin(OrchGate
    COMPANY_NAME "JohnPascu"
    PLUGIN_MANUFACTURER_CODE Jpas
    PLUGIN_CODE Ogat

    FORMATS VST3 Standalone

    PRODUCT_NAME "OrchGate"
    PLUGIN_NAME "OrchGate"
    BUNDLE_ID "com.johnpascu.OrchGate"

    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    VST3_REPLACES_VST2 FALSE

    COPY_PLUGIN_AFTER_BUILD FALSE
)

juce_generate_juce_header(OrchGate)

target_sources(OrchGate
    PRIVATE
        Source/OrchGateProcessor.cpp
        Source/OrchGateEditor.cpp
)

add_dependencies(OrchGate OrchGateGenerateBuildInfo)
target_include_directories(OrchGate PRIVATE ${OrchGateGeneratedDir})

target_compile_definitions(OrchGate
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
)

target_link_libraries(OrchGate
    PRIVATE
        juce::juce_audio_processors
        juce::juce_audio_utils
        juce::juce_gui_extra

    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

# JUCE 9 VST3 wrapper warning control.
if (TARGET OrchGate_VST3)
    target_compile_definitions(OrchGate_VST3
        PRIVATE
            JUCE_VST3_CAN_REPLACE_VST2=0
    )
endif()
