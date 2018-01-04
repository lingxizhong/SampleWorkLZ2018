/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#ifndef SCREENREWARD_H
#define SCREENREWARD_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QGridLayout>
#include <QPushButton>

class screenReward : public QWidget
{
    Q_OBJECT
public:
    explicit screenReward(QWidget *parent = nullptr);

    ~screenReward();

signals:

public slots:
    void change(bool);
private:
    QVBoxLayout *layout;
    QLabel *headerText;
    QLabel *firstSentence;
    QLabel *markovText;
    QLabel *rewardText;
    QLabel *rewardImage;
    QLabel *markovImage;
    QFont font;
    QPushButton *diagram;
    int flag;


};

#endif // SCREENREWARD_H
