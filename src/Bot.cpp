#include "Bot.h"

BusOut leds(PB_3, PA_7, PA_6, PA_5, PA_3, PA_1, PA_0, PA_2);

Bot::Bot() {
    m_monitor_enab = false;

    m_bot_io = new BotIO;

    m_error = 0.0f;
    m_k = 0.001f;
    m_div = 1800.0f;
    m_speed = 0.25f;
    m_slow_fact = 0.0f;

    m_speed_fac = 1.0f;
    m_sht_count = 0;
    
    m_persistance_amount = 200;

    m_base_threashold = 0.20;
    m_threashold = m_base_threashold;

    m_follow_tempo.start();
    m_shortcut_reset.start();

    for(int i = 0; i < 5; i++) {
        m_capt_stamp[i].start();
    }

    // Setting PWM duty cycle
    m_bot_io->L.period(1.0f / 15000.0f);
    m_bot_io->R.period(1.0f / 15000.0f);
}

Bot::~Bot() {

}

void Bot::InitBot() {
    m_current_state = main_states_t::INIT;
    m_follow_current_state = turn_state_t::STRAIGHT;
    m_shortcut_state = shortcut_state_t::SHT_INIT;
    m_shortcut_routine_state_c = shortcut_routine_state_t::SHTR_INIT;

    m_anti_spam.start();
    m_persistance_timer.start();
}

void Bot::MainRoutine() {
    bool BP = m_bot_io->BP;
    bool JACK = m_bot_io->JACK;

    if(m_sht_count == 1) {
        m_speed_fac = 0.78f;
    } else if((m_shortcut_state != shortcut_state_t::SEEN1) || (m_sht_count > 1)) {
        m_speed_fac = 1.0f;
    }

    switch(m_current_state) {
        case main_states_t::INIT:
            if(!JACK) {
                m_current_state = main_states_t::RUN;

                Forward(m_speed);
                m_lap_time.start();
            }

            break;
        case main_states_t::RUN: 
            if(BP) {
                m_current_state = main_states_t::STOP;

                m_lap_time.stop();
                UART::serialOut << "Lap time : " << std::to_string((int)m_lap_time.read()) << UART::endl;
                m_lap_time.reset();

                break;
            }

            if((m_shortcut_state == shortcut_state_t::SEEN2) && (m_sht_count < 2)) {
                m_current_state = main_states_t::SHRTCUT;
                m_shortcut_routine_state_f = shortcut_routine_state_t::SHTR_INIT;
                m_shortcut_state = shortcut_state_t::SHT_INIT;

                UART::serialOut << "SHORTCUT" << UART::endl;

                break;
            }

            FollowRoutine();

            break;

        case main_states_t::STOP:
            if(JACK) {
                m_current_state = main_states_t::INIT;
            }

            Stop();

            m_sht_count = 0;
            m_speed_fac = 1.0f;

            break;

        case main_states_t::SHRTCUT:
            if(BP) {
                m_current_state = main_states_t::STOP;

                m_lap_time.stop();
                UART::serialOut << "Lap time : " << std::to_string((int)m_lap_time.read()) << UART::endl;
                m_lap_time.reset();

                break;
            }

            ShortcutRoutine();

            break;
    }

    if(m_monitor_enab && m_anti_spam.elapsed_time() > 0.3s) {
        UART::serialOut << "-----------------------------" << UART::endl;
        m_anti_spam.reset();
        PrintState();
        ShowSensorState();
    }
}

BotIO* Bot::GetIO() {
    return m_bot_io;
}

main_states_t Bot::GetCurrentState() {
    return m_current_state;
}

void Bot::PrintState() {
    std::unordered_map<main_states_t, std::string> stringify {
        {main_states_t::INIT, "INIT"},
        {main_states_t::RUN, "RUN"},
        {main_states_t::STOP, "STOP"},
        {main_states_t::SHRTCUT, "SHORTCUT"}
    };

    std::unordered_map<turn_state_t, std::string> stringify_turn {
        {turn_state_t::LITTLE_LEFT, "LITTLE LEFT"},
        {turn_state_t::LITTLE_RIGHT, "LITTLE RIGHT"},
        {turn_state_t::STRAIGHT, "STAIGHT"},
        {turn_state_t::BIG_LEFT, "BIG LEFT"},
        {turn_state_t::BIG_RIGHT, "BIG RIGHT"},
        {turn_state_t::MEGA_LEFT, "MEGA LEFT"},
        {turn_state_t::MEGA_RIGHT, "MEGA RIGHT"}
    };

    std::unordered_map<shortcut_state_t, std::string> stringify_shortcut {
        {shortcut_state_t::SHT_INIT, "SHT INIT"},
        {shortcut_state_t::SEEN1, "SEEN1"},
        {shortcut_state_t::SEEN2, "SEEN2"},
        {shortcut_state_t::BLACKOUT, "BLACKOUT"}
    };

    UART::serialOut << stringify[m_current_state] << " " << stringify_turn[m_follow_current_state] << " " << stringify_shortcut[m_shortcut_state] << UART::endl;
}

void Bot::ToggleMonitoring() {
    m_monitor_enab = !m_monitor_enab;

    if(m_monitor_enab) {
        UART::serialOut << "Monitoring enabled" << UART::endl;
    } else {
        UART::serialOut << "Monitoring disabled" << UART::endl;
    }
}

void Bot::SetError(float nError) {
    m_error = nError;
}

float Bot::GetError() {
    return m_error;
}

void Bot::UpdateCaptRead() {
    BusOut busSelectMux(PA_8, PF_1, PF_0);
    AnalogIn anaIn(PB_1);

    uint8_t leds_data = 0;
    for(int i = 0; i < 5; i++) {
        busSelectMux = i;
        wait_us(1);

        if(i == 0) {
            if(m_capt_read[i] > m_base_threashold) {
                m_capt_read[i] = anaIn.read();
                m_capt_stamp[i].reset();
            } else if((m_capt_stamp[i].read_ms() >= m_persistance_amount)) {
                m_capt_read[i] = anaIn.read();
                m_capt_stamp[i].reset();
            }
        } else {
            m_capt_read[i] = anaIn.read();
        }

        leds_data |= (m_capt_read[i] <= m_base_threashold) << i;
    }
    leds = leds_data;

    m_bot_io->BP = m_bot_io->dBP;
    m_bot_io->JACK = m_bot_io->dJACK;
}

void Bot::ShowSensorState() {
    for(auto& s : m_capt_read) {
        UART::serialOut << std::to_string((int)(s * 100.0f)) << "% ";
    }

    UART::serialOut << UART::endl;

    for(auto& s : m_capt_read) {
        UART::serialOut << std::to_string((int)(s * 100.0f) <= m_threashold * 100.0f) << " ";
    }

    UART::serialOut << UART::endl << "Jack: " << std::to_string(m_bot_io->JACK) << " BP: " << std::to_string(m_bot_io->BP) << UART::endl;
    UART::serialOut << "PWM L: " << std::to_string((int)(m_bot_io->L.read() * 100.0f)) << " PWM R: " << std::to_string((int)(m_bot_io->R.read() * 100.0f)) << UART::endl;
    UART::serialOut << "Sht Count : " << std::to_string(m_sht_count) << UART::endl;
}

void Bot::Forward(float speed) {
    m_bot_io->L.write(speed - 0.02);
    m_bot_io->R.write(speed);
}

void Bot::Stop() {
    m_bot_io->L.write(0);
    m_bot_io->R.write(0);
}

void Bot::FollowRoutine() {
    float threshold = m_base_threashold;
    
    bool CD = m_capt_read[3] <= threshold; // Is white ?
    bool CG = m_capt_read[2] <= threshold; // Is white ?

    bool CED = m_capt_read[4] <= threshold;
    bool CEG = m_capt_read[1] <= threshold;

    bool CR = m_capt_read[0] <= threshold;

    if(m_current_state == main_states_t::SHRTCUT) {
        return;
    }

    if(CED && CEG) {
        return;
    } else if(CR && CEG) {
        return;
    }

    m_follow_current_state = m_follow_future_state;
    switch (m_follow_current_state)
    {
        case turn_state_t::STRAIGHT:
            if(CG && (!CD)) {
                m_follow_future_state = turn_state_t::LITTLE_RIGHT;
            }

            if(CG && (!CD)) {
                m_follow_future_state = turn_state_t::LITTLE_LEFT;
            }

            if(CD && (!CG)) {
                m_follow_future_state = turn_state_t::LITTLE_RIGHT;
            }

            break;

        case turn_state_t::LITTLE_LEFT:
            if(CD && CG) {
                m_follow_future_state = turn_state_t::STRAIGHT;
            }

            if(CEG && CG) {
                m_follow_future_state = turn_state_t::BIG_LEFT;
            } else if(CEG) {
                m_follow_future_state = turn_state_t::MEGA_LEFT;
            }

            break;

        case turn_state_t::LITTLE_RIGHT:
            if(CD && CG) {
                m_follow_future_state = turn_state_t::STRAIGHT;
            }

            if(CED && CD) {
                m_follow_future_state = turn_state_t::BIG_RIGHT;
            } else if(CED) {
                m_follow_future_state = turn_state_t::MEGA_RIGHT;
            }

            break;

        case turn_state_t::BIG_LEFT:
            if(CD && CG) {
                m_follow_future_state = turn_state_t::STRAIGHT;
            }

            if(CEG) {
                m_follow_future_state = turn_state_t::MEGA_LEFT;
            }

            break;

        case turn_state_t::BIG_RIGHT:
            if(CD && CG) {
                m_follow_future_state = turn_state_t::STRAIGHT;
            }

            if(CED) {
                m_follow_future_state = turn_state_t::MEGA_RIGHT;
            }

            break;

        case turn_state_t::MEGA_LEFT:
            if(CD && CG) {
                m_follow_future_state = turn_state_t::STRAIGHT;
            }

            if(CEG && CG) {
                m_follow_future_state = turn_state_t::BIG_LEFT;
            }

            if(CG && (!CD)) {
                m_follow_future_state = turn_state_t::LITTLE_LEFT;
            }

            if(CD && (!CG)) {
                m_follow_future_state = turn_state_t::LITTLE_RIGHT;
            }

            break;

        case turn_state_t::MEGA_RIGHT:
            if(CD && CG) {
                m_follow_future_state = turn_state_t::STRAIGHT;
            }

            if(CED && CD) {
                m_follow_future_state = turn_state_t::BIG_RIGHT;
            }

            if(CG && (!CD)) {
                m_follow_future_state = turn_state_t::LITTLE_LEFT;
            }

            if(CD && (!CG)) {
                m_follow_future_state = turn_state_t::LITTLE_RIGHT;
            }

            break;
    }

    float fspeed = m_speed_fac * m_speed;

    switch(m_follow_current_state)
    {
        case turn_state_t::STRAIGHT:
            Forward(fspeed + 0.18f);
            break;

        case turn_state_t::LITTLE_LEFT:
            SetLeft(fspeed / m_error);
            SetRight(fspeed * m_error);
            break;

        case turn_state_t::LITTLE_RIGHT:
            SetLeft(fspeed * 1.059f);
            SetRight(fspeed / 1.059f);
            break;

        case turn_state_t::BIG_LEFT:
            SetLeft(fspeed / m_div);
            SetRight(fspeed * m_div);
            break;

        case turn_state_t::BIG_RIGHT:
            SetLeft(fspeed * m_div);
            SetRight(fspeed / m_div);
            break;

        case turn_state_t::MEGA_LEFT:
            SetLeft(fspeed / m_mega_div);
            SetRight(fspeed * m_mega_div);
            break;

        case turn_state_t::MEGA_RIGHT:
            SetLeft(fspeed * m_mega_div);
            SetRight(fspeed / m_mega_div);
            break;
    }
}

void Bot::ShortcutDetectionUpdate() {
    if(m_shortcut_reset.read() > 1.8f) {
        m_shortcut_state = shortcut_state_t::SHT_INIT;
        m_shortcut_reset.reset();
    }

    bool CEG = m_capt_read[1] <= m_base_threashold;
    bool CR = m_capt_read[0] <= m_base_threashold;
    bool CG = m_capt_read[2] <= m_base_threashold; // Is white ?
    bool CD = m_capt_read[3] <= m_base_threashold; // Is white ?

    bool CED = m_capt_read[4] <= m_base_threashold;

    if(CED && CG && CD && CEG && (m_sht_count != 1)) {
        return;
    }

    switch(m_shortcut_state) {
        case shortcut_state_t::SHT_INIT:
            if(CEG && CR && CG) {
                m_shortcut_state = shortcut_state_t::SEEN1;
            }

            m_speed_fac = 1;

            break;

        case shortcut_state_t::SEEN1:
            if(!CEG && !CG && !CR) {
                m_shortcut_state = shortcut_state_t::BLACKOUT;
            }

            m_speed_fac = 0.6f;

            break;

        case shortcut_state_t::BLACKOUT:
            if(CEG && CR && CG) {
                m_shortcut_state = shortcut_state_t::SEEN2;
            }

            break;

        case shortcut_state_t::SEEN2:
            if(!CR && !CG) {
                m_shortcut_state = shortcut_state_t::SHT_INIT;
            }

            break;
    }
}

void Bot::ShortcutRoutine() {
    bool CD = m_capt_read[3] <= m_base_threashold; // Is white ?
    bool CG = m_capt_read[2] <= m_base_threashold; // Is white ?

    m_shortcut_routine_state_c = m_shortcut_routine_state_f;
    switch(m_shortcut_routine_state_c) {
        case shortcut_routine_state_t::SHTR_INIT:
            if(!CD && !CG) {
                m_shortcut_routine_state_f = shortcut_routine_state_t::LEAVE;
                m_sht_count++;
            }

            break;

        case shortcut_routine_state_t::LEAVE:
            if(CD && CG) {
                m_shortcut_routine_state_f = shortcut_routine_state_t::GOT;
            }

            break;

        case shortcut_routine_state_t::GOT:
            m_current_state = main_states_t::RUN;
            m_shortcut_routine_state_f = shortcut_routine_state_t::SHTR_INIT;
            m_shortcut_state = shortcut_state_t::SHT_INIT;

            break;

        case shortcut_routine_state_t::FORWARD:
            break;
    }

    switch(m_shortcut_routine_state_c) {
        case shortcut_routine_state_t::SHTR_INIT:
            SetLeft(0);
            SetRight(0.28);

            break;

        case shortcut_routine_state_t::LEAVE:
            SetLeft(0);
            SetRight(0.28);

            break;

        case shortcut_routine_state_t::GOT:
            break;

        case shortcut_routine_state_t::FORWARD:
            break;
    }
}

void Bot::SetK(float nK) {
    m_k = nK;
}

void Bot::AddLeft(float value) {
    float currentVal = m_bot_io->L.read();

    m_bot_io->L.write(currentVal + value);
}

void Bot::AddRight(float value) {
    float currentVal = m_bot_io->R.read();

    m_bot_io->R.write(currentVal + value);
}

void Bot::SetDiv(float nDiv) {
    m_div = nDiv;
}

void Bot::SetSpeed(float speed) {
    m_speed = speed;
}

void Bot::SetLeft(float speed) {
    m_bot_io->L.write(speed);
} 

void Bot::SetRight(float speed) {
    m_bot_io->R.write(speed);
}

void Bot::SetMegaDiv(float nMegaDiv) {
    m_mega_div = nMegaDiv;
}