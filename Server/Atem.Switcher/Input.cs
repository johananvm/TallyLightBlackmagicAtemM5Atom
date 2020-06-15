using BMDSwitcherAPI;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Atem.Switcher
{
    public class Input : NotifyPropertyChanged, IDisposable
    {

        #region Fields

        InputMonitor inputMonitor;
        IBMDSwitcherInput input;

        #endregion

        #region Properties

        public int Id { get; }

        string shortName;
        public string ShortName
        {
            get { return shortName; }
            private set
            {
                if (Equals(shortName, value))
                    return;
                shortName = value;
                OnPropertyChanged(nameof(ShortName));
            }
        }

        string name;
        public string Name
        {
            get { return name; }
            private set
            {
                if (Equals(name, value))
                    return;
                name = value;
                OnPropertyChanged(nameof(Name));
            }
        }

        bool isProgram;
        public bool IsProgram
        {
            get { return isProgram; }
            private set
            {
                if (Equals(isProgram, value))
                    return;
                isProgram = value;
                OnPropertyChanged(nameof(IsProgram));
            }
        }

        bool isPreview;
        public bool IsPreview
        {
            get { return isPreview; }
            private set 
            {
                if (Equals(isPreview, value))
                    return;
                isPreview = value;
                OnPropertyChanged(nameof(IsPreview));
            }
        }

        #endregion

        #region Constructors

        internal Input(AtemSwitcher switcher, IBMDSwitcherInput input)
        {
            inputMonitor = new InputMonitor();
            inputMonitor.NameChanged += (s, e) => UpdateFields();
            inputMonitor.ProgramChanged += (s, e) => UpdateFields();
            inputMonitor.PreviewChanged += (s, e) => UpdateFields();
            this.input = input;
            this.input.AddCallback(inputMonitor);
            
            long id;
            this.input.GetInputId(out id);
            Id = (int)id;
            UpdateFields();
        }

        public void Dispose()
        {
            input.RemoveCallback(inputMonitor);
            inputMonitor = null;
            input = null;
        }

        #endregion

        public override string ToString()
        {
            return $"Input {Id} {ShortName} Program {IsProgram} Preview {IsPreview}";
        }

        void UpdateFields()
        {
            string name;
            input.GetLongName(out name);
            Name = name;

            input.GetShortName(out name);
            ShortName = name;

            int value;
            input.IsPreviewTallied(out value);
            IsPreview = value > 0;

            input.IsProgramTallied(out value);
            IsProgram = value > 0;

        }

    }
}
