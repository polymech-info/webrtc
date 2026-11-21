# Portable WebRTC VAD — C only, no Abseil, no full APM.
# Builds on desktop and Android. Full webrtc_apm stays Win32-only.

if(TARGET webrtc_vad)
  return()
endif()

enable_language(C)

set(_WEBRTC_ROOT "${CMAKE_CURRENT_LIST_DIR}")

set(_WEBRTC_VAD_SOURCES
  common_audio/vad/webrtc_vad.c
  common_audio/vad/vad_core.c
  common_audio/vad/vad_filterbank.c
  common_audio/vad/vad_gmm.c
  common_audio/vad/vad_sp.c
  common_audio/signal_processing/division_operations.c
  common_audio/signal_processing/energy.c
  common_audio/signal_processing/get_scaling_square.c
  common_audio/signal_processing/resample.c
  common_audio/signal_processing/resample_48khz.c
  common_audio/signal_processing/resample_by_2.c
  common_audio/signal_processing/resample_by_2_internal.c
  common_audio/signal_processing/resample_fractional.c
  common_audio/signal_processing/spl_init.c
  common_audio/signal_processing/spl_inl.c
  common_audio/signal_processing/min_max_operations.c
  common_audio/signal_processing/vector_scaling_operations.c
  common_audio/signal_processing/cross_correlation.c
  common_audio/signal_processing/downsample_fast.c
  common_audio/signal_processing/copy_set_operations.c
  common_audio/signal_processing/spl_sqrt.c
  pm_vad_shims/rtc_fatal_message.c
)

set(_WEBRTC_VAD_EXISTING)
foreach(_src IN LISTS _WEBRTC_VAD_SOURCES)
  if(EXISTS "${_WEBRTC_ROOT}/${_src}")
    list(APPEND _WEBRTC_VAD_EXISTING "${_WEBRTC_ROOT}/${_src}")
  else()
    message(WARNING "webrtc_vad: missing ${_src}")
  endif()
endforeach()

add_library(webrtc_vad STATIC ${_WEBRTC_VAD_EXISTING})
set_target_properties(webrtc_vad PROPERTIES
  POSITION_INDEPENDENT_CODE ON
  C_STANDARD 99
  C_STANDARD_REQUIRED ON
)
target_include_directories(webrtc_vad PUBLIC "${_WEBRTC_ROOT}")
target_compile_definitions(webrtc_vad PUBLIC HAVE_WEBRTC_VAD=1)

if(WIN32)
  target_compile_definitions(webrtc_vad PUBLIC WEBRTC_WIN=1)
  target_compile_definitions(webrtc_vad PRIVATE WIN32_LEAN_AND_MEAN NOMINMAX)
elseif(ANDROID)
  target_compile_definitions(webrtc_vad PUBLIC WEBRTC_ANDROID=1 WEBRTC_POSIX=1)
  target_compile_definitions(webrtc_vad PRIVATE WEBRTC_LINUX=1)
  # arm64/armv7: prefer portable C paths over MIPS/ARMv7 intrinsics
  target_compile_definitions(webrtc_vad PRIVATE WEBRTC_HAS_NEON=0)
else()
  target_compile_definitions(webrtc_vad PUBLIC WEBRTC_POSIX=1)
  if(APPLE)
    target_compile_definitions(webrtc_vad PUBLIC WEBRTC_MAC=1)
  else()
    target_compile_definitions(webrtc_vad PUBLIC WEBRTC_LINUX=1)
  endif()
endif()

if(MSVC)
  target_compile_options(webrtc_vad PRIVATE /W3 /wd4244 /wd4267)
else()
  target_compile_options(webrtc_vad PRIVATE -Wno-unused-parameter)
endif()

message(STATUS "WebRTC VAD: portable target webrtc_vad")
