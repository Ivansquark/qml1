#include "backend.h"

backend::backend(QObject* parent): QObject(parent) {
}

backend::~backend() {
    delete IPs;
}

void backend::initSlot() {
    qDebug()<<"backend thread: "<< QThread::currentThreadId();
    emit sendText("backend: started");
    //! ______________create server_______________
    server = new QTcpServer(this);
    connect(server,&QTcpServer::newConnection, this, &backend::newConnection);
    if(!server->listen(QHostAddress::Any, Port)) {
        server->close();
        emit sendText("backend: Server unable to create");
    } else {
        emit sendText("backend: Server was created at port: " + QString::number(Port));
        IPs = new QString;
    }
    QList<QHostAddress> listIP = QNetworkInterface::allAddresses();
    qDebug()<<"server->serverAddress()= "<<server->serverAddress();
    for(auto i : listIP) {
        if(!i.isLoopback()) {
            if(i.protocol() == QAbstractSocket::IPv4Protocol) {
                *IPs += i.toString() + "|";
            }
        } else {
            sendText("backend: loopback");
        }
    }
    qDebug() << "IPs: " << *IPs;
    emit sendText("backend: IPs- " + *IPs);
}

void backend::newConnection() {
    client = server->nextPendingConnection();
    connect(client, &QTcpSocket::disconnected, [&]{
        sendText("backend: client disconnected");
        emit clientDisconnectedSignal();
        client->deleteLater();
    });
    connect(client, &QTcpSocket::readyRead, this, &backend::readClient);
    emit clientConnectedSignal();
    sendText("backend: connected");
}

void backend::readClient() {
    //! receiving data: ("mA"), (uint8_t AmperageLimit), (uint8_t AmperageState),
    qDebug()<<"readClient"<<QThread::currentThread();
    QTcpSocket* clientSocket = static_cast<QTcpSocket*>(QObject::sender());
    QDataStream in(clientSocket);
    in.setVersion(QDataStream::Qt_5_11);
    QByteArray receivedArr;
    qint64 bytes = clientSocket->bytesAvailable();
    if(bytes) {
        uint8_t temp;
        for(int i=0; i<4; i++) {
            in >> temp;
            receivedArr.append(temp);
        }
        //qDebug()<<"bytes="<<bytes<<receivedArr;
        if(receivedArr[0]=='m' && receivedArr[1]=='A') {
            AmperageState as; AmperageLimit al;
            switch(receivedArr[2]) {
                case 0: al = AmperageLimit::mA5; break;
                case 1: al = AmperageLimit::mA10; break;
                case 2: al = AmperageLimit::mA20; break;
                case 3: al = AmperageLimit::mA50; break;
                default : al = AmperageLimit::mA5; break;
            }
            switch(receivedArr[3]) {
                case 0: as = AmperageState::Alternating; break;
                case 1: as = AmperageState::Direct; break;
                default : as = AmperageState::Alternating; break;
            }
            emit sendAmperageStateToScreen(as, al);
        }
    }
}

void backend::sendToClient(QTcpSocket &socket) {
    //! sending data: ("amperage"), (float A), (uint8_t AmperageState),
    //!               ("voltage"), (float Amp_max), (float Mean), (uint8_t VoltageState)
    if(socket.state() == QTcpSocket::ConnectedState) {
        QByteArray sendBytes;
        QByteArray temp; temp.clear();
        QDataStream out(&sendBytes, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_5_11);
        //uint8_t* arr = new uint8_t[30];
        QString str = "amperage"; temp = str.toLocal8Bit();
        temp.append(*((uint8_t*)(&amperage))); temp.append(*((uint8_t*)(&amperage)+1));
        temp.append(*((uint8_t*)(&amperage)+2)); temp.append(*((uint8_t*)(&amperage)+3));
        temp.append((uint8_t)amperageState);
        temp.append((uint8_t)amperageLimit);
        str = "voltage"; temp.append(str.toLocal8Bit());
        temp.append(*((uint8_t*)(&voltage_Amp))); temp.append(*((uint8_t*)(&voltage_Amp)+1));
        temp.append(*((uint8_t*)(&voltage_Amp)+2)); temp.append(*((uint8_t*)(&voltage_Amp)+3));
        temp.append(*((uint8_t*)(&voltage_Mean))); temp.append(*((uint8_t*)(&voltage_Mean)+1));
        temp.append(*((uint8_t*)(&voltage_Mean)+2)); temp.append(*((uint8_t*)(&voltage_Mean)+3));
        temp.append((uint8_t)voltageState);
        uint8_t x = 0;
        out<<x;
        sendBytes.clear();
        //! fill sending array with data
        //for(int i = 0; i< temp.size(); i++) {
        //    out << (uint8_t)temp[i];
        //}
        for(auto i: temp) {
            out << static_cast<uint8_t>(i);
        }
        out.device()->seek(0); // go to first byte
        out<<static_cast<uint8_t>(static_cast<uint8_t>(sendBytes.size()) - sizeof(uint8_t)); // replace first byte(0) with size of packet
        //qDebug()<< sendBytes;
        qint64 written = socket.write(sendBytes);
        socket.waitForBytesWritten(-1);
        written=0;
        //qDebug()<< "written: "<<written<<" bytes";
    }
}

void backend::sendSlot() {
    if(client->state() == QAbstractSocket::ConnectedState) {
        sendToClient(*client);
    }
}

void backend::receiveVoltageMeasurements(const float &vol_Amp, const float &vol_Mean) {
    voltage_Amp = vol_Amp; voltage_Mean = vol_Mean;
}

void backend::receiveVoltageState(const VoltageState &vs) {
    voltageState = vs;
}

void backend::receiveAmperageState(const AmperageState &as, const AmperageLimit &al) {
    amperageState = as; amperageLimit = al;
}

void backend::receiveAmperageMeasurements(const float &amp) {
    amperage = amp;
}
