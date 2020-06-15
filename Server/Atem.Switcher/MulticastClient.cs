using System;
using System.Collections.Generic;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;

namespace Atem.Switcher
{
    public delegate void DataReceived(UdpClient client, EndPoint endPoint, byte[] data);

    public class UdpSender : MulticastClient
    {

        public UdpSender(string ipAddress, int port)
            :this(new IPEndPoint(IPAddress.Parse(ipAddress), port))
        {

        }

        public UdpSender(IPEndPoint endPoint) 
            : base(endPoint, IPAddress.Any)
        {

        }


    }

    public abstract class MulticastClient : UdpClient
    {

        #region Fields

        readonly IPEndPoint endPoint;
        bool isDisposed;
        readonly Thread receiveThread;

        #endregion

        #region Properties

        public event DataReceived DataReceived;

        #endregion

        #region Constructor

        protected MulticastClient(IPEndPoint endPoint, IPAddress localAddress)
            : base(endPoint.AddressFamily)
        {
            this.endPoint = endPoint;
            Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.ReuseAddress, true);
            Client.SetSocketOption(SocketOptionLevel.IP, SocketOptionName.AddMembership, new MulticastOption(endPoint.Address, localAddress));
            Client.Bind(new IPEndPoint(endPoint.AddressFamily == AddressFamily.InterNetworkV6 ? IPAddress.IPv6Any : IPAddress.Any, endPoint.Port));
            receiveThread = new Thread(ReceiveHandler) { IsBackground = true, Name = $"Multicast Client Listener {endPoint}" };
            receiveThread.Start();
        }

        protected override void Dispose(bool disposing)
        {
            isDisposed = true;
            receiveThread.Abort();
            base.Dispose(disposing);
        }

        #endregion

        void ReceiveHandler()
        {
            while (!isDisposed)
            {
                try
                {
                    IPEndPoint endPoint = null;
                    var data = Receive(ref endPoint);
                    DataReceived?.Invoke(this, endPoint, data);
                }
                catch (ThreadAbortException)
                {
                    // Ignore
                }
            }
        }

        public void SendAll(byte[] data)
        {
            Send(data, data.Length, endPoint);
        }

    }



}
