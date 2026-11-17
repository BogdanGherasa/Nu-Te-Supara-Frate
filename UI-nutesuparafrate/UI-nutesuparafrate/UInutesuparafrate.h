#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_UInutesuparafrate.h"

class UInutesuparafrate : public QMainWindow
{
    Q_OBJECT

public:
    UInutesuparafrate(QWidget *parent = nullptr);
    ~UInutesuparafrate();

private:
    Ui::UInutesuparafrateClass ui;
};

