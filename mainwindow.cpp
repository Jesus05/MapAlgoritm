#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "DebugAddons.h"
#include <QDebug>
#include "Types.h"
#include <QWheelEvent>
#include <QImage>
#include "trandomizer.h"
#include "troomsgenerator.h"

MainWindow *debugPointer;

MapObjectStruct ObjectList[] = {
  { 0, "Enter"     , 0, 100, {   -1}, {1, 2, 4,          -1}},
  { 1, "Exit"      , 0, 100, {   -1}, {0,                -1}},
  { 2, "BeforeExit", 0, 100, {   -1}, {0, 1, 2, 3, 4, 5, -1}}, //Думаю ее вообще лучше ставить постпроцессом.
  { 3, "Situation" , 0, 100, {   -1}, {                  -1}},
  { 4, "Boss"      , 3, 100, {   -1}, {0,                -1}},
  { 5, "Vault"     , 4, 100, {   -1}, {                  -1}}
};

MapObjAlgList GetList()
{
  MapObjAlgList result;
  
  MapObjectForAlg temp;
  
  temp = ObjectList[0];
  
  temp.chance = 100;
  temp.minCount = 1;
  temp.maxCount = 1;
  
  result.push_back(temp);
  
  temp = ObjectList[1];
  
  temp.chance = 100;
  temp.minCount = 1;
  temp.maxCount = 1;
  
  result.push_back(temp);
  
  temp = ObjectList[2];
  
  temp.chance = 100;
  temp.minCount = 1;
  temp.maxCount = 1;
  
  result.push_back(temp);
  
  temp = ObjectList[3];
  
  temp.chance = 100;
  temp.minCount = 5;
  temp.maxCount = 12;
  
  result.push_back(temp);
  
  temp = ObjectList[4];
  
  temp.chance = 100;
  temp.minCount = 0;
  temp.maxCount = 2;
  
  result.push_back(temp);
  
  temp = ObjectList[5];
  
  temp.chance = 100;
  temp.minCount = 0;
  temp.maxCount = 1;
  
  result.push_back(temp);
  return result;
}

MainWindow::MainWindow(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  ui->setupUi(this);
  ui->graphicsView->setScene(new QGraphicsScene);
  ui->graphicsView->setSceneRect(0, 0, 1, 1);
  m_minX = 0;
  m_minY = 0;
  m_maxX = 1;
  m_maxY = 1;
  debugPointer = this;
  ui->graphicsView->viewport()->installEventFilter(this);
}

MainWindow::~MainWindow()
{
  delete ui;
}

void TestOne()
{
  int min = 0;
  int max = 1000;
  
  static int up = 0, down = 0, left = 0, right = 0;
//  for (int j = min; j < max; j++)
  {

    for (int i = min; i < max; i++)
    {
      const Directions temp = TRoomsGenerator::GetRandomExits(3, (Directions)(DIR_DOWN | DIR_LEFT));
      if (temp & DIR_UP) up++;
      if (temp & DIR_DOWN) down++;
      if (temp & DIR_LEFT) left++;
      if (temp & DIR_RIGHT) right++;
    }
    
    int summ = up + down + left + right;
    
//    double upP = (double)up/summ;
//    double downP = (double)down/summ;
//    double leftP = (double)left/summ;
//    double rightP = (double)right/summ;
    
    qDebug() << DIR_UP << (double)up/summ << DIR_DOWN << (double)down/summ << DIR_LEFT << (double)left/summ << DIR_RIGHT << (double)right/summ;
  }
  
//  qDebug() << GetRandomExits(1, DIR_NO);

/*  qDebug() << "Min: " << min << " Max: " << max;

  for (int i = min; i < max + 1; i++)
  {
    int roomCount = GetRandomExitCount(i, max);
//    if ((roomCount == 4 || roomCount == 1) && i != 0)
      qDebug() << i << " : " << roomCount;
  }*/
}

void TestTwo()
{
  int min = 100;
  int max = 101;
  
  for (int i = min; i < max; i++)
  {
    qDebug() << "BeforeDraw";
    TRoomsGenerator::DrawLabirint(GetList(), i);
    qDebug() << "AfterDraw";
  }
}

void TestThree()
{
  int min = 0;
  int max = 1000;

  static int up = 0, down = 0, left = 0, right = 0;
//  for (int j = min; j < max; j++)
  {

    for (int i = min; i < max; i++)
    {
      const Directions temp = TRoomsGenerator::SelectRandDirection((Directions)(DIR_DOWN | DIR_LEFT | DIR_UP | DIR_RIGHT));
      if (temp & DIR_UP) up++;
      if (temp & DIR_DOWN) down++;
      if (temp & DIR_LEFT) left++;
      if (temp & DIR_RIGHT) right++;
    }
    
    int summ = up + down + left + right;
    
/*    double upP = (double)up/summ;
    double downP = (double)down/summ;
    double leftP = (double)left/summ;
    double rightP = (double)right/summ;
*/    
    qDebug() << DIR_UP << (double)up/summ << DIR_DOWN << (double)down/summ << DIR_LEFT << (double)left/summ << DIR_RIGHT << (double)right/summ;
  }

}

void MainWindow::on_pushButton_clicked()
{
  TestFour();
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
  QMainWindow::resizeEvent(event);
//  if (ui->graphicsView->scene() != NULL)
  {
    ui->graphicsView->fitInView(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY, Qt::KeepAspectRatio);
    ui->graphicsView->fitInView(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY, Qt::KeepAspectRatio);
  }
}

bool MainWindow::eventFilter(QObject */*target*/, QEvent *event)
{
  if (QEvent::Wheel == event->type())
  {
    QWheelEvent *wheelEvent = dynamic_cast<QWheelEvent*>(event);
    if (wheelEvent != NULL)
    {
      double d = 1.0 + 10.0 / wheelEvent->delta();
      ui->graphicsView->scale(d, d);
      event->accept();
      return true;
    }
  }
  return false;
}


void PrintAllRoomNeighborTags(const RoomList &rooms)
{
  RoomList::const_iterator it;
    for (it = rooms.begin(); it != rooms.end(); it++)
  {
    const Room &room = *it;
    const QList<Tag> &temp = TRoomsGenerator::GetNearestRoomTags(room, rooms);
    const MapObjAlgList &temp2 = TRoomsGenerator::GetObjListForRoom(GetList(), room, rooms);
    qDebug() << "TagList: " << room << temp << temp2;
  }
}

void MainWindow::TestFour()
{
  int roomCount = ui->leCount->text().toInt();
  m_minX = 0;
  m_minY = 0;
  m_maxX = 0;
  m_maxY = 0;

  int randSeed = ui->leSeed->text().toInt(); //qrand();
  qDebug() << "randSeed" << randSeed;
  qsrand(randSeed);

  qDebug() << "BeforeDraw";
  RoomList rooms = TRoomsGenerator::DrawLabirint(GetList(), roomCount);
  qDebug() << "AfterDraw" << "room count" << rooms.count();
  
  DrawRooms(rooms);
  
//  PrintAllRoomNeighborTags(rooms);
  
  on_pushButton_2_clicked();
}

QColor GetColorByTag(const Tag &tag)
{
  switch (tag) {
    case 0: return Qt::blue; break; //Enter
    case 1: return Qt::darkBlue; break; //Exit
    case 2: return Qt::darkCyan; break; //BeforeExit
    case 3: return Qt::yellow; break; //Situation
    case 4: return Qt::darkGray; break; //Boss
    case 5: return Qt::magenta; break; //Vault
    default: return Qt::black; break;
  }
}

void MainWindow::DrawRooms(const RoomList &rooms)
{
  ui->graphicsView->scene()->clear();
  ui->graphicsView->setSceneRect(0, 0, 1, 1);
  
  RoomList::const_iterator it;
  
  for (it = rooms.begin(); it != rooms.end(); it++)
  {
    const Room &room = *it;
    const int lminx = room.x * 20, lminy = -room.y * 20, lmaxx = (room.x+1) * 20, lmaxy = (-room.y+1) * 20;
    ui->graphicsView->scene()->addRect(lminx + 5, lminy + 5, 10, 10, QPen(), QBrush(GetColorByTag(room.roomType)));
    m_minX = qMin(m_minX, lminx);
    m_minY = qMin(m_minY, lminy);
    m_maxX = qMax(m_maxX, lmaxx);
    m_maxY = qMax(m_maxY, lmaxy);
    
    if (room.exits & DIR_LEFT) ui->graphicsView->scene()->addLine(lminx, lminy + 10, lminx + 5, lminy + 10);
    if (room.exits & DIR_UP) ui->graphicsView->scene()->addLine(lminx + 10, lminy, lminx + 10, lminy + 5);
    if (room.exits & DIR_DOWN) ui->graphicsView->scene()->addLine(lminx + 10, lminy + 20, lminx + 10, lminy + 15);
    if (room.exits & DIR_RIGHT) ui->graphicsView->scene()->addLine(lminx + 20, lminy + 10, lminx + 15, lminy + 10);
    
//    qDebug() << "minX:" << minX << "minY:" << minY << "maxX" << maxX << "maxY" << maxY;
  }
  
//  qDebug() << "minX:" << minX << "minY:" << minY << "maxX" << maxX << "maxY" << maxY;
  
  ui->graphicsView->fitInView(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY, Qt::KeepAspectRatio);
  ui->graphicsView->fitInView(m_minX, m_minY, m_maxX - m_minX, m_maxY - m_minY, Qt::KeepAspectRatio);
}

void MainWindow::on_pushButton_2_clicked()
{
  ui->lLastSeed->setText(ui->leSeed->text());
//  ui->pushButton_2->setText(QString("Generate seed. old seed:") + ui->leSeed->text());
  ui->leSeed->setText(QString::number(qrand()));
}

void MainWindow::on_pushButton_3_clicked()
{
  QImage image(ui->graphicsView->scene()->sceneRect().size().toSize(), QImage::Format_ARGB32);
  QPainter painter(&image);
  painter.setRenderHint(QPainter::Antialiasing);
  ui->graphicsView->scene()->render(&painter);
  image.save(QString("Labirint seed ") + ui->lLastSeed->text() + " count " + ui->leCount->text() + ".png");
  ui->graphicsView->scene()->items().count();
}
