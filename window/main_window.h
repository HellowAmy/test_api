
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
    bool _is_connect_host = false;
    bool _is_connect_sdk = false;
    int _timer_id = 0;
    int _timer_count = 0;
    int _timer_connecting = 0;

    QLabel *_lab_status_sdk = nullptr;
    QLabel *_lab_status_host = nullptr;
    QPushButton *_but_connect = nullptr;
    QPushButton *_but_quit = nullptr;
    QPushButton *_but_update_file = nullptr;
    QPushButton *_but_send_json = nullptr;
    QPushButton *_but_send_api = nullptr;
    QPushButton *_but_send_stop = nullptr;
    QLineEdit *_edit_api = nullptr;
    QTextEdit *_edit_json = nullptr;
    QListWidget *_lab_err_tips = nullptr;
    QLabel *_lab_timer_num = nullptr;
    QLabel *_lab_connect_time = nullptr;
    QLabel *_lab_count_send = nullptr;

};
#endif // MAIN_WINDOW_H
