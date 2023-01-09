#include "UARTDataOut.h"

UARTDataOut::UARTDataOut(SerialPeriph* serial_out) {
    m_serial_out = serial_out;
}

UARTDataOut::UARTDataOut() {
    m_serial_out = nullptr;
}

void UARTDataOut::SetOutputPort(SerialPeriph* serial_out) {
    m_serial_out = serial_out;
}

UARTDataOut& UARTDataOut::operator<<(std::string data) {
    if(!m_serial_out) {
        return *this;
    }

    m_serial_out->PrintStr(data);

    return *this;
}

UARTDataOut& UARTDataOut::operator<<(const char* data) {
    if(!m_serial_out) {
        return *this;
    }

    m_serial_out->PrintStr(std::string(data));

    return *this;
}

// Serial out global variable
namespace UART {
    UARTDataOut serialOut = UARTDataOut();
}