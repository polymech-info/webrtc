cmake_minimum_required(VERSION 3.20)

project(webrtc_apm LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(WEBRTC_APM_BUILD_PROBE "Build the standalone WebRTC APM probe executable" ON)

include(FetchContent)

set(ABSL_PROPAGATE_CXX_STD ON)
FetchContent_Declare(
    abseil-cpp
    GIT_REPOSITORY https://github.com/abseil/abseil-cpp.git
    GIT_TAG master
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(abseil-cpp)

set(WEBRTC_APM_SOURCES
    api/audio/audio_processing.cc
    api/audio/audio_processing_statistics.cc
    api/audio/builtin_audio_processing_builder.cc
    api/audio/echo_canceller3_config.cc
    api/environment/deprecated_global_field_trials.cc
    api/environment/environment_factory.cc
    api/field_trials.cc
    api/field_trials_registry.cc
    api/rtc_event_log/rtc_event_log.cc
    api/task_queue/default_task_queue_factory_win.cc
    api/task_queue/pending_task_safety_flag.cc
    api/task_queue/task_queue_base.cc
    rtc_base/experiments/field_trial_parser.cc
    rtc_base/experiments/field_trial_units.cc
    common_audio/audio_converter.cc
    common_audio/audio_ring_buffer.cc
    common_audio/audio_util.cc
    common_audio/blocker.cc
    common_audio/channel_buffer.cc
    common_audio/fir_filter.cc
    common_audio/fir_filter_c.cc
    common_audio/lapped_transform.cc
    common_audio/real_fourier.cc
    common_audio/real_fourier_ooura.cc
    common_audio/resampler/push_resampler.cc
    common_audio/resampler/push_sinc_resampler.cc
    common_audio/resampler/resampler.cc
    common_audio/resampler/sinc_resampler.cc
    common_audio/resampler/sinc_resampler_avx2.cc
    common_audio/resampler/sinc_resampler_sse.cc
    common_audio/resampler/sinusoidal_linear_chirp_source.cc
    common_audio/ring_buffer.c
    common_audio/signal_processing/auto_corr_to_refl_coef.c
    common_audio/signal_processing/auto_correlation.c
    common_audio/signal_processing/complex_bit_reverse.c
    common_audio/signal_processing/complex_fft.c
    common_audio/signal_processing/copy_set_operations.c
    common_audio/signal_processing/cross_correlation.c
    common_audio/signal_processing/division_operations.c
    common_audio/signal_processing/dot_product_with_scale.cc
    common_audio/signal_processing/downsample_fast.c
    common_audio/signal_processing/energy.c
    common_audio/signal_processing/filter_ar.c
    common_audio/signal_processing/filter_ar_fast_q12.c
    common_audio/signal_processing/filter_ma_fast_q12.c
    common_audio/signal_processing/get_hanning_window.c
    common_audio/signal_processing/get_scaling_square.c
    common_audio/signal_processing/ilbc_specific_functions.c
    common_audio/signal_processing/levinson_durbin.c
    common_audio/signal_processing/lpc_to_refl_coef.c
    common_audio/signal_processing/min_max_operations.c
    common_audio/signal_processing/randomization_functions.c
    common_audio/signal_processing/real_fft.c
    common_audio/signal_processing/refl_coef_to_lpc.c
    common_audio/signal_processing/resample.c
    common_audio/signal_processing/resample_48khz.c
    common_audio/signal_processing/resample_by_2.c
    common_audio/signal_processing/resample_by_2_internal.c
    common_audio/signal_processing/resample_fractional.c
    common_audio/signal_processing/spl_init.c
    common_audio/signal_processing/spl_inl.c
    common_audio/signal_processing/spl_sqrt.c
    common_audio/signal_processing/splitting_filter.c
    common_audio/signal_processing/sqrt_of_one_minus_x_squared.c
    common_audio/signal_processing/vector_scaling_operations.c
    common_audio/smoothing_filter.cc
    common_audio/third_party/ooura/fft_size_128/ooura_fft.cc
    common_audio/third_party/ooura/fft_size_128/ooura_fft_sse2.cc
    common_audio/third_party/ooura/fft_size_256/fft4g.cc
    common_audio/vad/vad.cc
    common_audio/vad/vad_core.c
    common_audio/vad/vad_filterbank.c
    common_audio/vad/vad_gmm.c
    common_audio/vad/vad_sp.c
    common_audio/vad/webrtc_vad.c
    common_audio/wav_file.cc
    common_audio/wav_header.cc
    common_audio/window_generator.cc
    modules/audio_processing/aec3/adaptive_fir_filter.cc
    modules/audio_processing/aec3/adaptive_fir_filter_avx2.cc
    modules/audio_processing/aec3/adaptive_fir_filter_erl.cc
    modules/audio_processing/aec3/adaptive_fir_filter_erl_avx2.cc
    modules/audio_processing/aec3/aec3_common.cc
    modules/audio_processing/aec3/aec3_fft.cc
    modules/audio_processing/aec3/aec_state.cc
    modules/audio_processing/aec3/alignment_mixer.cc
    modules/audio_processing/aec3/api_call_jitter_metrics.cc
    modules/audio_processing/aec3/block_buffer.cc
    modules/audio_processing/aec3/block_delay_buffer.cc
    modules/audio_processing/aec3/block_framer.cc
    modules/audio_processing/aec3/block_processor.cc
    modules/audio_processing/aec3/block_processor_metrics.cc
    modules/audio_processing/aec3/clockdrift_detector.cc
    modules/audio_processing/aec3/coarse_filter_update_gain.cc
    modules/audio_processing/aec3/comfort_noise_generator.cc
    modules/audio_processing/aec3/config_selector.cc
    modules/audio_processing/aec3/decimator.cc
    modules/audio_processing/aec3/dominant_nearend_detector.cc
    modules/audio_processing/aec3/downsampled_render_buffer.cc
    modules/audio_processing/aec3/echo_audibility.cc
    modules/audio_processing/aec3/echo_canceller3.cc
    modules/audio_processing/aec3/echo_path_delay_estimator.cc
    modules/audio_processing/aec3/echo_path_variability.cc
    modules/audio_processing/aec3/echo_remover.cc
    modules/audio_processing/aec3/echo_remover_metrics.cc
    modules/audio_processing/aec3/fft_data_avx2.cc
    modules/audio_processing/aec3/erl_estimator.cc
    modules/audio_processing/aec3/erle_estimator.cc
    modules/audio_processing/aec3/fft_buffer.cc
    modules/audio_processing/aec3/filter_analyzer.cc
    modules/audio_processing/aec3/frame_blocker.cc
    modules/audio_processing/aec3/fullband_erle_estimator.cc
    modules/audio_processing/aec3/matched_filter.cc
    modules/audio_processing/aec3/matched_filter_lag_aggregator.cc
    modules/audio_processing/aec3/moving_average_spectrum.cc
    modules/audio_processing/aec3/multi_channel_content_detector.cc
    modules/audio_processing/aec3/refined_filter_update_gain.cc
    modules/audio_processing/aec3/render_buffer.cc
    modules/audio_processing/aec3/render_delay_buffer.cc
    modules/audio_processing/aec3/render_delay_controller.cc
    modules/audio_processing/aec3/render_delay_controller_metrics.cc
    modules/audio_processing/aec3/render_signal_analyzer.cc
    modules/audio_processing/aec3/residual_echo_estimator.cc
    modules/audio_processing/aec3/reverb_decay_estimator.cc
    modules/audio_processing/aec3/reverb_frequency_response.cc
    modules/audio_processing/aec3/reverb_model.cc
    modules/audio_processing/aec3/reverb_model_estimator.cc
    modules/audio_processing/aec3/signal_dependent_erle_estimator.cc
    modules/audio_processing/aec3/spectrum_buffer.cc
    modules/audio_processing/aec3/stationarity_estimator.cc
    modules/audio_processing/aec3/subband_erle_estimator.cc
    modules/audio_processing/aec3/subband_nearend_detector.cc
    modules/audio_processing/aec3/subtractor.cc
    modules/audio_processing/aec3/subtractor_output.cc
    modules/audio_processing/aec3/subtractor_output_analyzer.cc
    modules/audio_processing/aec3/suppression_filter.cc
    modules/audio_processing/aec3/suppression_gain.cc
    modules/audio_processing/aec3/transparent_mode.cc
    modules/audio_processing/aec3/vector_math.cc
    modules/audio_processing/aec3/vector_math_avx2.cc
    modules/audio_processing/aec3/vector_math_sse2.cc
    modules/audio_processing/aec_dump/null_aec_dump_factory.cc
    modules/audio_processing/agc/agc.cc
    modules/audio_processing/agc/agc_manager_direct.cc
    modules/audio_processing/agc/loudness_histogram.cc
    modules/audio_processing/agc/legacy/analog_agc.cc
    modules/audio_processing/agc/legacy/digital_agc.cc
    modules/audio_processing/agc/utility.cc
    modules/audio_processing/agc2/adaptive_agc.cc
    modules/audio_processing/agc2/adaptive_digital_gain_controller.cc
    modules/audio_processing/agc2/adaptive_digital_gain_applier.cc
    modules/audio_processing/agc2/adaptive_mode_level_estimator.cc
    modules/audio_processing/agc2/biquad_filter.cc
    modules/audio_processing/agc2/clipping_predictor.cc
    modules/audio_processing/agc2/clipping_predictor_level_buffer.cc
    modules/audio_processing/agc2/clipping_predictor_evaluator.cc
    modules/audio_processing/agc2/cpu_features.cc
    modules/audio_processing/agc2/fixed_digital_level_estimator.cc
    modules/audio_processing/agc2/gain_applier.cc
    modules/audio_processing/agc2/gain_map_internal.cc
    modules/audio_processing/agc2/input_volume_controller.cc
    modules/audio_processing/agc2/input_volume_stats_reporter.cc
    modules/audio_processing/agc2/interpolated_gain_curve.cc
    modules/audio_processing/agc2/limiter.cc
    modules/audio_processing/agc2/noise_level_estimator.cc
    modules/audio_processing/agc2/saturation_protector.cc
    modules/audio_processing/agc2/saturation_protector_buffer.cc
    modules/audio_processing/agc2/speech_level_estimator.cc
    modules/audio_processing/agc2/speech_level_estimator_experimental_impl.cc
    modules/audio_processing/agc2/speech_level_estimator_impl.cc
    modules/audio_processing/audio_buffer.cc
    modules/audio_processing/audio_processing_impl.cc
    modules/audio_processing/capture_mixer/audio_content_analyzer.cc
    modules/audio_processing/capture_mixer/capture_mixer.cc
    modules/audio_processing/capture_mixer/channel_content_remixer.cc
    modules/audio_processing/capture_mixer/dc_levels_estimator.cc
    modules/audio_processing/capture_mixer/energy_estimator.cc
    modules/audio_processing/capture_mixer/remixing_logic.cc
    modules/audio_processing/capture_mixer/saturation_estimator.cc
    modules/audio_processing/capture_levels_adjuster/audio_samples_scaler.cc
    modules/audio_processing/capture_levels_adjuster/capture_levels_adjuster.cc
    modules/audio_processing/gain_control_impl.cc
    modules/audio_processing/gain_controller2.cc
    modules/audio_processing/high_pass_filter.cc
    modules/audio_processing/include/aec_dump.cc
    modules/audio_processing/include/audio_frame_proxies.cc
    modules/audio_processing/logging/apm_data_dumper.cc
    modules/audio_processing/ns/fast_math.cc
    modules/audio_processing/ns/histograms.cc
    modules/audio_processing/ns/noise_estimator.cc
    modules/audio_processing/ns/noise_suppressor.cc
    modules/audio_processing/ns/ns_fft.cc
    modules/audio_processing/ns/prior_signal_model.cc
    modules/audio_processing/ns/prior_signal_model_estimator.cc
    modules/audio_processing/ns/quantile_noise_estimator.cc
    modules/audio_processing/ns/signal_model.cc
    modules/audio_processing/ns/signal_model_estimator.cc
    modules/audio_processing/ns/speech_probability_estimator.cc
    modules/audio_processing/ns/suppression_params.cc
    modules/audio_processing/ns/wiener_filter.cc
    modules/audio_processing/post_filter.cc
    modules/audio_processing/residual_echo_detector.cc
    modules/audio_processing/rms_level.cc
    modules/audio_processing/splitting_filter.cc
    modules/audio_processing/three_band_filter_bank.cc
    modules/audio_processing/utility/cascaded_biquad_filter.cc
    modules/audio_processing/vad/gmm.cc
    modules/audio_processing/vad/pitch_based_vad.cc
    modules/audio_processing/vad/pitch_internal.cc
    modules/audio_processing/vad/pole_zero_filter.cc
    modules/audio_processing/vad/standalone_vad.cc
    modules/audio_processing/vad/vad_audio_proc.cc
    modules/audio_processing/vad/vad_circular_buffer.cc
    modules/audio_processing/vad/voice_activity_detector.cc
    modules/audio_coding/codecs/isac/main/source/filter_functions.c
    modules/audio_coding/codecs/isac/main/source/isac_vad.c
    modules/audio_coding/codecs/isac/main/source/pitch_estimator.c
    modules/audio_coding/codecs/isac/main/source/pitch_filter.c
    rtc_base/checks.cc
    rtc_base/cpu_info.cc
    rtc_base/denormal_disabler.cc
    rtc_base/event.cc
    rtc_base/event_tracer.cc
    rtc_base/logging.cc
    rtc_base/memory/aligned_malloc.cc
    rtc_base/platform_thread_types.cc
    rtc_base/platform_thread.cc
    rtc_base/race_checker.cc
    rtc_base/ref_counter.cc
    rtc_base/string_encode.cc
    rtc_base/string_to_number.cc
    rtc_base/string_utils.cc
    rtc_base/system/file_wrapper.cc
    rtc_base/system_time.cc
    rtc_base/strings/string_builder.cc
    rtc_base/synchronization/mutex.cc
    rtc_base/synchronization/yield_policy.cc
    rtc_base/task_queue_win.cc
    rtc_base/time_utils.cc
    system_wrappers/source/clock.cc
    system_wrappers/source/cpu_features.cc
    system_wrappers/source/field_trial.cc
    system_wrappers/source/metrics.cc
    pm_apm_shims/webrtc_apm_matched_filter_avx2_stub.cpp
    pm_apm_shims/webrtc_apm_vad_stub.cpp
)

set(WEBRTC_APM_EXISTING_SOURCES)
foreach(source IN LISTS WEBRTC_APM_SOURCES)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
        list(APPEND WEBRTC_APM_EXISTING_SOURCES "${source}")
    else()
        message(STATUS "Skipping missing WebRTC APM source: ${source}")
    endif()
endforeach()

add_library(webrtc_apm STATIC ${WEBRTC_APM_EXISTING_SOURCES})
if(MSVC)
    set_source_files_properties(
        common_audio/resampler/sinc_resampler_avx2.cc
        modules/audio_processing/aec3/adaptive_fir_filter_avx2.cc
        modules/audio_processing/aec3/adaptive_fir_filter_erl_avx2.cc
        modules/audio_processing/aec3/fft_data_avx2.cc
        modules/audio_processing/aec3/vector_math_avx2.cc
        PROPERTIES COMPILE_OPTIONS "/arch:AVX2"
    )
endif()
target_compile_features(webrtc_apm PUBLIC cxx_std_20)
target_include_directories(webrtc_apm PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}"
)
target_compile_definitions(webrtc_apm PUBLIC
    WEBRTC_APM_DEBUG_DUMP=0
    WEBRTC_STRICT_FIELD_TRIALS=0
    RTC_DISABLE_TRACE_EVENTS=1
)
target_link_libraries(webrtc_apm PUBLIC
    absl::any_invocable
    absl::base
    absl::strings
    absl::synchronization
)

if(WEBRTC_APM_BUILD_PROBE)
    add_executable(webrtc_apm_probe
        tools/apm_probe.cpp
    )

    target_compile_features(webrtc_apm_probe PRIVATE cxx_std_20)

    target_include_directories(webrtc_apm_probe PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}"
    )

    target_compile_definitions(webrtc_apm_probe PRIVATE
        WEBRTC_APM_PROBE=1
    )
endif()

if(WIN32)
    target_compile_definitions(webrtc_apm PUBLIC
        WEBRTC_WIN=1
    )
    target_compile_definitions(webrtc_apm PRIVATE
        WIN32_LEAN_AND_MEAN
        NOMINMAX
    )
    if(WEBRTC_APM_BUILD_PROBE)
        target_compile_definitions(webrtc_apm_probe PRIVATE
            WEBRTC_WIN=1
            WIN32_LEAN_AND_MEAN
            NOMINMAX
        )
    endif()
endif()

if(WIN32)
    target_link_libraries(webrtc_apm PUBLIC winmm)
endif()

if(MSVC)
    target_compile_options(webrtc_apm PRIVATE
        /permissive-
        /Zc:__cplusplus
        /wd4068
    )
    if(WEBRTC_APM_BUILD_PROBE)
        target_compile_options(webrtc_apm_probe PRIVATE
            /permissive-
            /Zc:__cplusplus
            /wd4068
        )
    endif()
endif()

if(WEBRTC_APM_BUILD_PROBE)
    target_link_libraries(webrtc_apm_probe PRIVATE webrtc_apm)
endif()
