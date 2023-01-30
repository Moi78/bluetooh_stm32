#ifndef BOT_H__
#define BOT_H__

#include <mbed.h>
#include <unordered_map>
#include <chrono>
#include <array>

#include "UARTDataOut.h"

using namespace std::chrono_literals; 

enum main_states {
    INIT,
    RUN,
    STOP
} typedef main_states_t;

// Bot IO Map
struct BotIO {
    PwmOut L = PwmOut(PB_4);
    PwmOut R = PwmOut(PB_5);

    DigitalIn BP = DigitalIn(A0, PullUp);
    DigitalIn JACK = DigitalIn(A1, PullUp);
};

class Bot {
public:
    Bot();
    ~Bot();

    void InitBot();
    void MainRoutine();

    BotIO* GetIO();
    main_states_t GetCurrentState();

    void Forward(float speed);

    void PrintState();

    void ToggleMonitoring();

    void SetError(float nError);
    float GetError();

    void UpdateCaptRead();

    void ShowSensorState();
private:
    BotIO* m_bot_io;
    main_states_t m_current_state;

    bool m_monitor_enab;
    Timer m_anti_spam;

    float m_error;

    std::array<float, 5> m_capt_read;
};

#endif //BOT_H__