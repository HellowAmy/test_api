
#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QWidget>
#include <QLabel>
#include <QTimerEvent>
#include <QPushButton>
#include <QTextEdit>
#include <QLineEdit>
#include <QListWidget>
#include "NetApiLib/NetHandle.h"
#include "NetApiLib/PaserProtocolFile.h"

class main_window : public QWidget
{
    Q_OBJECT
public:
    main_window(QWidget *parent = nullptr);
    ~main_window();


    void SetStatusSdk(bool ok);
    void SetStatusHost(bool ok);

    void SetErrTips(QString info,bool err = true);

signals:
    void sn_click();

protected:
    
    void timerEvent(QTimerEvent *event) override;


private:
    int timer_id = 0;
    int timer_count = 0;

    QLabel *_lab_status_sdk = nullptr;
    QLabel *_lab_status_host = nullptr;
    QPushButton *_but_connect = nullptr;
    QPushButton *_but_update_file = nullptr;
    QPushButton *_but_send_json = nullptr;
    QPushButton *_but_send_api = nullptr;
    QLineEdit *_edit_api = nullptr;
    QTextEdit *_edit_json = nullptr;
    QListWidget *_lab_err_tips = nullptr;
};
#endif // MAIN_WINDOW_H
