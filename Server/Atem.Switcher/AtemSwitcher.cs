using BMDSwitcherAPI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Atem.Switcher
{
    public class AtemSwitcher : IDisposable
    {

        #region Fields

        readonly IPAddress ipAddress;
        string name;

        IBMDSwitcher switcher;
        readonly IBMDSwitcherDiscovery switcherDiscovery;
        readonly SwitcherMonitor switcherMonitor;
        readonly List<Input> inputs;

        #endregion

        #region Properties

        public IEnumerable<Input> Inputs { get { return inputs; } }
        public bool IsConnected { get { return switcher != null; } }
        public string Name { get { return name; } }

        #endregion

        #region Constructor and Disposing


        public AtemSwitcher(string ipAddress)
            : this(IPAddress.Parse(ipAddress))
        {

        }

        public AtemSwitcher(IPAddress ipAddress)
        {
            this.ipAddress = ipAddress;
            inputs = new List<Input>();
            switcherDiscovery = new CBMDSwitcherDiscovery();
            switcherMonitor = new SwitcherMonitor();
            switcherMonitor.Disconnected += (s, e) => { Disconnect(); };
        }

        public void Dispose()
        {
            Disconnect();
        }

        #endregion

        public void Connect()
        {
            if (IsConnected)
                throw new InvalidOperationException("Already connected");

            _BMDSwitcherConnectToFailure failed = 0;
            try
            {
                switcherDiscovery.ConnectTo(ipAddress.ToString(), out switcher, out failed);
                switcher.GetProductName(out name);
                switcher.AddCallback(switcherMonitor);

                inputs.Clear();
                foreach (var item in GetInputs())
                    inputs.Add(new Input(this, item));
            }
            catch (COMException)
            {
                switch (failed)
                {
                    case _BMDSwitcherConnectToFailure.bmdSwitcherConnectToFailureNoResponse:
                        throw new Exception("No response from Switcher");
                    case _BMDSwitcherConnectToFailure.bmdSwitcherConnectToFailureIncompatibleFirmware:
                        throw new Exception("Switcher has incompatible firmware");
                    default:
                        throw new Exception("Connection failed for unknown reason");
                }
            }
        }

        public void Disconnect()
        {
            if (switcher == null)
                return;
            switcher.RemoveCallback(switcherMonitor);
            switcher = null;
            foreach (var item in inputs)
                item.Dispose();
            inputs.Clear();
        }

        internal void UpdateInputs()
        {

        }

        internal IEnumerable<IBMDSwitcherMixEffectBlock> GetMixEffectBlocks()
        {
            // Create a mix effect block iterator
            IntPtr meIteratorPtr;
            switcher.CreateIterator(typeof(IBMDSwitcherMixEffectBlockIterator).GUID, out meIteratorPtr);
            IBMDSwitcherMixEffectBlockIterator meIterator = Marshal.GetObjectForIUnknown(meIteratorPtr) as IBMDSwitcherMixEffectBlockIterator;
            if (meIterator == null)
                yield break;

            try
            {
                // Iterate through all mix effect blocks
                while (true)
                {
                    IBMDSwitcherMixEffectBlock me;
                    meIterator.Next(out me);

                    if (me != null)
                        yield return me;
                    else
                        break;
                }
            }
            finally
            {
                Marshal.ReleaseComObject(meIterator);
            }
        }

        internal IEnumerable<IBMDSwitcherInput> GetInputs()
        {
            // Create an input iterator
            IntPtr inputIteratorPtr;
            switcher.CreateIterator(typeof(IBMDSwitcherInputIterator).GUID, out inputIteratorPtr);
            var inputIterator = Marshal.GetObjectForIUnknown(inputIteratorPtr) as IBMDSwitcherInputIterator;
            if (inputIterator == null)
                yield break;

            try
            {
                // Scan through all inputs
                while (true)
                {
                    IBMDSwitcherInput input;
                    inputIterator.Next(out input);

                    if (input != null)
                        yield return input;
                    else
                        break;
                }
            }
            finally
            {
                Marshal.ReleaseComObject(inputIterator);
            }
        }

    }
}
