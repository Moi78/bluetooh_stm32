#ifndef BOT_H__
#define BOT_H__

enum main_states {
    INIT,
    RUN,
    STOP
} typedef main_states_t;

void MainRoutine(main_states_t& current, DigitalIn bp, DigitalIn jack) {
    bool BP = !bp;
    bool JACK = !jack;

    switch(current) {
        case main_states_t::INIT:
            if(!JACK) {
                current = main_states_t::RUN;
            }

            break;
        case main_states_t::RUN: 
            if(BP) {
                current = main_states_t::STOP;
            }

            break;

        case main_states_t::STOP:
            if(JACK) {
                current = main_states_t::INIT;
            }
            break;
    }
}

#endif //BOT_H__