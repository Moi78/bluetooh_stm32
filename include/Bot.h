#ifndef BOT_H__
#define BOT_H__

#include <mbed.h>
#include <unordered_map>
#include <chrono>
#include <array>

#include "UARTDataOut.h"
#define RACE_MODE 0

using namespace std::chrono_literals; 

enum main_states {
    INIT,
    RUN,
    STOP
} typedef main_states_t;

enum turn_state {
    STRAIGHT,
    LITTLE_LEFT,
    LITTLE_RIGHT,
    BIG_LEFT,
    BIG_RIGHT
} typedef turn_state_t;

// Jack en pos ==> Interrupteur fermé

// Bot IO Map
struct BotIO {
    PwmOut L = PwmOut(PB_4);
    PwmOut R = PwmOut(PB_5);

    bool BP = false;
    bool JACK = true;

    DigitalIn dBP = DigitalIn(PB_7, PullNone);
    DigitalIn dJACK = DigitalIn(PB_6, PullNone);
};

class Bot {
public:
    Bot();
    ~Bot();

    void InitBot();
    void MainRoutine();
    void FollowRoutine();

    BotIO* GetIO();
    main_states_t GetCurrentState();

    void Forward(float speed);
    void AddRight(float value);
    void AddLeft(float value);

    void Stop();

    void PrintState();

    void ToggleMonitoring();

    void SetError(float nError);
    float GetError();

    void SetK(float nK);
    void SetDiv(float nDiv);

    void SetSpeed(float speed);
    void SetLeft(float speed);
    void SetRight(float speed);

    void UpdateCaptRead();

    void ShowSensorState();
private:
    BotIO* m_bot_io;
    main_states_t m_current_state;
    turn_state_t m_follow_current_state;
    turn_state_t m_follow_future_state;

    float m_base_threashold;
    float m_threashold;

    bool m_monitor_enab;
    Timer m_anti_spam;
    
    Timer m_follow_tempo;

    float m_error;
    float m_k;
    float m_div;
    float m_speed;

    std::array<float, 5> m_capt_read;
};

#endif //BOT_H__