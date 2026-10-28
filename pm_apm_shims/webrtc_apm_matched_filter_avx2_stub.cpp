#include "modules/audio_processing/aec3/matched_filter.h"

namespace webrtc {

void MatchedFilterCore_AVX2(size_t /*x_start_index*/,
                            float /*x2_sum_threshold*/,
                            float /*smoothing*/,
                            std::span<const float> /*x*/,
                            std::span<const float> /*y*/,
                            std::span<float> /*h*/,
                            bool* filters_updated,
                            float* /*error_sum*/,
                            bool /*compute_accumulated_error*/,
                            std::span<float> /*accumulated_error*/,
                            std::span<float> /*scratch_memory*/)
{
    *filters_updated = false;
}

}  // namespace webrtc
