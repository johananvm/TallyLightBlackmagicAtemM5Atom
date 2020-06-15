using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Atem.Switcher
{


    public delegate void Test(string by);

    class Program
    {

        static void Main(string[] args)
        {
            var upd = new UdpSender("224.100.0.1", 1052); // Send updates over UDP channel
            var atem = new AtemSwitcher("192.168.10.240"); // Set the ip-address of your Blackmagic Atem switch
            atem.Connect();
            Console.WriteLine($"Connected {atem.Name}");
            foreach (var item in atem.Inputs)
            {
                item.PropertyChanged += (s, e) => 
                {
                    var data = atem.Inputs.Where(x => x.Id < 127 && x.IsPreview).Select(x => (byte)x.Id).ToList();
                    data.AddRange(atem.Inputs.Where(x => x.Id < 127 && x.IsProgram).Select(x => (byte)((byte)x.Id | 0x80)));
                    upd.SendAll(data.ToArray());
                };
            }

            Console.ReadLine();
        }


    }
}

