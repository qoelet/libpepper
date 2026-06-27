#pragma once

inline int meterCount(float value, int n) {
    if (value <= 0.f) return 0;
    if (value >= 1.f) return n;
    int c = (int)(value * (float)n + 0.5f);
    return c > n ? n : c;
}

inline int activeKnob(const float* cur, const float* prev, int count, float threshold) {
    int   best = 0;
    float bestDelta = threshold;
    for (int i = 0; i < count; ++i) {
        float d = cur[i] - prev[i];
        if (d < 0.f) d = -d;
        if (d > bestDelta) { bestDelta = d; best = i + 1; }
    }
    return best;
}

inline int trackActiveKnob(const float* cur, float* ref, int count, float threshold) {
    int   best = 0;
    float bestDelta = threshold;
    for (int i = 0; i < count; ++i) {
        float d = cur[i] - ref[i];
        if (d < 0.f) d = -d;
        if (d > bestDelta) { bestDelta = d; best = i + 1; }
    }
    if (best) {
        for (int i = 0; i < count; ++i) {
            float d = cur[i] - ref[i];
            if (d < 0.f) d = -d;
            if (d > threshold) ref[i] = cur[i];
        }
    }
    return best;
}
