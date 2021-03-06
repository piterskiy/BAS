#include "recordprocesscommunication.h"
#include "every_cpp.h"
#include <QXmlStreamWriter>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace BrowserAutomationStudioFramework
{
    RecordProcessCommunication::RecordProcessCommunication(QObject *parent) :
        IRecordProcessCommunication(parent)
    {
        Comunicator = 0;
        CanSend = false;
        CanSendIsChanged = false;
    }

    void RecordProcessCommunication::SendIsChanged()
    {
        if(Comunicator && CanSendIsChanged && CanSend)
        {
            CanSendIsChanged = false;
            QString WriteString;
            QXmlStreamWriter xmlWriter(&WriteString);
            xmlWriter.writeTextElement("IsChanged","");
            Comunicator->Send(WriteString);
        }
    }

    QString RecordProcessCommunication::ConvertResources(const QList<IRecordProcessCommunication::ResourceDescription>& Resources)
    {
        QJsonArray array;
        for(ResourceDescription res:Resources)
        {
            QJsonObject obj;
            obj["name"] = res.Name;
            obj["description"] = res.Description;
            array.append(obj);
        }

        QJsonDocument doc;
        doc.setArray(array);
        QString res = doc.toJson();
        return res;
    }

    void RecordProcessCommunication::SendCode(const QString& Code)
    {
        if(Comunicator && CanSend)
        {
            QString WriteString;
            QXmlStreamWriter xmlWriter(&WriteString);
            xmlWriter.writeTextElement("SetCode",Code);
            Comunicator->Send(WriteString);
            SendData.clear();
        }else
        {
            SendData = Code;
        }
    }

    void RecordProcessCommunication::SetWindow(const QString& Window)
    {
        if(Comunicator && CanSend)
        {
            QString WriteString;
            QXmlStreamWriter xmlWriter(&WriteString);
            xmlWriter.writeTextElement("SetWindow",Window);
            Comunicator->Send(WriteString);
            this->Window.clear();
        }else
        {
            this->Window = Window;
        }
    }

    void RecordProcessCommunication::SendResources(const QList<IRecordProcessCommunication::ResourceDescription>& Resources)
    {
        if(Comunicator && CanSend)
        {
            QString WriteString;
            QXmlStreamWriter xmlWriter(&WriteString);
            xmlWriter.writeTextElement("SetResources",ConvertResources(Resources));
            Comunicator->Send(WriteString);
            SendResourcesString.clear();
        }else
        {
            SendResourcesString = ConvertResources(Resources);
        }

    }

    void RecordProcessCommunication::ReceivedData(const QString& text)
    {
        QXmlStreamReader xmlReader(text);
        while(!xmlReader.atEnd() && !xmlReader.hasError())
        {
            QXmlStreamReader::TokenType token = xmlReader.readNext();

            if(xmlReader.name() == "ReceivedCode" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                bool CanSendIsChangedSaved = CanSendIsChanged;
                CanSendIsChanged = false;
                emit ReceiveCode(xmlReader.text().toString());
                CanSendIsChanged = CanSendIsChangedSaved;
            }
            if(xmlReader.name() == "Terminate" && token == QXmlStreamReader::StartElement)
            {
                emit Stop();
                /*if(Comunicator)
                    Comunicator->AbortByUser();*/
            }
            if(xmlReader.name() == "Restart" && token == QXmlStreamReader::StartElement)
            {
                emit NeedRestart();
                emit Stop();
                /*if(Comunicator)
                    Comunicator->AbortByUser();*/
            }
        }
        if (xmlReader.hasError())
        {
            return;
        }
    }

    void RecordProcessCommunication::SetCanSendDataTrue()
    {
        CanSend = true;
        if(!SendData.isEmpty() && Comunicator)
        {
            QString WriteString;
            QXmlStreamWriter xmlWriter(&WriteString);
            xmlWriter.writeTextElement("SetCode",SendData);
            Comunicator->Send(WriteString);
            SendData.clear();
        }

        if(!SendResourcesString.isEmpty() && Comunicator)
        {
            QString WriteString;
            QXmlStreamWriter xmlWriter(&WriteString);
            xmlWriter.writeTextElement("SetResources",SendResourcesString);
            Comunicator->Send(WriteString);
            SendResourcesString.clear();
        }

        if(!Window.isEmpty() && Comunicator)
        {
            SetWindow(Window);
        }
    }

    void RecordProcessCommunication::SetCanSendDataFalse()
    {
        CanSend = false;
        SendData.clear();
        SendResourcesString.clear();
        Comunicator = 0;
    }

    void RecordProcessCommunication::Closed()
    {
        CanSendIsChanged = false;
    }

    void RecordProcessCommunication::InstallProcessComunicator(IProcessComunicator *Comunicator)
    {
        CanSend = false;
        CanSendIsChanged = true;
        this->Comunicator = Comunicator;
        connect(Comunicator,SIGNAL(Received(QString)),this,SLOT(ReceivedData(QString)));
        connect(Comunicator,SIGNAL(ProcessStarted()),this,SLOT(SetCanSendDataTrue()));
        connect(Comunicator,SIGNAL(ProcessFinished()),this,SLOT(SetCanSendDataFalse()));
    }
}
