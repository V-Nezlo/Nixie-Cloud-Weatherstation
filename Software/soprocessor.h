#pragma once

#include <QApplication>
#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QComboBox>
#include <QList>
#include <QLabel>
#include <QLineEdit>
#include <QTextEdit>
#include <QPushButton>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMessageBox>
#include <QByteArray>
#include <QIcon>

class Soprocessor : public QWidget
{
    Q_OBJECT

public:
    Soprocessor(QWidget *parent = nullptr);
    ~Soprocessor();
    QSerialPort* usart = new QSerialPort;
    QLineEdit *field_ssid;
    QLineEdit *field_pass;

public slots:
    void port_transfer(QString portname);
    void port_connect();
    void port_data_send(QString data_send);
    void line_ejector();
signals:
    void connected(bool);
    void connected_invert(bool);
    void connected_string(QString);
    void transp_res_data(QString);
    void transp_ssidpass(QString);
    void transp_ssidpasssended(QString);

private:
    QString m_portname;
    QString m_data_ssid;
    QString m_data_pass;
};

