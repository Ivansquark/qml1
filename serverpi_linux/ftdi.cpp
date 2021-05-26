#include "ftdi.h"

Ftdi::Ftdi(QObject* obj) :QObject(obj)  {

}

Ftdi::~Ftdi() {
    FT_Close(UserDevice0_Handle);
}

void Ftdi::initSlot() {
    qDebug()<<"Ftdi thread: "<< QThread::currentThreadId();
    emit sendText("Ftdi started");
}

void Ftdi::protocolInitSlot() {
    protocolInit();
}

void Ftdi::protocolDataInitSlot() {
    protocolDataInit();
}

void Ftdi::getRawDataSlot() {
    getRawData();
}

void Ftdi::usb_serial_init() {
    emit sendVoltageState(voltageState);
    //Определяем количство, подключенных устройств
    ftStatus = FT_CreateDeviceInfoList(&dwNumDevs);
    if (ftStatus == FT_OK) {
        if ( dwNumDevs > 0) {
            //Если подключено хотя бы одно устройство
            //Печатаем число, подключенных устройств
            qDebug()<<"Number of devices is " << dwNumDevs;
             //Выделяем нужный объем памяти для хранения нужного объема информации
            //devInfo = (FT_DEVICE_LIST_INFO_NODE*)malloc(sizeof(FT_DEVICE_LIST_INFO_NODE)*dwNumDevs);
             //Создаем структуры, описывающие каждое из устройств
            ftStatus = FT_GetDeviceInfoList(&devInfo,&dwNumDevs);
            if (ftStatus == FT_OK) {
                //Если успешно, то...
                for (uint32_t i = 0; i < dwNumDevs; i++) {
                    //...для каждого устройства извлекаем поля структуры, описывающей его
                    ftStatus_GetInfoDetail = FT_GetDeviceInfoDetail(i,
                                                                    &Flags,
                                                                    &Type,
                                                                    &ID,
                                                                    &LocId,
                                                                    SerialNumber,
                                                                    Description,
                                                                    &ftHandleTemp);
                    if (ftStatus == FT_OK) {
                        //Если успешно выполнена процедура, то печатаем серийный номер (может пригодиться потом, когда к одному ПК будет поделючено несколько
                        //D2xx устройств. Аналогично можно поступить с остальными полями.
                        //printf("  SerialNumber=%s\n",SerialNumber);
                        //Открываем устройство
                        ftStatus = FT_OpenEx(UserDevice0_SN,
                                           FT_OPEN_BY_SERIAL_NUMBER,
                                           &UserDevice0_Handle);
                        if (ftStatus == FT_OK) {
                            //Сообщаем об успешном открытии устройства
                            qDebug() << "Device which serial number " << UserDevice0_SN
                                     << " is opened. Device handle is " << UserDevice0_Handle;
                        }
                        else {
                            //Сообщаем о неудаче и выходим из программы, т.к. далее делать нечего
                            //(не можем получить дескриптор устройства, а без него никуда:(
                            qDebug() << "Can't open the device with serial number "
                                     << UserDevice0_SN << "Error # " << FT_W32_GetLastError(UserDevice0_Handle);
                            Connected = false;
                            emit connectedSignal(Connected);
                            return;
                        }
                    }
                    else {
                        //Не удалось получить значение полей структуры, описывающей устройство
                        qDebug() << "FT_GetDeviceInfoDetail failed. Error # ";//<< GetLastError();
                        Connected = false;
                        emit connectedSignal(Connected);
                        return;
                    }
                }
            }
            else {
                qDebug() << "Can't get the device info list. Error # ";//<< GetLastError();
                Connected = false;
                emit connectedSignal(Connected);
                return;
            }
        }
        else {
            //К ПК не подключено ни одного устройства
            qDebug() << "No device is connected to the PC";
            Connected = false;
            emit connectedSignal(Connected);
            return;
        }
    }
    else {
            qDebug() << "FT_CreateDeviceInfoList failed";
            Connected = false;
            emit connectedSignal(Connected);
            return;
    }
    //Настраиваем скорость передачи 9600 бод. ВНИМАНИЕ!!! скорость должна соответствовать той,
    //на которую был настроен USART модуль в микроконтроллере, иначе возможы ошибки при передаче
    if (FT_SetBaudRate(UserDevice0_Handle,1000000) == FT_OK) {}
    else {
            qDebug() << "Can't set baud rate. Error # " << FT_W32_GetLastError(UserDevice0_Handle);
            Connected = false;
            emit connectedSignal(Connected);
            return;
    }
    //Настройка передачи: слово - 8 бит, стоп-битов - 1, проверка очетности отстутствует
    if (FT_SetDataCharacteristics (UserDevice0_Handle,
                                   FT_BITS_8,
                                   FT_STOP_BITS_1,
                                   FT_PARITY_NONE) == FT_OK) {
        //qDebug() << "Device has been configured";
        //!!!!!!! set connection status flag !!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        Connected = true; //
        emit connectedSignal(Connected);
        //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        resetRequest();
    }
    else {
        qDebug() << "Can't configure device. Error # " << FT_W32_GetLastError(UserDevice0_Handle);
        Connected = false; emit connectedSignal(Connected);
        return;
    }
}

void Ftdi::protocolInit() {
    qDebug()<<"Ftdi thread: "<<QThread::currentThreadId();
    usb_serial_init();
    qDebug()<<"usb_serial_init();";
    if(Connected) {
        clearReceivedBuffer();
        sendInfoRequest();
        if(!Connected){return;}
        readInfoAnswer();
    }
    resetRequest();
}

void Ftdi::protocolDataInit() {
    QString str;
    clearReceivedBuffer();
    send_42_04_d1_Request();if(!Connected){return;}
    read_42_04_d1_Answer(); if(!Connected){return;}
    resetRequest();
    str = QString::number(UserDevice0_ReceiverBuffer[0]) + QString::number(UserDevice0_ReceiverBuffer[1]);
    clearReceivedBuffer();
    send_42_04_d2_Request();if(!Connected){return;}
    read_42_04_d2_Answer(); if(!Connected){return;}
    str += "\n" + QString::number(UserDevice0_ReceiverBuffer[0]) + QString::number(UserDevice0_ReceiverBuffer[1]);
    resetRequest();
    clearReceivedBuffer();
    send_42_04_d3_Request();if(!Connected){return;}
    read_42_04_d3_Answer(); if(!Connected){return;}
    str += "\n" + QString::number(UserDevice0_ReceiverBuffer[0]) + QString::number(UserDevice0_ReceiverBuffer[1]);
    resetRequest();
    clearReceivedBuffer();
    send_42_04_d4_Request();if(!Connected){return;}
    read_42_04_d4_Answer(); if(!Connected){return;}
    str += "\n" + QString::number(UserDevice0_ReceiverBuffer[0]) + QString::number(UserDevice0_ReceiverBuffer[1]);
    resetRequest();
    clearReceivedBuffer();
    send_42_04_d5_Request();if(!Connected){return;}
    read_42_04_d5_Answer(); if(!Connected){return;}
    str += "\n" + QString::number(UserDevice0_ReceiverBuffer[0]) + QString::number(UserDevice0_ReceiverBuffer[1]);
    resetRequest();
    clearReceivedBuffer();
    send_42_04_d6_Request();if(!Connected){return;}
    read_42_04_d6_Answer(); if(!Connected){return;}
    str += "\n" + QString::number(UserDevice0_ReceiverBuffer[0]) + QString::number(UserDevice0_ReceiverBuffer[1]);
    resetRequest();
    clearReceivedBuffer();
    send_42_04_d7_Request();if(!Connected){return;}
    read_42_04_d7_Answer(); if(!Connected){return;}
    str += "\n" + QString::number(UserDevice0_ReceiverBuffer[0]) + QString::number(UserDevice0_ReceiverBuffer[1]);
    resetRequest();
    //browser->setText(str);
    emit sendText(str);
    qDebug()<<str;
    protocolInitialized = true;
}

void Ftdi::getRawData() {
    //qDebug()<<"Ftdi thread: "<< QThread::currentThreadId();
    if(!Connected) {return;}
    clearReceivedBuffer();
    sendRawDataRequest();if(!Connected) {return;}
    readRawDataAnswer(); if(!Connected) {return;}
    resetRequest();
    //qDebug()<<"readRawDataAnswer();";
    countAmplitude();
    countMeanAverage();
}

void Ftdi::resetRequest() {
    ftStatus = FT_ResetDevice(UserDevice0_Handle);
    if (ftStatus == FT_OK) {
       //Connected = true;
    }
    else {
       Connected = false; emit connectedSignal(Connected);
    }
}

void Ftdi::sendInfoRequest() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x19;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0x19;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else {
        qDebug() << "Can't write the data into the device. Error # " << FT_W32_GetLastError(UserDevice0_Handle);
        Connected = false; emit connectedSignal(Connected);
        return;
    }
}

void Ftdi::readInfoAnswer() {
    //First read in received buffer - device must return 78 bytes of INFO
    UserDevice0_ReadStatus = FT_Read(UserDevice0_Handle,
                                     &UserDevice0_ReceiverBuffer,
                                     78,
                                     &UserDevice0_BytesRead);
    if (UserDevice0_ReadStatus == FT_OK) {
        //browser->setText(UserDevice0_ReceiverBuffer);
        emit sendText(UserDevice0_ReceiverBuffer);
        clearReceivedBuffer();
    }
    else{
            //browser->setText("Can't read the data. Error #" + QString::number(FT_W32_GetLastError(UserDevice0_Handle)));
            Connected = false; emit connectedSignal(Connected);
            return;
    }
    qDebug()<<"ReadInfoAnswer()";
}

void Ftdi::sendRawDataRequest() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x20;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0xd0;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else {
            qDebug() << "Can't write the data into the device. Error # " << FT_W32_GetLastError(UserDevice0_Handle);
            Connected = false; emit connectedSignal(Connected);
            return;
    }
}

void Ftdi::readRawDataAnswer() {
    //First read in received buffer - device must return 16386 bytes of RawData
    UserDevice0_ReadStatus = FT_Read(UserDevice0_Handle,
                                     &UserDevice0_ReceiverBuffer,
                                     16386,
                                     &UserDevice0_BytesRead);
    if (UserDevice0_ReadStatus == FT_OK) {  }
    else{
        //browser->setText("Can't read the data. Error #" + QString::number(FT_W32_GetLastError(UserDevice0_Handle)));
        Connected = false; emit connectedSignal(Connected);
        return;
    }
}

void Ftdi::send_42_04_d1_Request() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x04;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0xd1;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else { Connected = false; emit connectedSignal(Connected); return; }
}

void Ftdi::read_42_04_d1_Answer() {
    UserDevice0_ReadStatus = FT_Read(UserDevice0_Handle,
                                     &UserDevice0_ReceiverBuffer,
                                     2050,
                                     &UserDevice0_BytesRead);
    if (UserDevice0_ReadStatus == FT_OK) { }
    else{
        //browser->setText("Can't read the data d1. Error #" + QString::number(FT_W32_GetLastError(UserDevice0_Handle)));
        Connected = false; emit connectedSignal(Connected);
        return;
    }
}

void Ftdi::send_42_04_d2_Request() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x04;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0xd2;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else { Connected = false; emit connectedSignal(Connected); return; }
}

void Ftdi::read_42_04_d2_Answer() {
    UserDevice0_ReadStatus = FT_Read(UserDevice0_Handle,
                                     &UserDevice0_ReceiverBuffer,
                                     2050,
                                     &UserDevice0_BytesRead);
    if (UserDevice0_ReadStatus == FT_OK) {
        countOffset();
    }
    else{
        //browser->setText("Can't read the data d2. Error #" + QString::number(FT_W32_GetLastError(UserDevice0_Handle)));
        Connected = false; emit connectedSignal(Connected);
        return;
    }
}

void Ftdi::send_42_04_d3_Request() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x04;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0xd3;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else { Connected = false; emit connectedSignal(Connected); return; }
}

void Ftdi::read_42_04_d3_Answer() {
    UserDevice0_ReadStatus = FT_Read(UserDevice0_Handle,
                                     &UserDevice0_ReceiverBuffer,
                                     2050,
                                     &UserDevice0_BytesRead);
    if (UserDevice0_ReadStatus == FT_OK) {
        countCoef();
    }
    else{
        //browser->setText("Can't read the data d2. Error #" + QString::number(FT_W32_GetLastError(UserDevice0_Handle)));
        Connected = false; emit connectedSignal(Connected);
        return;
    }
}

void Ftdi::send_42_04_d4_Request() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x04;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0xd4;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else { Connected = false; emit connectedSignal(Connected); return; }
}

void Ftdi::read_42_04_d4_Answer() {
    read_42_04_d1_Answer();
}

void Ftdi::send_42_04_d5_Request() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x04;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0xd5;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else { Connected = false; emit connectedSignal(Connected); return; }
}

void Ftdi::read_42_04_d5_Answer() {
    read_42_04_d1_Answer();
}

void Ftdi::send_42_04_d6_Request() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x04;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0xd6;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else { Connected = false; emit connectedSignal(Connected); return; }
}

void Ftdi::read_42_04_d6_Answer() {
    read_42_04_d1_Answer();
}

void Ftdi::send_42_04_d7_Request() {
    UserDevice0_TransmitterBuffer[0]=0x42;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[0], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[1]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[1], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[2]=0x00;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[2], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[3]=0x04;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[3], 1, &UserDevice0_BytesWritten);
    UserDevice0_TransmitterBuffer[4]=0xd7;
    UserDevice0_WriteStatus = FT_Write(UserDevice0_Handle, &UserDevice0_TransmitterBuffer[4], 1, &UserDevice0_BytesWritten);
    //Успешно ли отправлен байт?
    if (UserDevice0_WriteStatus == FT_OK) {}
    else { Connected = false; emit connectedSignal(Connected); return; }
}

void Ftdi::read_42_04_d7_Answer() {
    read_42_04_d1_Answer();
}

void Ftdi::countOffset() {
    if(UserDevice0_ReceiverBuffer[1] == 0x06) {
        uint16_t temp = 0;
        int64_t sum = 0;
        uint16_t count = 0;
        for(int i = 2; i<2050; i+=2) {
            temp = (((uint8_t)UserDevice0_ReceiverBuffer[i+1]<<8) | ((uint8_t)UserDevice0_ReceiverBuffer[i]));
            sum+=*(reinterpret_cast<int16_t*>(&temp));
            count+=1;
        }
        int32_t offset = sum/count;
        Offset = QString::number(offset);
        qDebug()<<"offset count " << count << "Sum "<< sum;
    }
}

void Ftdi::countCoef() {
    if(UserDevice0_ReceiverBuffer[1] == 0x06) {
        int16_t temp = 0;
        int64_t sum = 0;
        uint16_t count = 0;
        for(int i = 2; i<2050; i+=2) {
            temp = (((int8_t)UserDevice0_ReceiverBuffer[i+1]<<8) + (int8_t)(UserDevice0_ReceiverBuffer[i]));
            sum+=*(reinterpret_cast<int16_t*>(&temp));
            count+=1;
            //qDebug()<<"temp="<< temp << "sum="<<sum<<(UserDevice0_ReceiverBuffer[i+1])<<(UserDevice0_ReceiverBuffer[i]);
        }
        uint32_t coef = sum/count;
        Coef = QString::number(coef);
    }
}

void Ftdi::countAmplitude() {
    if(UserDevice0_ReceiverBuffer[1] == 0x06) {
        int16_t max = 0;
        int16_t min = 0;
        int32_t max_sum = 0; double max_average = 0;
        int32_t min_sum = 0; double min_average = 0;
        uint16_t count_min = 0; uint16_t count_max = 0;
        //! find max value in array
        for(int j = 0; j<20; j++) {
            for(int i = 0; i<800; i+=2) {
                uint16_t temp_1 = (((uint8_t)UserDevice0_ReceiverBuffer[2+i+j*800+1]<<8) |
                                    ((uint8_t)UserDevice0_ReceiverBuffer[2+i+j*800]));
                int16_t temp = *(reinterpret_cast<int16_t*>(&temp_1));
                if(temp > max) {
                    max = temp;
                    count_max = i+j;
                } if(temp < min) {
                    min = temp;
                    count_min = i+j;
                }
            }
            max_sum += max;
            min_sum += min;
            max = 0; min = 0;
        }
        max_average = max_sum/20.F;
        min_average = min_sum/20.F;
        double maxAmplitude = 0;
        if(abs(max_average) > abs(min_average)) {
            maxAmplitude = max_average;
        } else {
            maxAmplitude = min_average;
        }
        //! condition of determintion alternate or direct current
        voltageStateSet = voltageState;
        if(max_average <= 1 && min_average < -30) {
            voltageState = VoltageState::Direct;
        } else {
            voltageState = VoltageState::Alternating;
        }
        if(voltageState != voltageStateSet) {
            emit sendVoltageState(voltageState);
        }
        Amplitude = maxAmplitude;
        Amplitude_str = QString::number(abs(Amplitude),'g',4);
        //qDebug()<<" AMPLITUDE: "<< Amplitude_str << "MIN=" << min_average
        //        << "count_min=" << count_min << "count_max=" <<count_max
        //        <<"minA_low= " << Qt::hex << (uint8_t)UserDevice0_ReceiverBuffer[count_min]//QString::number(UserDevice0_ReceiverBuffer[count_min],16)
        //        <<"minA_hi= " << Qt::hex << (uint8_t)UserDevice0_ReceiverBuffer[count_min+1];//QString::number(UserDevice0_ReceiverBuffer[count_min+1],16);
    }
}

void Ftdi::countMeanAverage() {
    if(UserDevice0_ReceiverBuffer[1] == 0x06) {
        //uint16_t temp = 0;
        double sum = 0; double sum_sum = 0; double sum_average = 0;
        uint16_t count = 0;
        for(int j = 0; j<80; j++) {
            for(int i = 0; i<200; i+=2) {
                uint16_t temp_1 = (((uint8_t)UserDevice0_ReceiverBuffer[2+i+j*200+1]<<8) |
                                   ((uint8_t)UserDevice0_ReceiverBuffer[2+i+j*200]));
                int16_t temp = *(reinterpret_cast<int16_t*>(&temp_1));
                sum += (temp*temp);
                //sum+=abs(temp);
                //sum+=*(reinterpret_cast<int16_t*>(&temp));
                count++;
            }
            double temp2 = sqrt(sum/count);
            sum_sum += abs(temp2);
            sum=0;
            count = 0;
        }
        sum_average = sum_sum/80.F;
        Mean = (float)(sum_average);
        Mean_str = QString::number(Mean,'g',4);
        qDebug()<<"mean= " << Mean << "MEAN: ";
    }
}
