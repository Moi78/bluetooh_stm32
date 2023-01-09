#include "Bot.h"

Bot::Bot() {
    m_monitor_enab = false;

    m_bot_io = new BotIO;

    // Setting PWM duty cycle
    m_bot_io->L.period(1.0f / 1000.0f);
    m_bot_io->R.period(1.0f / 1000.0f);
}

Bot::~Bot() {

}

void Bot::InitBot() {
    m_current_state = main_states_t::INIT;
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
}

BotIO* Bot::GetIO() {
    return m_bot_io;
}

main_states_t Bot::GetCurrentState() {
    return m_current_state;
}

void Bot::PrintState() {
    std::unordered_map<main_states_t, std::string> stringify {
        {main_states_t::INIT, "INIT\n"},
        {main_states_t::RUN, "RUN\n"},
        {main_states_t::STOP, "STOP\n"}
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