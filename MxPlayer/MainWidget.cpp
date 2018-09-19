#include "stdafx.h"
#include "MainWidget.h"

MainWidget::MainWidget(QWidget* parent)
:QWidget(parent)
{
    m_pBtnPlay = new QPushButton("播放", this);
    connect(m_pBtnPlay,SIGNAL(clicked()),this,SLOT(btnPlayClicked()));
    
    m_pBtnPause = new QPushButton("暂停", this);
    connect(m_pBtnPause,SIGNAL(clicked()),this,SLOT(btnPauseClicked()));
    
    m_pBtnStop = new QPushButton("停止", this);
    connect(m_pBtnStop,SIGNAL(clicked()),this,SLOT(btnStopClicked()));

}


void MainWidget::resizeEvent(QResizeEvent* )
{
    m_pBtnPlay->setGeometry(10, 10, 120, 25);
    m_pBtnPause->setGeometry(10, 45, 120, 25);
    m_pBtnStop->setGeometry(10, 80, 120, 25);
}


void MainWidget::btnPlayClicked(){
    
}
void MainWidget::btnPauseClicked(){
    
}
void MainWidget::btnStopClicked(){
    
}













