/*
 * tcpmodbus.h
 *
 *  Created on: Jul 20, 2019
 *      Author: atakan
 */

// headers
#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// MODBUS properties
#define MAXIMUM_MSG_LENGH 260

// MODBUS FUNCTION CODES
enum
{
    READ_COILS        = 0x01,  // read coil
    READ_INPUT_BITS   = 0x02,
    READ_HOLDING_REGS = 0x03,  // read holding registers
    READ_INPUT_REGS   = 0x04,
    WRITE_COIL        = 0x05,
    WRITE_REG         = 0x06,
    WRITE_COILS       = 0x0F,
    WRITE_REGS        = 0x10,
};

#ifndef TCPMODBUS_H_
#define TCPMODBUS_H_

class TcpModbus {
public:
    TcpModbus(std::string host, uint16_t port = 502);
    ~TcpModbus();
    void readHoldingRegisters(int dstRegister, int count, uint16_t *buffer);
    void setSlaveId(int slaveId);
    bool createSession();
private:
	bool _tcpConnectionStatus;
	uint16_t _tcpPort;
	int _fdSocketNumber;
    int _msg_id;
    int _slaveId ;
    int _destinationSlaveId;
    std::string _destinationHost;
    struct sockaddr_in _tcpSocketAddr;
    void closeSession();
    void modbus_read(int dstRegister, int count, int functionCode);
    void generatePackage(uint8_t *package, int dstRegister, int functionCode);
    ssize_t modbus_send(uint8_t *package, int length);
    ssize_t modbus_receive(uint8_t *buffer);
};

#endif /* TCPMODBUS_H_ */
