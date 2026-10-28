#include "modules/audio_processing/agc2/vad_wrapper.h"

namespace webrtc {

VoiceActivityDetectorWrapper::VoiceActivityDetectorWrapper(
    const AvailableCpuFeatures& cpu_features,
    int sample_rate_hz)
    : VoiceActivityDetectorWrapper(500, cpu_features, sample_rate_hz)
{
}

VoiceActivityDetectorWrapper::VoiceActivityDetectorWrapper(
    int vad_reset_period_ms,
    const AvailableCpuFeatures& /*cpu_features*/,
    int sample_rate_hz)
    : vad_reset_period_frames_(vad_reset_period_ms / 10),
      frame_size_(sample_rate_hz / 100),
      time_to_vad_reset_(vad_reset_period_frames_),
      vad_(nullptr),
      resampled_buffer_(),
      resampler_()
{
}

VoiceActivityDetectorWrapper::VoiceActivityDetectorWrapper(
    int vad_reset_period_ms,
    std::unique_ptr<MonoVad> vad,
    int sample_rate_hz)
    : vad_reset_period_frames_(vad_reset_period_ms / 10),
      frame_size_(sample_rate_hz / 100),
      time_to_vad_reset_(vad_reset_period_frames_),
      vad_(std::move(vad)),
      resampled_buffer_(),
      resampler_()
{
}

VoiceActivityDetectorWrapper::~VoiceActivityDetectorWrapper() = default;

float VoiceActivityDetectorWrapper::Analyze(DeinterleavedView<const float> /*frame*/)
{
    return 0.0f;
}

}  // namespace webrtc
