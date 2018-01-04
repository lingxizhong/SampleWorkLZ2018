/***
 * Authors: Matt Wilson, Osama Kergaye, Lingxi Zhong, Kathy Huang, Andrew Radford, Joel Swanson
 *
 */

#ifndef SCREENWELCOME_H
#define SCREENWELCOME_H

#include <QObject>
#include <QWidget>

class screenWelcome : public QWidget
{
    Q_OBJECT
public:
    explicit screenWelcome(QWidget *parent = nullptr);

signals:

public slots:
};

#endif // SCREENWELCOME_H
