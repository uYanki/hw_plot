#include "getlocalip.h"

QStringList getlocalip() {
    static QStringList ips;
    if (ips.count() == 0) {
        ips << "127.0.0.1";

        QList<QNetworkInterface> netInterfaces = QNetworkInterface::allInterfaces();
        foreach (const QNetworkInterface& netInterface, netInterfaces) {
            // 过滤虚拟网卡
            QString humanReadableName = netInterface.humanReadableName().toLower();
            if (humanReadableName.startsWith("vmware network adapter") /*虚拟机*/ ||
                humanReadableName.startsWith("npcap loopback adapter")) /*抓包工具*/ {
                continue;
            }

            //当前网络接口
            if (netInterface.flags() ==
                (QNetworkInterface::IsUp |
                 QNetworkInterface::IsRunning |
                 QNetworkInterface::CanBroadcast |
                 QNetworkInterface::CanMulticast)) {
                QList<QNetworkAddressEntry> addrs = netInterface.addressEntries();
                foreach (QNetworkAddressEntry addr, addrs) {
                    if (addr.ip().protocol() == QAbstractSocket::IPv4Protocol) {  //只取IPV4
                        ips << addr.ip().toString();
                    }
                }
            }
        }
    }
    return ips;
}
