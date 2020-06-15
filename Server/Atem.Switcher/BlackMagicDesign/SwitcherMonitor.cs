using BMDSwitcherAPI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Atem.Switcher
{
    class SwitcherMonitor : IBMDSwitcherCallback
    {
        public event SwitcherEventHandler Disconnected;

        void IBMDSwitcherCallback.Notify(_BMDSwitcherEventType eventType, _BMDSwitcherVideoMode coreVideoMode)
        {
            if (eventType == _BMDSwitcherEventType.bmdSwitcherEventTypeDisconnected)
                Disconnected?.Invoke(this, null);
        }
    }
}
