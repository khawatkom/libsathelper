/*
 * Socket.h
 *
 *  Created on: 06/11/2016
 *      Author: Lucas Teske
 */

#ifndef INCLUDES_SOCKET_H_
#define INCLUDES_SOCKET_H_

#include <cstdint>
#include <stdint.h>
#include <memory.h>
#include <SatHelper/ipaddress.h>
#include <SatHelper/tools.h>

#ifdef _WIN32
#include <atomic>
#include <winsock2.h>
#include <Ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif


#if defined(__GNUC__) || defined(__MINGW32__)
#include <unistd.h>
#endif


namespace SatHelper {
    class Socket {
    private:
        #ifdef _WIN32
        static std::atomic_bool initialized;
        static std::atomic_uint sockCount;
        void socketInitialize();
        #endif
    protected:
        IPAddress address;
        struct sockaddr_in socketAddr;
        int s;

    public:
        Socket() :
                Socket(IPAddress()) {
            memset(&socketAddr, 0x00, sizeof(sockaddr_in));
            #ifdef _WIN32
            socketInitialize();
            #endif
        }

        Socket(IPAddress addr) :
                address(addr), s(0) {
            #ifdef _WIN32
            socketInitialize();
            #endif
        }

        Socket(std::string addr) :
                address(IPAddress(addr)), s(0) {
            #ifdef _WIN32
            socketInitialize();
            #endif
        }

        Socket(IPAddress addr, int socket) :
                address(addr), s(socket) {
            #ifdef _WIN32
            socketInitialize();
            #endif
        }

        Socket(const Socket &a) :
                Socket(a.address, a.s) {
            #ifdef _WIN32
            socketInitialize();
            #endif
            socketAddr = a.socketAddr;
        }

        virtual ~Socket();

        void Receive(char *data, int length);
        void Send(char *data, int length);
        void SendTo(char *data, int length, IPAddress destinationAddress, int destinationPort);
        int ReceiveFrom(char *data, int length, IPAddress fromAddress, int fromPort);
        uint64_t AvailableData();

#ifdef _MSC_VER
		inline void usleep(DWORD waitTime) {
			LARGE_INTEGER perfCnt, start, now;

			QueryPerformanceFrequency(&perfCnt);
			QueryPerformanceCounter(&start);

			do {
				QueryPerformanceCounter((LARGE_INTEGER*) &now);
			} while ((now.QuadPart - start.QuadPart) / float(perfCnt.QuadPart) * 1000 * 1000 < waitTime);
		}
#endif

        inline const IPAddress GetAddress() const {
            return address;
        }

        inline const void WaitForData(uint64_t bytes, uint32_t timeout) {
            uint32_t checkTime = Tools::getTimestamp();
            while (AvailableData() < bytes) {
                if (Tools::getTimestamp() - checkTime > timeout) {
                    return;
                }
                usleep(10);
            }
        }

        virtual void Close();
    };
}
#endif /* INCLUDES_SOCKET_H_ */
