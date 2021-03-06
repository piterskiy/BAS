#include "subprocesswebelement.h"
#include <QXmlStreamWriter>
#include <QRect>
#include "every_cpp.h"

namespace BrowserAutomationStudioFramework
{
    SubprocessWebElement::SubprocessWebElement(QObject *parent) :
        IWebElement(parent)
    {
        const char alphanum[] = "abcdefghijklmnopqrstuvwxyz";
        for(int i = 0;i<10;i++)
            ID += QString(alphanum[qrand() % (sizeof(alphanum) - 1)]);
    }

    QString SubprocessWebElement::GetSelectorString()
    {
        QString res = "[documentRoot";
        int len = Selector.length();
        for(int i =0;i<len;i++)
        {
            res += ", ";
            QPair<QString,QString> s = Selector.at(i);
            res += s.first;
            res += " ";
            res += s.second;
        }
        res += "]";
        return res;
    }
    void SubprocessWebElement::GetInternalList(QList<IWebElement*>& List)
    {

    }
    SubprocessWebElement::~SubprocessWebElement()
    {

    }

    QRect SubprocessWebElement::GetGeometry()
    {
        return QRect();
    }

    void SubprocessWebElement::Received(const QString& value)
    {
        QXmlStreamReader xmlReader(value);
        QString IDIncome;
        while(!xmlReader.atEnd() && !xmlReader.hasError())
        {
            QXmlStreamReader::TokenType token = xmlReader.readNext();

            if(xmlReader.name() == "Error" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetFailMessage(xmlReader.text().toString());
                Worker->FailBecauseOfTimeout();
                return;
            }

            if(xmlReader.name() == "Element" && token == QXmlStreamReader::StartElement)
            {
                foreach(QXmlStreamAttribute attr,xmlReader.attributes())
                {
                    if(attr.name() == "ID")
                    {
                        IDIncome = attr.value().toString();

                    }
                }
            }
            if(IDIncome!=ID)
                continue;


            if(xmlReader.name() == "xml" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetAsyncResult(QScriptValue(xmlReader.text().toString()));
                emit xml();
            }else if(xmlReader.name() == "script" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetAsyncResult(QScriptValue(xmlReader.text().toString()));
                emit script();
            }else if(xmlReader.name() == "click" && token == QXmlStreamReader::StartElement)
            {
                emit click();
            }else if(xmlReader.name() == "system_click" && token == QXmlStreamReader::StartElement)
            {
                emit system_click();
            }else if(xmlReader.name() == "move" && token == QXmlStreamReader::StartElement)
            {
                emit move();
            }else if(xmlReader.name() == "text" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetAsyncResult(QScriptValue(xmlReader.text().toString()));
                emit text();
            }else if(xmlReader.name() == "clear" && token == QXmlStreamReader::StartElement)
            {
                emit clear();
            }else if(xmlReader.name() == "type" && token == QXmlStreamReader::StartElement)
            {
                emit type();
            }else if(xmlReader.name() == "fill" && token == QXmlStreamReader::StartElement)
            {
                emit fill();
            }else if(xmlReader.name() == "exist" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetAsyncResult(xmlReader.text().toString().toInt());
                emit exist();
            }else if(xmlReader.name() == "submit" && token == QXmlStreamReader::StartElement)
            {
                emit submit();
            }else if(xmlReader.name() == "style" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetAsyncResult(QScriptValue(xmlReader.text().toString()));
                emit style();
            }else if(xmlReader.name() == "check" && token == QXmlStreamReader::StartElement)
            {
                emit check();
            }else if(xmlReader.name() == "focus" && token == QXmlStreamReader::StartElement)
            {
                emit focus();
            }else if(xmlReader.name() == "set" && token == QXmlStreamReader::StartElement)
            {
                emit set();
            }else if(xmlReader.name() == "set_integer" && token == QXmlStreamReader::StartElement)
            {
                emit set_integer();
            }else if(xmlReader.name() == "set_random" && token == QXmlStreamReader::StartElement)
            {
                emit set_random();
            }else if(xmlReader.name() == "attr" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetAsyncResult(QScriptValue(xmlReader.text().toString()));
                emit attr();
            }else if(xmlReader.name() == "set_attr" && token == QXmlStreamReader::StartElement)
            {
                emit set_attr();
            }else if(xmlReader.name() == "length" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetAsyncResult(QScriptValue(xmlReader.text().toString().toInt()));
                emit length();
            }else if(xmlReader.name() == "render_file" && token == QXmlStreamReader::StartElement)
            {
                emit render_file();
            }else if(xmlReader.name() == "render_base64" && token == QXmlStreamReader::StartElement)
            {
                xmlReader.readNext();
                Worker->SetAsyncResult(QScriptValue(xmlReader.text().toString()));
                emit render_base64();
            }


        }
        if (xmlReader.hasError())
        {
            return;
        }
    }

    void SubprocessWebElement::AddSelector(const QString& key,const QString& value)
    {
        QPair<QString,QString> pair;
        pair.first = key;
        pair.second = value;
        Selector.append(pair);
    }

    void SubprocessWebElement::SetWorker(IWorker* Worker)
    {
        this->Worker = Worker;
    }

    IWorker* SubprocessWebElement::GetWorker()
    {
        return Worker;
    }

    QString SubprocessWebElement::CreateXmlElement(const QString& ElementName,const QString& Text1,const QString& Text2)
    {
        QString WriteString;
        QXmlStreamWriter xmlWriter(&WriteString);
        xmlWriter.writeStartElement("Element");
            xmlWriter.writeStartElement(ElementName);
            xmlWriter.writeAttribute("ID",ID);
            xmlWriter.writeTextElement("A",Text1);
            xmlWriter.writeTextElement("B",Text2);

            xmlWriter.writeStartElement("Path");
                int len = Selector.length();
                for(int i =0;i<len;i++)
                {
                    QPair<QString,QString> s = Selector.at(i);
                    xmlWriter.writeTextElement(s.first,s.second);
                }
            xmlWriter.writeEndElement();

            xmlWriter.writeEndElement();
        xmlWriter.writeEndElement();
        return WriteString;
    }

    void SubprocessWebElement::xml(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during xml for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(xml()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("xml"));
    }

    void SubprocessWebElement::text(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during text for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(text()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("text"));
    }
    void SubprocessWebElement::script(const QString& javascript, const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during script for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(script()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("script",javascript));
    }
    void SubprocessWebElement::RunScriptAsync(const QString& javascript, const QObject *object_success,const char * slot_success)
    {
        Worker->SetFailMessage(QString("Timeout during RunScriptAsync for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(javascript()), object_success,slot_success, Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("javascript",javascript));
    }

    QString SubprocessWebElement::RunScriptNoCallback(const QString& javascript)
    {
        return QString();
    }

    void SubprocessWebElement::click(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during click for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(click()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("click"));
    }

    void SubprocessWebElement::system_click(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during system_click for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(system_click()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("system_click"));
    }

    void SubprocessWebElement::move(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during move for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(move()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("move"));
    }

    void SubprocessWebElement::clear(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during clear for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(clear()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("clear"));
    }
    void SubprocessWebElement::type(const QString& text, int interval, const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during type for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(type()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("type",text, QString::number(interval)));
    }
    void SubprocessWebElement::type(const QString& text, const QString& callback)
    {
        type(text,10,callback);
    }
    void SubprocessWebElement::fill(const QString& text, const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during fill for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(fill()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("fill",text));
    }
    void SubprocessWebElement::exist(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during exist for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(exist()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("exist"));
    }
    void SubprocessWebElement::submit(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during submit for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(submit()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("submit"));
    }
    void SubprocessWebElement::style(const QString& text, const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during style for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(style()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("style", text));
    }
    void SubprocessWebElement::check(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during check for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(check()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("check"));
    }
    void SubprocessWebElement::focus(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during focus for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(focus()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("focus"));
    }
    void SubprocessWebElement::set(const QString& text,const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during set for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(set()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("set",text));
    }
    void SubprocessWebElement::set_integer(int index,const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during set_integer for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(set_integer()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("set_integer", QString::number(index)));
    }
    void SubprocessWebElement::set_random(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during set_random for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(set_random()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("set_random"));
    }
    void SubprocessWebElement::attr(const QString& text,const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during attr for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(attr()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("attr", text));
    }
    void SubprocessWebElement::set_attr(const QString& val,const QString& text,const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during set_attr for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(set_attr()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("set_attr", val, text));
    }
    IWebElement* SubprocessWebElement::css(const QString& css)
    {
        SubprocessWebElement *element = new SubprocessWebElement();
        element->Selector = this->Selector;
        element->AddSelector("css",css);
        element->Worker = Worker;
        connect(Worker->GetProcessComunicator(),SIGNAL(Received(QString)),element,SLOT(Received(QString)));
        return element;
    }

    IWebElement* SubprocessWebElement::frame(const QString& name)
    {
        SubprocessWebElement *element = new SubprocessWebElement();
        element->Selector = this->Selector;
        element->AddSelector("frame",name);
        element->Worker = Worker;
        connect(Worker->GetProcessComunicator(),SIGNAL(Received(QString)),element,SLOT(Received(QString)));
        return element;
    }

    IWebElement* SubprocessWebElement::position(int x, int y)
    {
        SubprocessWebElement *element = new SubprocessWebElement();
        element->Selector = this->Selector;
        element->AddSelector("position",QString::number(x) + QString(",") + QString::number(y));
        element->Worker = Worker;
        connect(Worker->GetProcessComunicator(),SIGNAL(Received(QString)),element,SLOT(Received(QString)));
        return element;
    }

    void SubprocessWebElement::length(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during length for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(length()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("length"));
    }

    IWebElement* SubprocessWebElement::at(int at)
    {
        SubprocessWebElement *element = new SubprocessWebElement();
        element->Selector = this->Selector;
        element->AddSelector("at",QString::number(at));
        element->Worker = Worker;
        connect(Worker->GetProcessComunicator(),SIGNAL(Received(QString)),element,SLOT(Received(QString)));
        return element;
    }
    IWebElement* SubprocessWebElement::all(const QString& css)
    {
        SubprocessWebElement *element = new SubprocessWebElement();
        element->Selector = this->Selector;
        element->AddSelector("all",css);
        element->Worker = Worker;
        connect(Worker->GetProcessComunicator(),SIGNAL(Received(QString)),element,SLOT(Received(QString)));
        return element;
    }
    void SubprocessWebElement::render_file(const QString& path,const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during render_file for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(render_file()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("render_file", path));
    }
    void SubprocessWebElement::render_base64(const QString& callback)
    {
        Worker->SetScript(PrepareCallback(callback));
        Worker->SetFailMessage(QString("Timeout during render_base64 for") + GetSelectorString());
        Worker->GetWaiter()->WaitForSignal(this,SIGNAL(render_base64()), Worker,SLOT(RunSubScript()), Worker, SLOT(FailBecauseOfTimeout()));
        Worker->GetProcessComunicator()->Send(CreateXmlElement("render_base64"));
    }
    IWebElement* SubprocessWebElement::match(const QString& text)
    {
        SubprocessWebElement *element = new SubprocessWebElement();
        element->Selector = this->Selector;
        element->AddSelector("match",text);
        element->Worker = Worker;

        connect(Worker->GetProcessComunicator(),SIGNAL(Received(QString)),element,SLOT(Received(QString)));
        return element;
    }
    IWebElement* SubprocessWebElement::match_all(const QString& text)
    {
        SubprocessWebElement *element = new SubprocessWebElement();
        element->Selector = this->Selector;
        element->AddSelector("match_all",text);
        element->Worker = Worker;
        connect(Worker->GetProcessComunicator(),SIGNAL(Received(QString)),element,SLOT(Received(QString)));
        return element;
    }

    QString SubprocessWebElement::PrepareCallback(const QString& callback)
    {
        int first_index = callback.indexOf("{");
        int last_index = callback.lastIndexOf("}");
        if(first_index<0)
            return callback;
        if(last_index<0)
            return callback;

        return callback.mid(first_index+1,last_index - first_index - 1);
    }
}
