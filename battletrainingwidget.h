#ifndef BATTLETRAININGWIDGET_H
#define BATTLETRAININGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QStackedWidget>
#include "combatarenawidget.h"

class BattleTrainingWidget : public QWidget {
    Q_OBJECT
public:
    explicit BattleTrainingWidget(QWidget *parent = nullptr);

private slots:
    void onRealtimeAssist();
    void onAISimulation();
    void onBackToMenu();

private:
    QStackedWidget *m_stack;
    QWidget *m_menuPage;
    CombatArenaWidget *m_arenaWidget;

    void setupMenuPage();
};

#endif // BATTLETRAININGWIDGET_H
