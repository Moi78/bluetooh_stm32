#ifndef SERIAL_PERIPH_H__
#define SERIAL_PERIPH_H__

#include <mbed.h>
#include <cstddef>
#include <string>

class SerialPeriph : public BufferedSerial {
public:
    SerialPeriph(PinName TX, PinName RX, int rate = 9600);
    virtual ~SerialPeriph();

    void PrintStr(std::string str);
    std::string ReadFromPort(bool flush = true);

    void Poll();
    bool GotReturn();

private:
    std::string m_buff;
    bool m_gotReturn;
};

#endif // SERIAL_PERIPH_H__
