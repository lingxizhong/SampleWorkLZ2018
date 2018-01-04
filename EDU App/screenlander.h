/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#ifndef SCREENLANDER_H
#define SCREENLANDER_H

#include <QObject>
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

class screenLander : public QWidget
{
    Q_OBJECT
public:
    explicit screenLander(QWidget *parent = nullptr);

    ~screenLander();

signals:

public slots:
    void onPress();

private:
    QGridLayout *layout;
    QFont font;
    QLabel *titleLabel;
    QLabel *playGameLabel;
    QPushButton *playGameButton;
};

#endif // SCREENLANDER_H
