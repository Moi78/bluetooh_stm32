#include "Bot.h"

BusOut leds(PB_3, PA_7, PA_6, PA_5, PA_3, PA_1, PA_0, PA_2);

Bot::Bot() {
    m_monitor_enab = false;

    m_bot_io = new BotIO;

    m_error = 0.0f;
    m_k = 0.001f;
    m_div = 1800.0f;
    m_speed = 0.25f;

    m_base_threashold = 0.20;
    m_threashold = m_base_threashold;

    m_follow_tempo.start();

    // Setting PWM duty cycle
    m_bot_io->L.period(1.0f / 15000.0f);
    m_bot_io->R.period(1.0f / 15000.0f);
}

Bot::~Bot() {

}

void Bot::InitBot() {
    m_current_state = main_states_t::INIT;
    m_follow_current_state = turn_state_t::STRAIGHT;
    m_anti_spam.start();
}

void Bot::MainRoutine() {
    bool BP = m_bot_io->BP;
    bool JACK = m_bot_io->JACK;

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
                break;
            }

            FollowRoutine();

            break;

        case main_states_t::STOP:
            if(JACK) {
                m_current_state = main_states_t::INIT;
            }

            Stop();

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
        {main_states_t::STOP, "STOP"}
    };

    std::unordered_map<turn_state_t, std::string> stringify_turn {
        {turn_state_t::LITTLE_LEFT, "LITTLE LEFT"},
        {turn_state_t::LITTLE_RIGHT, "LITTLE RIGHT"},
        {turn_state_t::STRAIGHT, "STAIGHT"},
        {turn_state_t::BIG_LEFT, "BIG LEFT"},
        {turn_state_t::BIG_RIGHT, "BIG RIGHT"}
    };

    UART::serialOut << stringify[m_current_state] << " " << stringify_turn[m_follow_current_state] << UART::endl;
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

        m_capt_read[i] = anaIn.read();

        leds_data |= (m_capt_read[i] < m_threashold) << i;
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
    UART::serialOut << "Contrast : " << std::to_string((int)(fabs(m_capt_read[3] - m_capt_read[2]) * 100.0f)) << UART::endl;
}

void Bot::Forward(float speed) {
    m_bot_io->L.write(speed + (m_error));
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


    if(CD && CG && CED && CEG) {
        UART::serialOut << "CROISEMENT" << UART::endl;
        return;
    } else if(CR && CEG && (!CD) && (!CED)) {
        UART::serialOut << "RACCOURCIS" << UART::endl;
        return;
    }

    m_follow_current_state = m_follow_future_state;
    switch (m_follow_current_state)
    {
        case turn_state_t::STRAIGHT:
            if(CG && (!CD)) {   //#define DERIVE_LEGER_GAUCHE CG && (!CD)
                m_follow_future_state = turn_state_t::LITTLE_LEFT;
            }

            if(CD && (!CG)) {
                m_follow_future_state = turn_state::LITTLE_RIGHT;
            }

            if(CED) {
                m_follow_future_state = turn_state_t::BIG_RIGHT;
            }

            if(CEG) {
                m_follow_future_state = turn_state_t::BIG_LEFT;
            }

            break;

        case turn_state_t::LITTLE_LEFT:
            if(CG && CD) {
                m_follow_future_state = turn_state_t::STRAIGHT;
            }

            if(CD && (!CG)) {
                m_follow_future_state = turn_state::LITTLE_RIGHT;
            }

            if(CED) {
                m_follow_future_state = turn_state_t::BIG_RIGHT;
            }

            if(CEG) {
                m_follow_future_state = turn_state_t::BIG_LEFT;
            }

            break;

        case turn_state_t::LITTLE_RIGHT:
            if(CG && CD) {
                m_follow_future_state = turn_state::STRAIGHT;
            }

            if(CG && (!CD)) {   //#define DERIVE_LEGER_GAUCHE CG && (!CD)
                m_follow_future_state = turn_state_t::LITTLE_LEFT;
            }

            if(CED) {
                m_follow_future_state = turn_state_t::BIG_RIGHT;
            }

            if(CEG) {
                m_follow_future_state = turn_state_t::BIG_LEFT;
            }

            break;

        case turn_state_t::BIG_LEFT:
            if(CG && CD) {
                m_follow_future_state = turn_state::STRAIGHT;
            }

            if(CG && (!CD)) {   //#define DERIVE_LEGER_GAUCHE CG && (!CD)
                m_follow_future_state = turn_state_t::LITTLE_LEFT;
            }

            break;

        case turn_state_t::BIG_RIGHT:
            if(CG && CD) {
                m_follow_future_state = turn_state::STRAIGHT;
            }

            if(CD && (!CG)) {
                m_follow_future_state = turn_state::LITTLE_RIGHT;
            }

            break;
    }

    switch(m_follow_current_state)
    {
        case turn_state_t::STRAIGHT:
            Forward(m_speed + 0.15f);
            break;

        case turn_state_t::LITTLE_LEFT:
            SetLeft(m_speed + m_error - m_k);
            SetRight((m_speed + (m_k / 3)));
            break;

        case turn_state_t::LITTLE_RIGHT:
            SetLeft(m_speed + m_error + (m_k / 3));
            SetRight(m_speed - m_k);
            break;

        case turn_state_t::BIG_LEFT:
            SetLeft(m_speed / m_div);
            SetRight(m_speed * m_div);
            break;

        case turn_state_t::BIG_RIGHT:
            SetLeft(m_speed * m_div);
            SetRight(m_speed / m_div);
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
