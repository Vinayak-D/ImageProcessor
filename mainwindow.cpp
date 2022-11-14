#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QPixmap>
#include <QImage>
#include <QVector>
#include <QQueue>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

//read image using QFileDialog
void MainWindow::on_pushButton_clicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open File"), QDir::homePath(), tr("Images (*.png *.xpm *.jpg)"));
    if (!file_name.isEmpty()){

        //open prompt and display image
        QMessageBox::information(this, "...", file_name);
        QImage img(file_name);
        QPixmap pix = QPixmap::fromImage(img);
        int w = ui->label_pic->width();
        int h = ui->label_pic->height();
        ui->label_pic->setPixmap(pix.scaled(w,h,Qt::KeepAspectRatio));

        //get image width and height, create empty binary matrix
        unsigned int cols = img.width();
        unsigned int rows = img.height();
        unsigned int numBlackPixels = 0;
        QVector<QVector<int>> imgArray(rows,QVector<int>(cols, 0));

        //get pixel data and update matrix
        for (unsigned int i = 0; i < rows; i++){
            for (unsigned int j = 0; j < cols; j++){
                //img.pixel(x,y) where x = col, y = row (coordinates)
                QColor clrCurrent( img.pixel( j, i ));
                int r = clrCurrent.red();
                int g = clrCurrent.green();
                int b = clrCurrent.blue();
                int a = clrCurrent.alpha();
                //if black, assign 1
                if (r+g+b < 20 && a > 240){
                    imgArray[i][j] = 1;
                    numBlackPixels+=1;
                }
            }
        }
        QString filename = "C:/Users/vinay/Desktop/temp.txt";
        QFile fileout(filename);
        if (fileout.open(QFile::ReadWrite | QFile::Text)){
            QTextStream out(&fileout);
            for (unsigned int i = 0; i < rows; i++){
                for (unsigned int j = 0; j < cols; j++){
                    out << imgArray[i][j];
                }
                out << " " << Qt::endl;
            }
        }
        ui->dims->setText(QString::fromStdString("W: " + std::to_string(cols) + "  H: " + std::to_string(rows)));
        float pD = ((float)numBlackPixels/(float)(cols*rows))*100;
        ui->pDark->setText(QString::fromStdString(std::to_string(pD)));

        //process image: find number of blobs
        QVector<int> results = this->findProperties(imgArray, rows, cols);
        //update the UI with the results
        ui->nBlobs->setText(QString::fromStdString(std::to_string(results[0])));
        ui->mBArea->setText(QString::fromStdString(std::to_string(results[1])));
    }

}

//method to find number of blobs in image and area
QVector<int> MainWindow::findProperties(QVector<QVector<int>> &imgArray, unsigned int &rows, unsigned int &cols)
{
    int numIslands = 0;
    int maxArea = 0;
    int area = 0;
    //explored
    QVector<QVector<int>> explored(rows, QVector<int>(cols, 0));

    //loop thru
    for (unsigned int i = 0; i < rows; i++){
        for (unsigned int j = 0; j < cols; j++){

            //if 1 is reached and not explored, begin bfs and add 1 to numIslands
            if (imgArray[i][j] == 1 && explored[i][j] == 0){
                QVector<int> posn{(int)i,(int)j};
                area = this->bfSearch(posn, explored, rows, cols, imgArray);
                if (area > maxArea){
                    maxArea = area;
                }
                numIslands += 1;
            }
        }
    }
    QVector<int> results{numIslands, maxArea};
    return results;
}

//bfs method that returns the area
int MainWindow::bfSearch(QVector<int> &posn, QVector<QVector<int>> &explored, unsigned int &rows, unsigned int &cols, QVector<QVector<int>> &imgArray){

    //potential moves (up, down, left, right)
    QVector<int> u{-1, 0};
    QVector<int> d{1, 0};
    QVector<int> l{0, -1};
    QVector<int> r{0, 1};
    int islandArea = 0;
    //not explored queue
    QQueue <QVector<int>> not_explored;
    not_explored.enqueue(posn);

    while(!not_explored.isEmpty()){
        //find potential moves to explore
        QVector<QVector<int>> potential_moves{{posn[0]+u[0],posn[1]+u[1]},{posn[0]+d[0],posn[1]+d[1]},{posn[0]+l[0],posn[1]+l[1]},{posn[0]+r[0],posn[1]+r[1]}};
        for (QVector<int> move : potential_moves){
            if (move[0] < 0 || move[0] > (int)rows-1 || move[1] < 0 || move[1] > (int)cols-1 || explored[move[0]][move[1]] == 1 || imgArray[move[0]][move[1]] == 0){
                continue;
            }
            not_explored.enqueue(move);
        }
        //add current position to explored array
        explored[posn[0]][posn[1]] = 1;
        islandArea += 1;
        //remove position from front of queue
        not_explored.dequeue();
        if (!not_explored.isEmpty()){
            posn = not_explored.head();
        }
    }
    return islandArea;
}



