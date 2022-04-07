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
        READ,
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
                    int i = 0;
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
                    break;

                case States.READ:
                    string message = _serialPort.ReadLine();
                    
                    if (message = "correct")
                    {
                        if (Error = true;)           //CAMBIAR CORRECT POR EL VALOR CORRECTO
                        {
                            while (i < 3)
                            {
                                _serialPort.WriteLine(0xB0);  //ESTA EN MODO ERROR
                                i++;
                            }
                            states = States.INIT;
                        }
                        else
                        {
                            _serialPort.WriteLine(0xE3); //ESTA EN MODO MELO
                            states = States.INIT;
                        }
                    }
                    else
                    {
                        while (i < 3)
                        {
                            _serialPort.WriteLine(0xB0);  //EL CHECKSUM ESTA MALO 
                            i++;
                        }

                        states = States.INIT;

                    }
                    break;
            }
        }
    }
}