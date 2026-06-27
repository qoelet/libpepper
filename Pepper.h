#pragma once

#include <Bela.h>
#include <Utilities.h>

struct PepperConfig {
    int   cvInPin[8]  = {0, 1, 2, 3, 4, 5, 6, 7};  // CVin/Pot 1..8 -> analog input channel
    int   cvOutPin[8] = {0, 1, 2, 3, 4, 5, 6, 7};  // CVout 1..8    -> analog output channel
    int   ledPin[10]   = {6, 7, 10, 2, 3, 0, 1, 4, 5, 8};  // LED 1..10  -> digital channel
    int   buttonPin[4] = {15, 14, 13, 12};                  // Button 1..4 -> digital channel
    float cvInVoltMax  = 10.f;
    float cvOutVoltMax = 5.f;
    int   buttonDebounceFrames = 2;
};

template <class D>
class Pepper {
public:
    explicit Pepper(const PepperConfig& cfg = PepperConfig{}) : cfg_(cfg) {}

    bool _setup(BelaContext* c) {
        ctx_ = c;
        audioRate_   = c->audioSampleRate;
        controlRate_ = c->analogSampleRate;
        for (int i = 0; i < 8; ++i) if ((unsigned)cfg_.cvInPin[i]  >= c->analogInChannels)  return false;
        for (int i = 0; i < 8; ++i) if ((unsigned)cfg_.cvOutPin[i] >= c->analogOutChannels) return false;
        for (int i = 0; i < 10; ++i) if ((unsigned)cfg_.ledPin[i]    >= c->digitalChannels) return false;
        for (int i = 0; i < 4;  ++i) if ((unsigned)cfg_.buttonPin[i] >= c->digitalChannels) return false;
        for (int i = 0; i < 10; ++i) pinMode(c, 0, cfg_.ledPin[i], OUTPUT);
        for (int i = 0; i < 4;  ++i) pinMode(c, 0, cfg_.buttonPin[i], INPUT);
        for (int i = 0; i < 4;  ++i) { btnLevel_[i] = btnRose_[i] = btnFell_[i] = false; btnCount_[i] = 0; }
        for (int i = 0; i < 10; ++i) ledState_[i] = false;
        return self().init();
    }

    void _render(BelaContext* c) {
        ctx_ = c;
        latchButtons();
        for (cf_ = 0; cf_ < c->analogFrames; ++cf_) self().control();
        applyLeds();
        for (af_ = 0; af_ < c->audioFrames; ++af_) self().audio();
    }

    void _cleanup(BelaContext* c) { (void)c; self().onCleanup(); }

protected:
    void control() {}
    void audio()   {}
    bool init()    { return true; }
    void onCleanup() {}

    float cvIn(int ch) noexcept {
        if (ch < 1 || ch > 8 || !ctx_) return 0.f;
        return analogRead(ctx_, cf_, cfg_.cvInPin[ch - 1]);
    }

    float cvInV(int ch) noexcept { return cvIn(ch) * cfg_.cvInVoltMax; }

    void cvOut(int ch, float v) noexcept {
        if (ch < 1 || ch > 8 || !ctx_) return;
        analogWrite(ctx_, cf_, cfg_.cvOutPin[ch - 1], constrain(v, 0.f, 1.f));
    }

    void cvOutV(int ch, float volts) noexcept {
        cvOut(ch, volts / cfg_.cvOutVoltMax);
    }

    float pot(int ch) noexcept { return cvIn(ch); }  // pot attenuates the CV jack: same channel

    float audioIn(int ch) noexcept {
        if (ch < 1 || ch > 2 || !ctx_) return 0.f;
        return audioRead(ctx_, af_, ch - 1);
    }

    void audioOut(int ch, float v) noexcept {
        if (ch < 1 || ch > 2 || !ctx_) return;
        audioWrite(ctx_, af_, ch - 1, constrain(v, -1.f, 1.f));
    }

    void led(int i, bool on) noexcept {
        if (i < 1 || i > 10) return;
        ledState_[i - 1] = on;
    }

    void ledLevel(int i, float b) noexcept { led(i, b >= 0.5f); }  // v1: threshold; PWM is future

    void applyLeds() noexcept {
        if (!ctx_) return;
        for (unsigned f = 0; f < ctx_->digitalFrames; ++f)
            for (int i = 0; i < 10; ++i)
                digitalWrite(ctx_, f, cfg_.ledPin[i], ledState_[i] ? 1.f : 0.f);
    }

    bool button(int i)     noexcept { return (i < 1 || i > 4) ? false : btnLevel_[i - 1]; }

    bool buttonRose(int i) noexcept { return (i < 1 || i > 4) ? false : btnRose_[i - 1]; }

    bool buttonFell(int i) noexcept { return (i < 1 || i > 4) ? false : btnFell_[i - 1]; }

    void latchButtons() noexcept {
        if (!ctx_) return;
        for (int i = 0; i < 4; ++i) {
            btnRose_[i] = false;
            btnFell_[i] = false;
            bool raw = digitalRead(ctx_, 0, cfg_.buttonPin[i]) > 0.5f;
            if (raw == btnLevel_[i]) {
                btnCount_[i] = 0;
            } else if (++btnCount_[i] >= cfg_.buttonDebounceFrames) {
                btnLevel_[i] = raw;
                btnCount_[i] = 0;
                btnRose_[i] =  raw;
                btnFell_[i] = !raw;
            }
        }
    }

    float audioRate()   const noexcept { return audioRate_; }

    float controlRate() const noexcept { return controlRate_; }

    PepperConfig cfg_;
    BelaContext* ctx_ = nullptr;
    unsigned int cf_ = 0;
    unsigned int af_ = 0;
    float audioRate_ = 0.f, controlRate_ = 0.f;
    bool ledState_[10] = {};
    bool btnLevel_[4] = {};  bool btnRose_[4] = {};  bool btnFell_[4] = {};
    int  btnCount_[4] = {};

private:
    D& self() noexcept { return static_cast<D&>(*this); }
};

#define PEPPER_MAIN(T)                                                              \
    static T gPepperInstance;                                                       \
    bool setup(BelaContext* context, void*) { return gPepperInstance._setup(context); } \
    void render(BelaContext* context, void*) { gPepperInstance._render(context); }       \
    void cleanup(BelaContext* context, void*) { gPepperInstance._cleanup(context); }
