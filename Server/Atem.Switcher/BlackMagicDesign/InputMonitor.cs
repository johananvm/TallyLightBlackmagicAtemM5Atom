using BMDSwitcherAPI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Atem.Switcher
{
    class InputMonitor : IBMDSwitcherInputCallback
    {
        public event SwitcherEventHandler NameChanged;
        public event SwitcherEventHandler PreviewChanged;
        public event SwitcherEventHandler ProgramChanged;

        void IBMDSwitcherInputCallback.Notify(_BMDSwitcherInputEventType eventType)
        {
            switch (eventType)
            {
                case _BMDSwitcherInputEventType.bmdSwitcherInputEventTypeLongNameChanged:
                    NameChanged?.Invoke(this, null);
                    break;
                case _BMDSwitcherInputEventType.bmdSwitcherInputEventTypeIsProgramTalliedChanged:
                    ProgramChanged?.Invoke(this, null);
                    break;
                case _BMDSwitcherInputEventType.bmdSwitcherInputEventTypeIsPreviewTalliedChanged:
                    PreviewChanged?.Invoke(this, null);
                    break;
            }
        }
    }
}

