#ifndef TRAININGWIDGET_H
#define TRAININGWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <vector>
#include "mahjonglogic.h"

class TrainingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TrainingWidget(QWidget *parent = nullptr);

private slots:
    void generateNewHand();
    void onTileClicked(int index);

private:
    std::vector<Tile> currentHand;
    QVBoxLayout *mainLayout;
    QHBoxLayout *handLayout;
    QLabel *infoLabel;
    QList<QPushButton*> tileButtons;
    
    void updateUI();
};

#endif // TRAININGWIDGET_H
