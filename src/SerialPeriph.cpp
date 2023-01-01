#include "SerialPeriph.h"

SerialPeriph::SerialPeriph(PinName TX, PinName RX, int rate) :
BufferedSerial(TX, RX, rate) {
    m_gotReturn = false;
}

SerialPeriph::~SerialPeriph() {

}

void SerialPeriph::PrintStr(std::string str) {
    for(auto c : str) {
        write(&c, 1);
    }
}

std::string SerialPeriph::ReadFromPort(bool flush) {
    std::string copy_data;
    for(auto c : m_buff) {
        copy_data.push_back(c);
    }

    if(flush) {
        m_buff.clear();
        m_gotReturn = false;
    }

    return copy_data;
}

void SerialPeriph::Poll() {
    char r = 0x00;
    if(readable()) {
        read(&r, 1);
        m_gotReturn = r == '\n';

        m_buff.push_back(r);
    }
}

bool SerialPeriph::GotReturn() {
    return m_gotReturn;
}