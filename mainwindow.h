#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_clicked();
    QVector<int> findProperties(QVector<QVector<int>> &imgArray, unsigned int &rows, unsigned int &cols);
    int bfSearch(QVector<int> &posn, QVector<QVector<int>> &explored, unsigned int &rows, unsigned int &cols, QVector<QVector<int>> &imgArray);

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
