#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QProcess>
#include <QDebug>
#include <QList>
#include <QStringListModel>
#include <QFileDialog>
#include <QStringList>
#include <QStandardItemModel>
#include <QAction>
#include <QListView>
#include <QTime>
#include <QSettings>
#include <QTextCodec>
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();
    QListView* find(const QString);

private slots:
    void on_pushButton_clicked();

    void term(const int);

    void conn(const int);

    void log(const QString);

    void on_userName_editingFinished();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    bool questionBox(QString);

    void addToFile(QString, QString);

    QStringList readFile(QString);

    int countLinesInFile(QString);

    QString runCommand(QString, QStringList);

    QString re(QString, QString);
private:
    Ui::Widget *ui;
};
#endif // WIDGET_H
