#ifndef UDPV6_TRANSPORT_H
#define UDPV6_TRANSPORT_H

#include <boost/asio.hpp>
#include <boost/asio/ip/address_v6.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/thread.hpp>

#include "TransportInterface.h"
#include <vector>
#include <memory>
#include <map>

namespace eprosima{
namespace fastrtps{
namespace rtps{

class UDPv6Transport : public TransportInterface
{
public:
   /*Transport configuration
    * - bufferSize: length of the buffers used for transmission. Passing
    *               a buffer of different size will cause transmission to
    *               fail.
    *
    * - granularMode: Off by default (false). If enabled, a sender/receiver
    *                 resource will be returned per address, instead of per port.
    *                 (this means the channel->(port, direction) equivalency is changed
    *                 for channel->(port, IP, direction). 
    * */
   typedef struct {
      uint32_t sendBufferSize;
      uint32_t receiveBufferSize;
      bool granularMode; // Only supported as "false" for now.
   } TransportDescriptor;

   UDPv6Transport(const TransportDescriptor&);
   ~UDPv6Transport();

   // Checks whether there are open and bound sockets for the given port.
   virtual bool IsInputChannelOpen(Locator_t)         const;
   virtual bool IsOutputChannelOpen(Locator_t)        const;

   virtual bool IsLocatorSupported(Locator_t)         const;
   virtual bool DoLocatorsMatch(Locator_t, Locator_t) const;

   virtual Locator_t RemoteToMainLocal(Locator_t remote) const;

   virtual bool OpenInputChannel(Locator_t);
   virtual bool OpenOutputChannel(Locator_t);

   virtual bool CloseInputChannel(Locator_t);
   virtual bool CloseOutputChannel(Locator_t);

   virtual bool Send(const std::vector<char>& sendBuffer, Locator_t localLocator, Locator_t remoteLocator);
   virtual bool Receive(std::vector<char>& receiveBuffer, Locator_t localLocator, Locator_t & remoteLocator);

private:
   TransportDescriptor mDescriptor;

   // For UDPv6, the notion of channel corresponds to a port + direction tuple.
   // Outside granular mode, requesting an output port with any IP will trigger 
   // the binding of a socket per network interface.
	boost::asio::io_service mService;
   std::unique_ptr<boost::thread> ioServiceThread;

   mutable boost::recursive_mutex mOutputMapMutex;
   mutable boost::recursive_mutex mInputMapMutex;
   std::map<uint16_t, std::vector<boost::asio::ip::udp::socket> > mOutputSockets; // Maps port to socket collection.
   std::map<uint16_t, std::vector<boost::asio::ip::udp::socket> > mInputSockets;  // Maps port to socket collection.

   bool OpenAndBindOutputSockets(uint16_t port);
   bool OpenAndBindInputSockets(uint16_t port, boost::asio::ip::address_v6 multicastFilterAddress);
   boost::asio::ip::udp::socket OpenAndBindUnicastOutputSocket(boost::asio::ip::address_v6, uint32_t port);
   boost::asio::ip::udp::socket OpenAndBindMulticastInputSocket(uint32_t port, boost::asio::ip::address_v6 multicastFilterAddress);

   bool SendThroughSocket(const std::vector<char>& sendBuffer,
                          Locator_t remoteLocator,
                          boost::asio::ip::udp::socket& socket);

   void StartAsyncListen(std::vector<char>& receiveBuffer,
                         boost::asio::ip::udp::socket& socket, 
                         Locator_t remoteLocator, 
                         boost::interprocess::interprocess_semaphore& receiveSemaphore);
};

} // namespace rtps
} // namespace fastrtps
} // namespace eprosima

#endif
