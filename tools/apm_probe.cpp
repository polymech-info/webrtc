#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "api/audio/audio_processing.h"
#include "api/audio/builtin_audio_processing_builder.h"
#include "api/environment/environment_factory.h"
#include "common_audio/wav_file.h"

namespace {

struct Options {
    int frames = 300;
    int sample_rate_hz = 16000;
    int render_delay_ms = 0;
    bool aec = true;
    bool ns = true;
    bool agc = false;
    std::string capture_wav;
    std::string render_wav;
    std::string out_wav;
};

bool ParseBoolFlag(const std::string& arg, const char* name)
{
    return arg == name;
}

int ParseIntValue(int argc, char** argv, int& i, const char* name, int fallback)
{
    if (i + 1 >= argc) {
        std::cerr << "missing value for " << name << "\n";
        std::exit(2);
    }
    ++i;
    return std::atoi(argv[i]);
}

Options ParseOptions(int argc, char** argv)
{
    Options options;
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--frames") {
            options.frames = ParseIntValue(argc, argv, i, "--frames", options.frames);
        } else if (arg == "--sample-rate") {
            options.sample_rate_hz =
                ParseIntValue(argc, argv, i, "--sample-rate", options.sample_rate_hz);
        } else if (arg == "--render-delay-ms") {
            options.render_delay_ms =
                ParseIntValue(argc, argv, i, "--render-delay-ms", options.render_delay_ms);
        } else if (ParseBoolFlag(arg, "--aec")) {
            options.aec = true;
        } else if (ParseBoolFlag(arg, "--no-aec")) {
            options.aec = false;
        } else if (ParseBoolFlag(arg, "--ns")) {
            options.ns = true;
        } else if (ParseBoolFlag(arg, "--no-ns")) {
            options.ns = false;
        } else if (ParseBoolFlag(arg, "--agc")) {
            options.agc = true;
        } else if (arg == "--capture-wav") {
            if (i + 1 >= argc) {
                std::cerr << "missing value for --capture-wav\n";
                std::exit(2);
            }
            options.capture_wav = argv[++i];
        } else if (arg == "--render-wav") {
            if (i + 1 >= argc) {
                std::cerr << "missing value for --render-wav\n";
                std::exit(2);
            }
            options.render_wav = argv[++i];
        } else if (arg == "--out-wav") {
            if (i + 1 >= argc) {
                std::cerr << "missing value for --out-wav\n";
                std::exit(2);
            }
            options.out_wav = argv[++i];
        } else if (ParseBoolFlag(arg, "--help")) {
            std::cout << "usage: webrtc_apm_probe [--frames N] [--sample-rate 16000]"
                         " [--aec|--no-aec] [--ns|--no-ns] [--agc]"
                         " [--capture-wav input.wav] [--render-wav far.wav]"
                         " [--render-delay-ms N] [--out-wav processed.wav]\n";
            std::exit(0);
        } else {
            std::cerr << "unknown option: " << arg << "\n";
            std::exit(2);
        }
    }
    return options;
}

double Rms(const std::vector<int16_t>& samples)
{
    if (samples.empty()) {
        return 0.0;
    }
    double sum = 0.0;
    for (const int16_t sample : samples) {
        const double value = static_cast<double>(sample);
        sum += value * value;
    }
    return std::sqrt(sum / static_cast<double>(samples.size()));
}

double Rms(const std::vector<int16_t>& samples, size_t count)
{
    if (count == 0) {
        return 0.0;
    }
    double sum = 0.0;
    for (size_t i = 0; i < count; ++i) {
        const double value = static_cast<double>(samples[i]);
        sum += value * value;
    }
    return std::sqrt(sum / static_cast<double>(count));
}

int16_t ClampToInt16(double value)
{
    value = std::clamp(value, -32768.0, 32767.0);
    return static_cast<int16_t>(std::lrint(value));
}

void FillSyntheticFrame(std::vector<int16_t>& render,
                        std::vector<int16_t>& capture,
                        int frame_index,
                        int sample_rate_hz)
{
    constexpr double kPi = 3.14159265358979323846;
    const double render_freq_hz = 440.0;
    const double speech_freq_hz = 880.0;
    const int frame_size = static_cast<int>(render.size());
    const int base_sample = frame_index * frame_size;

    for (int i = 0; i < frame_size; ++i) {
        const double t = static_cast<double>(base_sample + i) /
                         static_cast<double>(sample_rate_hz);
        const double far = 5000.0 * std::sin(2.0 * kPi * render_freq_hz * t);
        const double near = 1200.0 * std::sin(2.0 * kPi * speech_freq_hz * t);
        render[static_cast<size_t>(i)] = ClampToInt16(far);
        capture[static_cast<size_t>(i)] = ClampToInt16(near + 0.35 * far);
    }
}

std::vector<int16_t> ReadWavMono(const std::string& path,
                                 int* sample_rate_hz,
                                 size_t* num_channels)
{
    webrtc::WavReader reader(path);
    *sample_rate_hz = reader.sample_rate();
    *num_channels = reader.num_channels();
    std::vector<int16_t> samples(reader.num_samples());
    const size_t read = reader.ReadSamples(samples.size(), samples.data());
    samples.resize(read);
    return samples;
}

std::vector<int16_t> ResampleMonoLinear(const std::vector<int16_t>& input,
                                        int input_rate_hz,
                                        int output_rate_hz)
{
    if (input.empty() || input_rate_hz == output_rate_hz) {
        return input;
    }

    const double ratio = static_cast<double>(input_rate_hz) /
                         static_cast<double>(output_rate_hz);
    const size_t output_size =
        static_cast<size_t>(std::floor(static_cast<double>(input.size()) / ratio));
    std::vector<int16_t> output(output_size);
    for (size_t i = 0; i < output.size(); ++i) {
        const double src_pos = static_cast<double>(i) * ratio;
        const size_t src_i = static_cast<size_t>(src_pos);
        const size_t src_next = std::min(src_i + 1, input.size() - 1);
        const double frac = src_pos - static_cast<double>(src_i);
        const double value = static_cast<double>(input[src_i]) * (1.0 - frac) +
                             static_cast<double>(input[src_next]) * frac;
        output[i] = ClampToInt16(value);
    }
    return output;
}

}  // namespace

int main(int argc, char** argv)
{
    Options options = ParseOptions(argc, argv);
    std::unique_ptr<webrtc::WavReader> capture_reader;
    std::vector<int16_t> render_wav_samples;
    int render_wav_input_rate_hz = 0;
    if (!options.capture_wav.empty()) {
        capture_reader = std::make_unique<webrtc::WavReader>(options.capture_wav);
        options.sample_rate_hz = capture_reader->sample_rate();
        if (capture_reader->num_channels() != 1) {
            std::cerr << "only mono WAV input is supported for this probe; got "
                      << capture_reader->num_channels() << " channels\n";
            return 2;
        }
        if (options.frames == 300) {
            const int frame_size = webrtc::AudioProcessing::GetFrameSize(options.sample_rate_hz);
            options.frames = static_cast<int>(capture_reader->num_samples() /
                                              static_cast<size_t>(frame_size));
        }
    }
    if (!options.render_wav.empty()) {
        size_t render_channels = 0;
        render_wav_samples = ReadWavMono(
            options.render_wav, &render_wav_input_rate_hz, &render_channels);
        if (render_channels != 1) {
            std::cerr << "only mono render WAV input is supported for this probe; got "
                      << render_channels << " channels\n";
            return 2;
        }
        render_wav_samples = ResampleMonoLinear(
            render_wav_samples, render_wav_input_rate_hz, options.sample_rate_hz);
    }

    webrtc::AudioProcessing::Config config;
    config.echo_canceller.enabled = options.aec;
    config.high_pass_filter.enabled = true;
    config.noise_suppression.enabled = options.ns;
    config.gain_controller2.enabled = options.agc;

    webrtc::StreamConfig capture_stream(options.sample_rate_hz, 1);
    webrtc::ProcessingConfig processing_config;
    processing_config.input_stream() = capture_stream;
    processing_config.output_stream() = capture_stream;
    processing_config.reverse_input_stream() = capture_stream;
    processing_config.reverse_output_stream() = capture_stream;

    webrtc::Environment env = webrtc::CreateEnvironment();
    webrtc::scoped_refptr<webrtc::AudioProcessing> apm =
        webrtc::BuiltinAudioProcessingBuilder(config).Build(env);
    if (!apm) {
        std::cerr << "failed to build WebRTC AudioProcessing\n";
        return 1;
    }
    const int init_result = apm->Initialize(processing_config);
    if (init_result != 0) {
        std::cerr << "APM Initialize failed: " << init_result << "\n";
        return 1;
    }

    std::vector<int16_t> render(capture_stream.num_frames());
    std::vector<int16_t> capture(capture_stream.num_frames());
    std::vector<int16_t> processed(capture_stream.num_frames());
    std::vector<int16_t> reverse_out(capture_stream.num_frames());
    std::unique_ptr<webrtc::WavWriter> writer;
    if (!options.out_wav.empty()) {
        writer = std::make_unique<webrtc::WavWriter>(
            options.out_wav, options.sample_rate_hz, 1);
    }

    double input_rms_total = 0.0;
    double output_rms_total = 0.0;
    double render_rms_total = 0.0;
    int process_errors = 0;
    int frames_processed = 0;
    const int render_delay_samples =
        (options.render_delay_ms * options.sample_rate_hz) / 1000;

    for (int frame = 0; frame < options.frames; ++frame) {
        if (capture_reader) {
            const size_t samples_read =
                capture_reader->ReadSamples(capture.size(), capture.data());
            if (samples_read == 0) {
                break;
            }
            if (samples_read < capture.size()) {
                std::fill(capture.begin() + static_cast<std::ptrdiff_t>(samples_read),
                          capture.end(), 0);
            }
        } else {
            FillSyntheticFrame(render, capture, frame, options.sample_rate_hz);
        }
        if (!render_wav_samples.empty()) {
            const int frame_start = frame * static_cast<int>(render.size()) -
                                    render_delay_samples;
            for (size_t i = 0; i < render.size(); ++i) {
                const int source_index = frame_start + static_cast<int>(i);
                render[i] = source_index >= 0 &&
                                    source_index < static_cast<int>(render_wav_samples.size())
                                ? render_wav_samples[static_cast<size_t>(source_index)]
                                : 0;
            }
        } else if (capture_reader) {
            std::fill(render.begin(), render.end(), 0);
        }

        const int reverse_result = apm->ProcessReverseStream(
            render.data(), capture_stream, capture_stream, reverse_out.data());
        const int capture_result = apm->ProcessStream(
            capture.data(), capture_stream, capture_stream, processed.data());

        if (reverse_result != 0 || capture_result != 0) {
            ++process_errors;
        }

        render_rms_total += Rms(render);
        input_rms_total += Rms(capture);
        output_rms_total += Rms(processed, capture_reader ? capture.size() : processed.size());
        if (writer) {
            writer->WriteSamples(processed.data(), processed.size());
        }
        ++frames_processed;
    }

    const double frames = static_cast<double>(std::max(1, frames_processed));

    std::cout << "WebRTC APM smoke\n";
    std::cout << "  mode=" << (capture_reader ? "wav" : "synthetic") << "\n";
    if (capture_reader) {
        std::cout << "  capture_wav=" << options.capture_wav << "\n";
        if (!options.render_wav.empty()) {
            std::cout << "  render_wav=" << options.render_wav << "\n";
            std::cout << "  render_wav_input_sample_rate_hz="
                      << render_wav_input_rate_hz << "\n";
            std::cout << "  render_delay_ms=" << options.render_delay_ms << "\n";
        }
        if (writer) {
            std::cout << "  out_wav=" << options.out_wav << "\n";
        }
    }
    std::cout << "  frames=" << frames_processed << "\n";
    std::cout << "  sample_rate_hz=" << options.sample_rate_hz << "\n";
    std::cout << "  frame_size=" << capture_stream.num_frames() << "\n";
    std::cout << "  echo_canceller.enabled=" << config.echo_canceller.enabled << "\n";
    std::cout << "  high_pass_filter.enabled=" << config.high_pass_filter.enabled << "\n";
    std::cout << "  noise_suppression.enabled=" << config.noise_suppression.enabled << "\n";
    std::cout << "  gain_controller2.enabled=" << config.gain_controller2.enabled << "\n";
    std::cout << "  avg_render_rms=" << (render_rms_total / frames) << "\n";
    std::cout << "  avg_input_rms=" << (input_rms_total / frames) << "\n";
    std::cout << "  avg_output_rms=" << (output_rms_total / frames) << "\n";
    std::cout << "  process_errors=" << process_errors << "\n";
    std::cout << "  conclusion=real APM instantiates and processes frames\n";

    return process_errors == 0 ? 0 : 1;
}
