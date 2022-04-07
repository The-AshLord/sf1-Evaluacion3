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
        RESPONSE,
    }
    class Program
    {
        static States? states = null;

        static void Main(string[] args)
        {
            states = States.INIT;

            switch (states)
            {
                case States.INIT:

                    bool Error;                                 //Esta variable es para poder induc
                    SerialPort _serialPort = new SerialPort();
                    _serialPort.PortName = "COM3";
                    _serialPort.BaudRate = 115200;
                    _serialPort.DtrEnable = true;
                    _serialPort.Open();

                    states = States.WAITING;
                    break;

                case States.WAITING:

                    switch (Console.ReadKey(true).Key)
                    {
                        case ConsoleKey.82: //Cuando se presiona R (CORRECTO)
                                
                             _serialPort.WriteLine(0x2A);
                            Error = false;
                            states = States.READ;
                            break;
                        case ConsoleKey.69: //Cuando se presiona E (INCORRECTO)
                            _serialPort.WriteLine(0x2A);
                            Error = true;
                            states = States.READ;
                            break;
                    }

                    break;                                // QUITE EL WRITE PORQUE EL WAITING TAMBIEN ESCRIBE
                //case States.WRITE:

                //    _serialPort.WriteLine(0x2A);
                //    states = States.READ;
                    break;

                case States.READ:
                    string message = _serialPort.ReadLine();
                    if (message = "correct")
                    {
                        if (Error = true;)           // CAMBIAR CORRECT POR EL VALOR CORRECTO
                        {
                            _serialPort.WriteLine(0xB0);  //ESTA EN MODO ERROR
                        }
                        else
                        {
                            _serialPort.WriteLine(0xE3); //ESTA EN MODO MELO
                        }
                    }
                    else
                    {
                        _serialPort.WriteLine(0xB0);  //ESTA EN MODO ERROR
                    }

                    states = States.RESPONSE;
                    break;

                case States.RESPONSE:
                    

                    break;
            }
        }
    }
}