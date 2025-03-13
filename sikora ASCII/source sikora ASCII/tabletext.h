#ifndef TABLE_H
#define TABLE_H

#include <QTableView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QHeaderView>
#include <QKeyEvent>
#include <QMessageBox>
#include <QDateTime>
#include <QDebug>

class tableText : public QTableView {
public:
    tableText (QWidget* parent=0) : QTableView(parent)
    {
        initTable();
    }
    ~tableText()
    {

    }

    void initTable()
    {
        local = new QDateTime (QDateTime::currentDateTime());
        model = new QStandardItemModel(0,2,this); //2 Rows and 2 Columns
        model->setHorizontalHeaderItem(0, new QStandardItem(QString("Nec/Lump[mm]")));
        model->setHorizontalHeaderItem(1, new QStandardItem(QString("Czas")));
        this->setModel(model);
        this->setFixedHeight(520);
        this->setFixedWidth(260);
        row = 0;

        //this->setFocusPolicy(Qt::NoFocus);//nie można kasować
        //this->verticalHeader()->hide();//pionowe wyliczenie wierszy
        this->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);//nie można rozciągać kolumn
        this->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);//nie można rozciągać wierszy
        //model->setRowCount(model->rowCount() + 1);//zwiększenie wiersza
        //index = model->index(0,0,QModelIndex());//przypisanie indesu
        //model->setData(index,"123");//wpisanie stałej
        //index = model->index(1,0,QModelIndex());
        //model->setData(index,"kaleka");
    }

    void tableText::setText(QString *string)
    {
        //QDateTime local(QDateTime::currentDateTime());
        //qDebug() << "Local time is:" << local->time();
        //qDebug() << "Local time is:" << local->currentDateTime().time();

        model->setRowCount(model->rowCount() + 1);
        this->row++;
        index = model->index(this->row-1,0,QModelIndex());
        model->setData(index,*string);
        index = model->index(this->row-1,1,QModelIndex());
        //model->setData(index,local->toLocalTime());

        model->setData(index,local->currentDateTime().time().toString());
        this->update(index);
        this->resetHorizontalScrollMode();
    }

    void tableDelRows()
    {
        /*
       if(this->row)
       {
            //model->setRowCount(model->rowCount() - 1);
            //this->row--;
       }
       */
            QMessageBox msgBox;
            msgBox.setWindowFlags(Qt::WindowStaysOnTopHint);
            msgBox.setText("Uwaga!");
            msgBox.setInformativeText("Czy chcesz skasować zaznczone wiersze");
            msgBox.addButton(tr("Nie"), QMessageBox::NoRole);
            msgBox.setStandardButtons(QMessageBox::Yes);
            msgBox.setButtonText(QMessageBox::Yes, trUtf8("Tak"));

            if (msgBox.exec() == QMessageBox::Yes) {
                this->indexes =  this->selectionModel()->selectedRows();
                int countRow = indexes.count();

                for( int i = countRow; i > 0; i--)
                this->model->removeRow(indexes.at(i-1).row(), QModelIndex());
            }



    }

    void keyPressEvent(QKeyEvent *ev)
    {
        if(ev->key()==Qt::Key_Delete)
        {
            tableDelRows();

        }
        ev->accept();
    }

    /*
        QShortcut * shortcut = new QShortcut(QKeySequence(Qt::Key_F2),this,SLOT(printChoice()));
        shortcut->setAutoRepeat(false);
     */
public:
    QDateTime *local;
    QStandardItemModel *model;
    QStandardItem *firstRow;
    QModelIndex index;
    QModelIndexList indexes;

    int row;
};
#endif // TABLE_H
