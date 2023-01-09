#ifndef BOT_H__
#define BOT_H__

#include <mbed.h>
#include <unordered_map>

#include "UARTDataOut.h"

enum main_states {
    INIT,
    RUN,
    STOP
} typedef main_states_t;

// Bot IO Map
struct BotIO {
    PwmOut L = PwmOut(PA_3);
    PwmOut R = PwmOut(PA_4);

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

    void PrintState();

    void ToggleMonitoring();
private:
    BotIO* m_bot_io;
    main_states_t m_current_state;

    bool m_monitor_enab;
    Timer m_anti_spam;
};

#endif //BOT_H__