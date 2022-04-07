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
        CHECHING,
        RESPONSE,
    }
    class Program
    {
        static States? states = null;
        static int[] bufferRx = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
        static int dataCounter = 0;
        static int[] bufferTx = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
        static bool isCheksumCorrect = false;
        static bool Error;


        static void Main(string[] args)
        {
            states = States.INIT;
            SerialPort _serialPort = new SerialPort();
            _serialPort.PortName = "COM3";
            _serialPort.BaudRate = 115200;
            _serialPort.DtrEnable = true;
            _serialPort.Open();

            //Para el cheksum
            byte[] data = { 0x73 };
            _serialPort.Write(data, 0, 1);
            byte[] bufferRead = new byte[17];


            switch (states)
            {
                case States.INIT:

                   

                    states = States.WAITING;
                    break;

                case States.WAITING:

                    switch (Console.ReadKey(true).Key)
                    {
                        case ConsoleKey.R: //Cuando se presiona R (CORRECTO)

                            _serialPort.WriteLine("0x2A");
                            Error = false;
                            states = States.READ;
                            break;
                        case ConsoleKey.E: //Cuando se presiona E (INCORRECTO)
                            _serialPort.WriteLine("0x2A");
                            Error = true;
                            states = States.READ;
                            break;
                    }

                    break;

                case States.READ:

                    //recibe los datos 
                    if (_serialPort.BytesToRead >= 17)
                    {
                        _serialPort.Read(bufferRead, 0, 17);

                        for (int i = 0; i < 17; i++)
                        {
                            Console.Write(bufferRead[i].ToString("X2") + " ");
                        }

                        //posiblemente esto no sea necesario
                        Console.WriteLine();
                        Console.WriteLine(System.BitConverter.ToSingle(bufferRead, 0));
                        Console.ReadKey();
                        _serialPort.Write(data, 0, 1);

                    }

                    states = States.CHECHING;
                    break;
                case States.CHECHING:

                    while (dataCounter < 18)
                    {
                         //pendiente de revisión

                        bufferRx[dataCounter] = bufferRead[dataCounter];
                        dataCounter++;

                        // is the packet completed?
                        if (bufferRx[0] == dataCounter - 1)
                        {

                            // Check received data
                            int calcChecksum = 0;
                            for (int i = 1; i <= dataCounter - 1; i++)
                            {
                                calcChecksum = calcChecksum ^ bufferRx[i - 1];
                            }
                            if (calcChecksum == bufferRx[dataCounter - 1])
                            {
                                bufferTx[0] = dataCounter - 3; //Length
                                calcChecksum = bufferTx[0];

                                // Calculate Tx checksum
                                for (int j = 4; j <= dataCounter - 1; j++)
                                {
                                    bufferTx[j - 3] = bufferRx[j - 1];
                                    calcChecksum = calcChecksum ^ bufferRx[j - 1];
                                }

                                bufferTx[dataCounter - 3] = calcChecksum;
                                isCheksumCorrect = true;
                                //Serial.write(bufferTx, dataCounter - 2);

                                states = States.RESPONSE;
                            }
                            else
                            {
                                //Si detecta que algo falla
                                _serialPort.WriteLine("0xB0");
                                dataCounter = 0;

                                states = States.CHECHING;
                            }
                        }
                    }


                    break;

                case States.RESPONSE:
                    {

                        if (isCheksumCorrect == true)
                        {
                            if (Error == true)           // CAMBIAR CORRECT POR EL VALOR CORRECTO
                            {
                                _serialPort.WriteLine("0xB0");  //ESTA EN MODO ERROR
                                states = States.READ;
                            }
                            else
                            {
                                _serialPort.WriteLine("0xE3"); //ESTA EN MODO MELO
                            }
                        }
                        else
                        {
                            _serialPort.WriteLine("0xB0");  //ESTA EN MODO ERROR
                            states = States.READ;
                        }
                    }

                    break;
            }
        }
    }
}