/*
 * tcpmodbus.cpp
 *
 *  Created on: Jul 20, 2019
 *      Author: atakan
 */

#include "tcpmodbus.h"

TcpModbus::TcpModbus(std::string host, uint16_t port):
_tcpConnectionStatus(false),
_tcpPort(0),
_fdSocketNumber(-1),
_slaveId(1),
_destinationHost("")
{
    _destinationHost = host;
    _tcpPort = port;
}

TcpModbus::~TcpModbus()
{
}

void TcpModbus::setSlaveId(int slaveId)
{
	_slaveId = slaveId;
}

bool TcpModbus::createSession()
{
	if(_destinationHost == "" || _tcpPort == 0) {
	        std::cout << "Missing Host and Port" << std::endl;
	        return false;
	    } else {
	        std::cout << "Found Proper Host "<< _destinationHost << " and Port " <<_tcpPort <<std::endl;
	    }

	    _fdSocketNumber = socket(AF_INET, SOCK_STREAM, 0);
	    if(_fdSocketNumber == -1) {
	        std::cout <<"Error Opening Socket" <<std::endl;
	        return false;
	    } else {
	        std::cout <<"Socket Opened Successfully" << std::endl;
	    }

	    _tcpSocketAddr.sin_family = AF_INET;
	    _tcpSocketAddr.sin_addr.s_addr = inet_addr(_destinationHost.c_str());
	    _tcpSocketAddr.sin_port = htons(_tcpPort);

	    if (connect(_fdSocketNumber, (struct sockaddr*)&_tcpSocketAddr, sizeof(_tcpSocketAddr)) < 0) {
	        std::cout<< "Connection Error" << std::endl;
	        return false;
	    }

	    std::cout<< "Connected" <<std::endl;
	    _tcpConnectionStatus = true;
	    return true;
}

void TcpModbus::closeSession()
{
    close(_fdSocketNumber);
    std::cout <<"Socket Closed" <<std::endl;
}

void TcpModbus::readHoldingRegisters(int dstRegister, int count, uint16_t *buffer)
{
    if(_tcpConnectionStatus) {
        if(count > 65535 || dstRegister > 65535) {
            std::cout << "register/count number will be lower than 65535 !" << std::endl;
            return;
        }
        modbus_read(dstRegister, count, READ_HOLDING_REGS);
        uint8_t package[MAXIMUM_MSG_LENGH];
        ssize_t k = modbus_receive(package);
        try {
            for(int i = 0; i < count; i++) {
                buffer[i] = ((uint16_t)package[9 + 2 * i]) << 8;
                buffer[i] += (uint16_t) package[10 + 2 * i];
            }
        } catch (std::exception &e) {
            std::cout<<e.what()<<std::endl;
            delete(&package);
            delete(&k);
            throw e;
        }
    } else {
        //throw modbus_connect_exception();
    }
}

void TcpModbus::modbus_read(int dstRegister, int count, int functionCode)
{
    uint8_t package[12];
    generatePackage(package, dstRegister, functionCode);
    package[5] = 6;
    package[10] = (uint8_t) (count >> 8);
    package[11] = (uint8_t) (count & 0x00FF);
    modbus_send(package, 12);
}

void TcpModbus::generatePackage(uint8_t *package, int dstRegister, int functionCode)
{
    package[0] = (uint8_t) _msg_id >> 8;
    package[1] = (uint8_t) (_msg_id & 0x00FF);
    package[2] = 0;
    package[3] = 0;
    package[4] = 0;
    package[6] = (uint8_t) _slaveId;
    package[7] = (uint8_t) functionCode;
    package[8] = (uint8_t) (dstRegister >> 8);
    package[9] = (uint8_t) (dstRegister & 0x00FF);
}

ssize_t TcpModbus::modbus_send(uint8_t *package, int length)
{
    _msg_id++; // !!!! make controller for max value
    return send(_fdSocketNumber, package, (size_t)length, 0);
}

ssize_t TcpModbus::modbus_receive(uint8_t *buffer)
{
    return recv(_fdSocketNumber, (char *) buffer, 1024, 0);
}
