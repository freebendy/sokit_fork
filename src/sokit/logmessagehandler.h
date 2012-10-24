#include <QWidget>
#include <QPlainTextEdit>
#include <QMetaObject>
#include <QThread>
#include <QBoxLayout>
#include <QDateTime>
#include <QDebug>
#include <cstdio>
#include <cassert>

QWidget         *DEBUG_MESSAGE_DISPLAY_WIDGET   = NULL;
QPlainTextEdit  *DEBUG_MESSAGE_DISPLAY_TEXTEDIT = NULL;

void debugMessageDisplayFunc(QtMsgType type, const char *msg)
{
//    bool do_abort = false;
    const char* msgTypeStr = NULL;
    switch (type) {
    case QtDebugMsg:
        msgTypeStr = "Debug";
        break;
    case QtWarningMsg:
        msgTypeStr = "Warning";
        break;
    case QtCriticalMsg:
        msgTypeStr = "Critical";
        break;
    case QtFatalMsg:
        msgTypeStr = "Fatal";
//        do_abort = true;
    default:
        assert(0);
        return;
    }
    QTime now = QTime::currentTime();
    QString formattedMessage = 
        QString::fromLatin1("%1 %2 %3")
        .arg(now.toString("hh:mm:ss:zzz"))
        .arg(msgTypeStr).arg(msg);
    // print on console:
    fprintf( stderr, "%s\n", formattedMessage.toLocal8Bit().constData() );
    // print in debug log window
    {

        bool isMainThread = QThread::currentThread() == QApplication::instance()->thread();
        if(DEBUG_MESSAGE_DISPLAY_TEXTEDIT)
        {
            if( isMainThread )
                DEBUG_MESSAGE_DISPLAY_TEXTEDIT->appendPlainText( formattedMessage );
            else // additional code, so that qDebug calls in threads will work aswell
                QMetaObject::invokeMethod( DEBUG_MESSAGE_DISPLAY_TEXTEDIT, "appendPlainText", Qt::QueuedConnection, Q_ARG( QString, formattedMessage ) );
        }
    }
}

void setupDebugDisplay()
{
    QWidget *widget = new QWidget();
    widget->setWindowTitle( "Debug Log" );
    widget->setMinimumSize(600, 400);
    //quit only when main window is closed
    widget->setAttribute( Qt::WA_QuitOnClose, false ); 
    QBoxLayout* layout = new QVBoxLayout();
    widget->setLayout( layout );
    QPlainTextEdit *textEdit = new QPlainTextEdit( widget );
    QFont font = QFont( "Monospace" );
    font.setStyleHint(QFont::TypeWriter);
    textEdit->setFont( font );
    textEdit->setReadOnly(true);
    layout->addWidget( textEdit );
    widget->show();
    DEBUG_MESSAGE_DISPLAY_WIDGET   = widget;
    DEBUG_MESSAGE_DISPLAY_TEXTEDIT = textEdit;
    qInstallMsgHandler(debugMessageDisplayFunc);
}
