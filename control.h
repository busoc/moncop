#ifndef CONTROL_H
#define CONTROL_H

#include <QString>
#include <QDateTime>
#include <QHostAddress>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <QDataStream>
#include <QException>

class GroupException: public QException {
public:
    GroupException(QString msg): msg(msg) {}

    void raise() const override
    {
        throw *this;
    }

    GroupException *clone() const override
    {
        return new GroupException(*this);
    }

    QString errorSring() const {
        return msg;
    }

private:
    QString msg;
};

struct Coze {
    quint64 size;
    quint64 count;
    quint32 prev;
    quint32 curr;
    QDateTime starts;
    QDateTime ends;

    quint32 lost() {
        return 0;
    }

    void reset() {
        size = 0;
        count = 0;
        starts = QDateTime::currentDateTime();
        ends = starts;
    }

    void update(quint64 z) {
        count++;
        size += z;
        ends = QDateTime::currentDateTime();
    }

    double rate() {
        return 0;
    }
};

struct Addr {
    quint32 Host;
    quint16 Port;

    QString toString() const {
        auto h = QHostAddress(Host).toString();
        return QString("%1:%2").arg(h).arg(Port);
    }

    QString Hostname() const {
        return QHostAddress(Host).toString();
    }

    bool operator==(const Addr &other) const {
        return Host == other.Host && Port == other.Port;
    }
};

Q_DECLARE_METATYPE(Addr)

struct Transfer {
    qint64 File;
    qint64 Size;
    Addr Src;
    Addr Dst;
    QDateTime Starts;
    QDateTime Ends;

    double Rate()
    {
        auto secs = Starts.secsTo(Ends);
        if (!secs) {
            auto msecs = Starts.msecsTo(Ends);
            return msecs > 0 ? Size*msecs : Size;
        }
        return Size/secs;
    }

    void Update(qint64 size)
    {
        if (size == 0) {

        }
        File++;
        Size += size;
        Ends = QDateTime::currentDateTime();
    }

};

class FileInfo {
public:
    QString File;
    qint64 Size;
    qint64 Curr;
    qint64 Written;
    QDateTime Starts;
    QDateTime Ends;
    Addr Src;
    Addr Dst;

    FileInfo()
    {
        Starts = QDateTime::currentDateTime();
        Ends = QDateTime::currentDateTime();
        File = "";
        Size = 0;
        Curr = 0;
        Written = 0;
    }

    FileInfo(const FileInfo &other)
    {
        File = other.File;
        Size = other.Size;
        Curr = other.Curr;
        Written = other.Written;
        Starts = other.Starts;
        Ends = other.Ends;
        Src = other.Src;
        Dst = other.Dst;
    }

    ~FileInfo() {}

    void Update(qint64 size)
    {
        if (size < Curr) {
            return ;
        }
        Written = size - Curr;
        Curr= size;
        Ends = QDateTime::currentDateTime();
    }

    qint64 Elapsed() const
    {
        return Starts.secsTo(Ends);
    }

    double Percent() const
    {
        if (!Size) {
            return 0;
        }
        return double(Curr) / double(Size);
    }

    bool Complete() const
    {
        return Curr == Size;
    }

    double Rate() const
    {
        double msecs = Starts.msecsTo(Ends);
        if (msecs < 1000) {
            return msecs == 0 ? Curr * 1000 : Curr * (1000/msecs);
        }
        double secs = Starts.secsTo(Ends);
        if (!secs) {
            return Size;
        }
        return Curr/secs;
    }

    bool operator==(const FileInfo &other)
    {
        if (Complete()) {
            return false;
        }
        if (Src == other.Src && Dst == other.Dst) {
            return File == other.File && Size == other.Size;
        }
        return false;
    }
};

Q_DECLARE_METATYPE(FileInfo)

class GroupInfo: public QObject {
    Q_OBJECT
public:

    GroupInfo(QString host, quint16 port, QString nic) {
        socket = new QUdpSocket;
        auto ok = socket->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ShareAddress);
        if (!ok) {
            throw GroupException(socket->errorString());
        }
        if (!nic.isEmpty()) {

        }
        group = QHostAddress(host);
        connect(socket, &QUdpSocket::readyRead, this, &GroupInfo::datagramReceived);
    }

    ~GroupInfo() {
        if (!socket) {
            return ;
        }
        socket->close();
        delete socket;
    }

    quint16 Port() const {
        return socket->localPort();
    }

    QHostAddress Host() const {
        return group;
    }

    bool Enabled() const {
        return joined;
    }

    bool Join() {
        if (Enabled()) {
            return true;
        }
        joined = socket->joinMulticastGroup(group);
        if (joined) {
            coze.reset();
        }
        return joined;
    }

    bool Leave() {
        if (!Enabled()) {
            return true;
        }
        auto ok = socket->leaveMulticastGroup(group);
        if (ok) {
            joined = false;
            coze.reset();
        }
        return ok;
    }

    Coze status() {
        return coze;
    }

signals:
    void recv(FileInfo &info);

private slots:
    void datagramReceived() {
        while(socket->hasPendingDatagrams()) {
            coze.update(socket->pendingDatagramSize());
            process(socket->receiveDatagram());
        }
    }

private:
    QUdpSocket *socket;
    QHostAddress group;
    bool joined = false;
    Coze coze;


    void process(QNetworkDatagram dg)
    {
        FileInfo info;
        QDataStream in(dg.data());
        in.setByteOrder(QDataStream::LittleEndian);
        char file[256];
        in.readRawData(file, 256);
        info.File = QString(file);
        in >> info.Size >> info.Curr >> info.Written;

        in.setByteOrder(QDataStream::BigEndian);
        in >> info.Src.Host;
        in.setByteOrder(QDataStream::LittleEndian);
        in >> info.Src.Port;

        in.setByteOrder(QDataStream::BigEndian);
        in >> info.Dst.Host;
        in.setByteOrder(QDataStream::LittleEndian);
        in >> info.Dst.Port;

        if (info.Size == 0) {
            return ;
        }
        emit recv(info);
    }
};


QString formatSize(double size);
QString formatRate(double rate);

#endif // CONTROL_H
