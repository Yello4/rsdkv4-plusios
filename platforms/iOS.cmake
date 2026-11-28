# iOS (iPhone / iPad) build, meant for the Xcode generator:
#
#   cmake -B build-ios -G Xcode -DCMAKE_SYSTEM_NAME=iOS -DCMAKE_OSX_ARCHITECTURES=arm64 \
#         -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0 -DSDL2_DIR=<sdl2 install>/lib/cmake/SDL2
#
# Expected dependencies (see .github/workflows/build-ios.yml, which fetches them):
#   dependencies/ios/libogg      libogg source
#   dependencies/ios/libvorbis   libvorbis source
#   dependencies/ios/libtheora   libtheora source
#   SDL2 built for iOS as a static library and installed somewhere SDL2_DIR points at
#
# No game data is bundled. The app reads Data.rsdk (and writes settings/saves/log.txt) from its
# Documents folder, which is exposed in the Files app (UIFileSharingEnabled).

set(DEP_PATH ios)

if(NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    set(CMAKE_OSX_DEPLOYMENT_TARGET 13.0)
endif()

# iOS-specific overrides
set(RETRO_SDL_VERSION 2)
# Hardware renderer through OpenGL ES 1.1 (like the Android port). It draws the mobile menus, the pause menu and the
# on-screen touch controls; without it the mobile Data.rsdk has no visible touch controls.
set(RETRO_USE_HW_RENDER ON)
# The iPhone file system is case-sensitive; the Files app makes it easy to end up with data.rsdk / DATA.RSDK etc.
set(RETRO_FORCE_CASE_INSENSITIVE ON)
set(RETRO_USE_STEAM OFF)
set(RETRO_USE_DISCORD OFF)
set(RETRO_UPDATE_CHECKER OFF)
set(RETRO_USE_CURL OFF)

if(NOT DEFINED RETRO_IOS_BUNDLE_ID)
    set(RETRO_IOS_BUNDLE_ID "com.rsdkv4plus.retroengine")
endif()

set(RETRO_IOS_RESOURCES
    ${CMAKE_SOURCE_DIR}/dependencies/ios/Base.lproj/LaunchScreen.storyboard
)

add_executable(RetroEngine MACOSX_BUNDLE ${RETRO_FILES} ${RETRO_IOS_RESOURCES})

# libogg needs a generated config_types.h
set(RETRO_IOS_OGG_CONFIG ${CMAKE_SOURCE_DIR}/dependencies/ios/libogg/include/ogg/config_types.h)
if(NOT EXISTS ${RETRO_IOS_OGG_CONFIG})
    file(COPY ${CMAKE_SOURCE_DIR}/dependencies/android/config_types.h DESTINATION ${CMAKE_SOURCE_DIR}/dependencies/ios/libogg/include/ogg/)
endif()

set(COMPILE_OGG TRUE)
set(COMPILE_VORBIS TRUE)
set(COMPILE_THEORA TRUE)

set(OGG_FLAGS -ffast-math -fsigned-char -O2 -w)
set(VORBIS_FLAGS -ffast-math -fsigned-char -O2 -w)
set(THEORA_FLAGS -ffast-math -fsigned-char -O2 -w)

find_package(SDL2 CONFIG REQUIRED)

if(TARGET SDL2::SDL2main)
    target_link_libraries(RetroEngine SDL2::SDL2main)
endif()

if(TARGET SDL2::SDL2-static)
    target_link_libraries(RetroEngine SDL2::SDL2-static)
else()
    target_link_libraries(RetroEngine SDL2::SDL2)
endif()

# System frameworks SDL2 needs on iOS (listed explicitly in case the SDL2 package doesn't carry them)
target_link_libraries(RetroEngine
    "-framework Foundation"
    "-framework UIKit"
    "-framework CoreGraphics"
    "-framework QuartzCore"
    "-framework OpenGLES"
    "-framework Metal"
    "-framework CoreVideo"
    "-framework CoreAudio"
    "-framework AudioToolbox"
    "-framework AVFoundation"
    "-framework CoreMotion"
    "-framework CoreHaptics"
    "-framework CoreBluetooth"
    "-framework GameController"
    iconv
)

set_target_properties(RetroEngine PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
    MACOSX_BUNDLE TRUE
    MACOSX_BUNDLE_INFO_PLIST ${CMAKE_SOURCE_DIR}/dependencies/ios/Info.plist
    MACOSX_BUNDLE_GUI_IDENTIFIER ${RETRO_IOS_BUNDLE_ID}
    MACOSX_BUNDLE_BUNDLE_VERSION ${DECOMP_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${DECOMP_VERSION}
    RESOURCE "${RETRO_IOS_RESOURCES}"
    XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${RETRO_IOS_BUNDLE_ID}
    XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2"
    XCODE_ATTRIBUTE_IPHONEOS_DEPLOYMENT_TARGET ${CMAKE_OSX_DEPLOYMENT_TARGET}
    XCODE_ATTRIBUTE_ENABLE_BITCODE NO
    XCODE_ATTRIBUTE_CODE_SIGNING_ALLOWED NO
    XCODE_ATTRIBUTE_CODE_SIGNING_REQUIRED NO
    XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ""
)
