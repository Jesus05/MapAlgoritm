#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
  Q_OBJECT
  
public:
  explicit MainWindow(QWidget *parent = 0);
  ~MainWindow();
  
private slots:
  void on_pushButton_clicked();
  void resizeEvent(QResizeEvent *event);
  
private:
  Ui::MainWindow *ui;
  int m_minX;
  int m_minY;
  int m_maxX;
  int m_maxY;
  void TestFour();
};

#endif // MAINWINDOW_H
