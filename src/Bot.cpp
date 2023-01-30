#include "Bot.h"

Bot::Bot() {
    m_monitor_enab = false;

    m_bot_io = new BotIO;

    m_error = 0.0f;

    // Setting PWM duty cycle
    m_bot_io->L.period(1.0f / 1000.0f);
    m_bot_io->R.period(1.0f / 1000.0f);
}

Bot::~Bot() {

}

void Bot::InitBot() {
    m_current_state = main_states_t::INIT;
    m_anti_spam.start();
}

void Bot::MainRoutine() {
    bool BP = !m_bot_io->BP;
    bool JACK = !m_bot_io->JACK;

    switch(m_current_state) {
        case main_states_t::INIT:
            if(!JACK) {
                m_current_state = main_states_t::RUN;
            }

            break;
        case main_states_t::RUN: 
            if(BP) {
                m_current_state = main_states_t::STOP;
            }

            break;

        case main_states_t::STOP:
            if(JACK) {
                m_current_state = main_states_t::INIT;
            }
            break;
    }

    if(m_monitor_enab && m_anti_spam.elapsed_time() > 0.5s) {
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

    UART::serialOut << stringify[m_current_state] << UART::endl;
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

    for(int i = 0; i < 5; i++) {
        busSelectMux = i;
        wait_us(1);

        m_capt_read[i] = anaIn.read();
    } 
}

void Bot::ShowSensorState() {
    for(auto& s : m_capt_read) {
        UART::serialOut << std::to_string((int)(s * 100.0f)) << "% ";
    }
    UART::serialOut << UART::endl;
}

void Bot::Forward(float speed) {
    m_bot_io->L.write(speed + (m_error / 100.0f));
    m_bot_io->R.write(speed);
}