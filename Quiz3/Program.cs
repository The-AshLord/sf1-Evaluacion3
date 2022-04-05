using System;
using System.Collections.Generic;
using System.IO.Ports;
using System.Threading;

namespace Quiz3
{
    enum States
    {
        INIT,
        WAITING,
        WRITE,
        READ,
    }
    class Program
    {
        static States? states = null;

        static void Main(string[] args)
        {


            switch (states)
            {

                case States.INIT:

                    SerialPort _serialPort = new SerialPort();
                    _serialPort.PortName = "COM3";
                    _serialPort.BaudRate = 115200;
                    _serialPort.DtrEnable = true;
                    _serialPort.Open();

                    break;

                case States.WRITE:

                    Serial.write(0x2A);
                    break;

                case States.WAITING:

                    break;



                case States.READ:

                    break;
            }
        }
    }
}