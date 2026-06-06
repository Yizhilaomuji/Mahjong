#ifndef TRAININGWIDGET_H
#define TRAININGWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <vector>
#include "mahjonglogic.h" // 假设你的逻辑类名

class TrainingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TrainingWidget(QWidget *parent = nullptr);

private slots:
    void onTileClicked(int index);
    void onHuClicked();
    void onContinueClicked();

private:
    void generateNewHand();
    void updateUI();

    QVBoxLayout *mainLayout;
    QHBoxLayout *handLayout;
    QLabel *infoLabel;
    QPushButton *huButton;
    
    std::vector<Tile> currentHand;
    QList<QPushButton*> tileButtons;
    bool m_isAnswered = false; // 记录当前题目是否已操作
};

#endif