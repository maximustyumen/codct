#include "SerialClass.h"

#ifdef WIN32

Serial::Serial(const char *portName, int portBaudRate)
{
    //We're not yet connected
    this->connected = false;

    //Устанавливаем скорость порта
    DWORD BaudRate;
    switch (portBaudRate)
    {
    case 110:       BaudRate=CBR_110;break;
    case 300:       BaudRate=CBR_300;break;
    case 600:       BaudRate=CBR_600;break;
    case 1200:      BaudRate=CBR_1200;break;
    case 2400:      BaudRate=CBR_2400;break;
    case 4800:      BaudRate=CBR_4800;break;
    case 9600:      BaudRate=CBR_9600;break;
    case 14400:     BaudRate=CBR_14400;break;
    case 19200:     BaudRate=CBR_19200;break;
    case 38400:     BaudRate=CBR_38400;break;
    case 56000:     BaudRate=CBR_56000;break;
    case 57600:     BaudRate=CBR_57600;break;
    case 115200:    BaudRate=CBR_115200;break;
    case 128000:    BaudRate=CBR_128000;break;
    case 256000:    BaudRate=CBR_256000;break;
    default:
                    printf("Serial: Not standart portBaudRate. Use CBR_115200 !!!");
                    BaudRate=CBR_115200;
                    break;
    }

    //Try to connect to the given port throuh CreateFile
    this->hSerial = CreateFileA(portName,
            GENERIC_READ | GENERIC_WRITE,
            0,
            NULL,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            NULL);

    //Check if the connection was successfull
    if(this->hSerial==INVALID_HANDLE_VALUE)
    {
        //If not success full display an Error
        if(GetLastError()==ERROR_FILE_NOT_FOUND){

            //Print Error if neccessary
            printf("ERROR: Handle was not attached. Reason: %s not available.\n", portName);

        }
        else
        {
            printf("ERROR!!!");
        }
    }
    else
    {
        //If connected we try to set the comm parameters
        DCB dcbSerialParams = {0};

        //Try to get the current
        if (!GetCommState(this->hSerial, &dcbSerialParams))
        {
            //If impossible, show an error
            printf("failed to get current serial parameters!");
        }
        else
        {
            //Define serial connection parameters for the arduino board
            dcbSerialParams.BaudRate=BaudRate;
            dcbSerialParams.ByteSize=8;
            dcbSerialParams.StopBits=ONESTOPBIT;
            dcbSerialParams.Parity=NOPARITY;
			dcbSerialParams.fDtrControl = 1;
			dcbSerialParams.EofChar = 4;
			dcbSerialParams.EvtChar = 10;			

             //Set the parameters and check for their proper application
             if(!SetCommState(hSerial, &dcbSerialParams))
             {
                printf("ALERT: Could not set Serial Port parameters");
             }
             else
             {
				COMMTIMEOUTS ComTimeouts;
				ComTimeouts.ReadIntervalTimeout = MAXDWORD;
				ComTimeouts.ReadTotalTimeoutConstant = 0;
				ComTimeouts.ReadTotalTimeoutMultiplier = 0;
				ComTimeouts.WriteTotalTimeoutConstant = 0;
				ComTimeouts.WriteTotalTimeoutMultiplier = 0;
				if(!SetCommTimeouts(hSerial, &ComTimeouts))
				{
					printf("Cann't SetCommTimeouts!\n");
					//return 0;
				}

                 //If everything went fine we're connected
                 this->connected = true;
                 //ждём пока Arduino перезагрузится
                 //We wait 2s as the arduino board will be reseting
                 Sleep(ARDUINO_WAIT_TIME);
             }
        }
    }

}

Serial::~Serial()
{
    //Check if we are connected before trying to disconnect
    if(this->connected)
    {
        //We're no longer connected
        this->connected = false;
        //Close the serial handler
        CloseHandle(this->hSerial);
    }
}

int Serial::ReadData(char *buffer, unsigned int nbChar)
{
    //Number of bytes we'll have read
    DWORD bytesRead;
    //Number of bytes we'll really ask to read
    unsigned int toRead;

    //Use the ClearCommError function to get status info on the Serial port
    ClearCommError(this->hSerial, &this->errors, &this->status);

    //Check if there is something to read
    if(this->status.cbInQue>0)
    {
        //If there is we check if there is enough data to read the required number
        //of characters, if not we'll read only the available characters to prevent
        //locking of the application.
        if(this->status.cbInQue>nbChar)
        {
            toRead = nbChar;
        }
        else
        {
            toRead = this->status.cbInQue;
        }

        //Try to read the require number of chars, and return the number of read bytes on success
        if(ReadFile(this->hSerial, buffer, toRead, &bytesRead, NULL) && bytesRead != 0)
        {
            return bytesRead;
        }

    }

    //If nothing has been read, or that an error was detected return -1
    return -1;

}


bool Serial::WriteData(char *buffer, unsigned int nbChar)
{
    DWORD bytesSend;

    //Try to write the buffer on the Serial port
    if(!WriteFile(this->hSerial, (void *)buffer, nbChar, &bytesSend, 0))
    {
        //In case it don't work get comm error and return false
        ClearCommError(this->hSerial, &this->errors, &this->status);

        return false;
    }
    else
        return true;
}

bool Serial::IsConnected()
{
    //Simply return the connection status
    return this->connected;
}


#else

Serial::Serial(const char *serialport, int portBaudRate)
{
	connected = false;
	struct termios toptions;

    //Устанавливаем скорость порта
    speed_t brate;
    switch (portBaudRate)
    {
    case 0:         brate=B0;break;
    case 50:        brate=B50;break;
    case 75:        brate=B75;break;
    case 110:       brate=B110;break;
    case 134:       brate=B134;break;
    case 150:       brate=B150;break;
    case 200:       brate=B200;break;
    case 300:       brate=B300;break;
    case 600:       brate=B600;break;
    case 1200:      brate=B1200;break;
    case 1800:      brate=B1800;break;
    case 2400:      brate=B2400;break;
    case 4800:      brate=B4800;break;
    case 9600:      brate=B9600;break;
    case 19200:     brate=B19200;break;
    case 38400:     brate=B38400;break;
    case 57600:     brate=B57600;break;
    case 115200:    brate=B115200;break;
    case 230400:    brate=B230400;break;
    default:
                    printf("Serial: Not standart portBaudRate. Use B115200 !!!");
                    brate=B115200;
                    break;
    }
	

	fd = open(serialport, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)  
	{
		printf("init_serialport: Unable to open port ");
		return;
	}

	if (tcgetattr(fd, &toptions) < 0) 
	{
		printf("init_serialport: Couldn't get term attributes");
		return;
	}


	cfsetispeed(&toptions, brate);
	cfsetospeed(&toptions, brate);

	// 8N1
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	// no flow control
	toptions.c_cflag &= ~CRTSCTS;

	toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
	toptions.c_oflag &= ~OPOST; // make raw

	// see: http://unixwiz.net/techtips/termios-vmin-vtime.html
	toptions.c_cc[VMIN]  = 0;
	toptions.c_cc[VTIME] = 0;

	if( tcsetattr(fd, TCSANOW, &toptions) < 0) 
	{
		printf("init_serialport: Couldn't set term attributes");
		return;
	}
	connected = true;

    //ждём пока Arduino перезагрузится
    //We wait 2s as the arduino board will be reseting
    //0.5 second use usleep(500000);
    usleep(ARDUINO_WAIT_TIME*1000);

}

Serial::~Serial()
{
	if (connected==true) close (fd);
}

bool Serial::IsConnected()
{
	return connected;
}
#endif
