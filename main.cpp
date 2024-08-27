#include <QApplication>
#include <QWidget>

#include <iostream>
#include <QTimer>
#include <QThread>
// #include "wid/widget.h"
#include "window/main_window.h"
#include "window/NetApiLib/Network.h"
#include "window/NetApiLib/NetHandle.h"
#include "window/NetApiLib/PaserProtocolFile.h"
#include "Tvlog.h"
using namespace std;


int run_main_app(int argc, char *argv[])
{
    QApplication a(argc,argv);

    main_window w;
    w.resize(500,500);
    w.show();

    Network::init([&](bool ok){
        emit _sp_net_->sn_connect_sdk(ok);
    });

    return a.exec();
}

int exit_main_app(int run_value)
{
    cout<<"exit run value : "<<run_value<<endl;
    return run_value;
}

int perse_arg(int argc, char *argv[])
{
    cout<<"perse_arg : "<<argc<<endl;
    for(int i=0;i<argc;i++)
    {
        cout<<"arg: "<<string(argv[i])<<endl;
    }
    return argc;
}

int main(int argc, char *argv[])
{
    Tflogs::get()->init("../config/out.log");
    Tflogs::get()->set_level(vlevel4::e_info);
    Tvlogs::get()->set_level(vlevel4::e_info);

    flogd("\n\n\n\n");
    flogd("== Run Main ==");

    perse_arg(argc,argv);

    int run_value = run_main_app(argc,argv);

    return exit_main_app(run_value);
}