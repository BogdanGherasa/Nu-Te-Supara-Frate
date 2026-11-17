#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_NuTeSuparaFrateUI.h"

class NuTeSuparaFrateUI : public QMainWindow
{
    Q_OBJECT

public:
    NuTeSuparaFrateUI(QWidget *parent = nullptr);
    ~NuTeSuparaFrateUI();

private:
    Ui::NuTeSuparaFrateUIClass ui;
};

