#ifndef UART_DATA_OUT_H__
#define UART_DATA_OUT_H__

#include <string>
#include "SerialPeriph.h"

class UARTDataOut {
public:
    UARTDataOut(SerialPeriph* serial_out);
    UARTDataOut();
    ~UARTDataOut() = default;

    void SetOutputPort(SerialPeriph* serial_out);

    // This operator overload allows us to call them in chain
    // eg. obj << "Data1" << "Data2" << "Data3";
    // CPP Style !
    
    UARTDataOut& operator<<(std::string data);
    UARTDataOut& operator<<(const char* data);

private:
    SerialPeriph* m_serial_out;
};

namespace UART {
    extern UARTDataOut serialOut;
    static const std::string endl = "\n";
}

#endif //UART_DATA_OUT_H__