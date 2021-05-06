#include "soprocessor.h"

Soprocessor::Soprocessor(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout* layer1 = new QVBoxLayout;
    QHBoxLayout* layer2 = new QHBoxLayout;
    QHBoxLayout* layer3 = new QHBoxLayout;
    QHBoxLayout* layer4 = new QHBoxLayout;

    QLabel *label_port = new QLabel;
    QLabel *label_ssid = new QLabel ("SSID");
    QLabel *label_pass = new QLabel ("Password");
    QLabel *label_curport = new QLabel ("Current port:");
    QLabel *label_status = new QLabel ("NOT CONNECTED");
    QLabel *label_curstatus = new QLabel ("Status:");

    QTextEdit *field_answer = new QTextEdit;
    field_answer->setReadOnly(true);
    field_answer->setFixedWidth(200);

    field_ssid = new QLineEdit;
    field_pass = new QLineEdit;
    QPushButton* button_connect = new QPushButton("Connect");
    QPushButton* button_send = new QPushButton("Send");
    button_send->setEnabled(false);

    QComboBox * cbo = new QComboBox;
    cbo->setEditable(false);

    layer2->addWidget(label_curport);
    layer2->addWidget(label_port);

    layer3->addWidget(label_curstatus);
    layer3->addWidget(label_status);

    layer1->addWidget(cbo);
    layer1->addLayout(layer2);
    layer1->addLayout(layer3);
    layer1->addWidget(button_connect);
    layer1->addWidget(label_ssid);
    layer1->addWidget(field_ssid);
    layer1->addWidget(label_pass);
    layer1->addWidget(field_pass);
    layer1->addWidget(button_send);

    layer4->addLayout(layer1);
    layer4->addWidget(field_answer);
    setLayout(layer4);

    connect(cbo, SIGNAL(currentTextChanged(QString)), label_port, SLOT(setText(QString)));
    connect(cbo, SIGNAL(currentTextChanged(QString)), SLOT(port_transfer(QString)));
    connect(button_connect, SIGNAL(clicked()), SLOT(port_connect()));
    connect(this, SIGNAL(connected(bool)),button_connect, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(connected(bool)),cbo, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(connected_invert(bool)), button_send, SLOT(setEnabled(bool)));
    connect(this, SIGNAL(connected_string(QString)), label_status, SLOT(setText(QString)));
    connect(button_send, SIGNAL(clicked()),SLOT(line_ejector()));
    connect(this, SIGNAL(transp_ssidpasssended(QString)), field_answer, SLOT(append(QString)));
    connect(this, SIGNAL(transp_ssidpass(QString)), SLOT(port_data_send(QString)));
    connect(this, SIGNAL(transp_res_data(QString)), field_answer, SLOT(append(QString)));

    auto serialPortInfos = QSerialPortInfo::availablePorts();
    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {
        serialPortInfo.portName();
        cbo->addItem(serialPortInfo.portName());
    }

   usart->setBaudRate(QSerialPort::Baud9600);

}

void Soprocessor::port_transfer(QString portname){
    m_portname = portname;
}

void Soprocessor::port_connect(){
        usart->setPortName(m_portname);
    if (!usart->open(QIODevice::ReadWrite)) {
        QMessageBox::warning(this, "Error", "Unable to connect");
        return;
    }
    else{
        QMessageBox::warning(this, "Done", "Connected!");
        emit connected(false);
        emit connected_invert(true);
        emit connected_string("CONNECTED");

    }
}

void Soprocessor::port_data_send(QString data_send){
QByteArray data_send_array = data_send.toUtf8();
usart->write(data_send_array);
usart->waitForBytesWritten();
QByteArray data_array;
while (usart->waitForReadyRead(200)) {
      data_array.append(usart->readAll());
  }

QString reseived_data = "Answer: ";
QString answer = QString(data_array);
reseived_data.append(answer);

//QMessageBox::warning(this, "Done", data_array);
emit transp_res_data(reseived_data);
QMessageBox::warning(this, "Done", "Sended");
}

void Soprocessor::line_ejector(){
    m_data_ssid = field_ssid->text();
    m_data_pass = field_pass->text();

    if ((field_ssid->text() == "")||(field_pass->text() == "")){
    QMessageBox::warning(this, "Error", "Nothing to send");
    return;
    }
    QString data_whatsend = "Sended: " + m_data_ssid + "," + m_data_pass;
    QString data_send = m_data_ssid + "," + m_data_pass;
    emit transp_ssidpass(data_send);
    emit transp_ssidpasssended(data_whatsend);
}

Soprocessor::~Soprocessor(){

}



