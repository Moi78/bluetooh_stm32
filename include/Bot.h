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
    STOP,
    SHRTCUT
} typedef main_states_t;

enum turn_state {
    STRAIGHT,
    LITTLE_LEFT,
    LITTLE_RIGHT,
    BIG_LEFT,
    BIG_RIGHT,
    MEGA_LEFT,
    MEGA_RIGHT
} typedef turn_state_t;

enum shortcut_state {
    SHT_INIT,
    SEEN1,
    BLACKOUT,
    SEEN2
} typedef shortcut_state_t;

enum shortcut_routine_state {
    SHTR_INIT,
    LEAVE,
    GOT,
    FORWARD
} typedef shortcut_routine_state_t;

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

    void ShortcutDetectionUpdate();
    void ShortcutRoutine();

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
    void SetMegaDiv(float nMegaDiv);

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

    shortcut_state_t m_shortcut_state;

    shortcut_routine_state_t m_shortcut_routine_state_c;
    shortcut_routine_state_t m_shortcut_routine_state_f;

    float m_base_threashold;
    float m_threashold;

    bool m_monitor_enab;
    Timer m_anti_spam;
    
    Timer m_follow_tempo;
    Timer m_shortcut_reset;
    Timer m_lap_time;

    float m_error;
    float m_k;
    float m_div;
    float m_mega_div;
    float m_speed;

    float m_speed_fac;
    int m_sht_count;

    float m_slow_fact;

    std::array<float, 5> m_capt_read;
    std::array<float, 5> m_capt_acq;
    
    uint64_t m_acq_count;
    int m_persistance_amount;

    Timer m_persistance_timer;
};

#endif //BOT_H__