// Generated verbatim purple ABTestRender scoring (MIT).
#pragma once
#include <array>
#include <cmath>
namespace th15 {
inline std::array<float,6> practice_ab_scores(std::array<float,5> result_origs){
                std::array<float,6> scores = { 0, 0.1, 0.5, 0.6, 0.9, 1.0 };
                // calculates
                {
                    float div[5] = { 3.5f, 4.8f, 4.0f, 3.1f, 3.0f };
                    float pows[5] = { 1.2, 1.1, 1.1, 1.2, 2.05 };
                    for (int i = 0; i < 5; i++) {
                        result_origs[i] = (result_origs[i] - 3.0f) / div[i];
                        result_origs[i] = powf(result_origs[i], pows[i]);
                    }
                    scores[0] = 0.6f * result_origs[0] + 0.15f * result_origs[1] + 0.05f * result_origs[2] + 0.15f * result_origs[3] + 0.05f * result_origs[4];
                    scores[1] = 0.0f * result_origs[0] + 0.5f * result_origs[1] + 0.2f * result_origs[2] + 0.15f * result_origs[3] + 0.15f * result_origs[4];
                    scores[2] = 0.0f * result_origs[0] + 0.1f * result_origs[1] + 0.75f * result_origs[2] + 0.05f * result_origs[3] + 0.1f * result_origs[4];
                    scores[3] = 0.1f * result_origs[0] + 0.4f * result_origs[1] + 0.0f * result_origs[2] + 0.5f * result_origs[3] + 0.0f * result_origs[4];
                    scores[4] = 0.05f * result_origs[0] + 0.05f * result_origs[1] + 0.0f * result_origs[2] + 0.1f * result_origs[3] + 0.8f * result_origs[4];
                    scores[5] = 0.1f * result_origs[0] + 0.3f * result_origs[1] + 0.3f * result_origs[2] + 0.2f * result_origs[3] + 0.1f * result_origs[4];
                    for (int i = 0; i < 6; i++) {
                        scores[i] = 1.0f / (1.0f + expf(1.8f - 4.0f * scores[i]));
                    }
                }
 return scores;
}
}
