#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    fontSize = 20;
    fontName = ui->calendarWidget->font().toString();
    font = QFont(fontName, fontSize);

    ui->label_title->setFont(font);

    ui->spinBox_fontSize->setValue(fontSize);

    ui->buttonBox_event->button(QDialogButtonBox::Cancel)->setText(tr("Anuluj"));
    ui->buttonBox_settings->button(QDialogButtonBox::Cancel)->setText(tr("Anuluj"));

    db = QSqlDatabase::addDatabase("QSQLITE");

    QString dbPath = QCoreApplication::applicationDirPath() + "/database";
    db.setDatabaseName(dbPath);

    emptyRecord = true;

    backgroundColor = Qt::yellow;

    selectedDate = ui->calendarWidget->selectedDate();

    previousMonth = selectedDate.month()-1;

    markNotesInMonth(selectedDate.year(), selectedDate.month(), previousMonth);

    on_calendarWidget_clicked(selectedDate);
}

MainWindow::~MainWindow()
{
    delete ui;

    if(db.isOpen())
    {
        db.close();
    }
}

void MainWindow::setCellBackground(const QDate &date, const QColor &color)
{
    QTextCharFormat cellFormat;
    cellFormat.setBackground(color);

    ui->calendarWidget->setDateTextFormat(date, cellFormat);
}

void MainWindow::clearCellBackground(const QDate &date)
{
    QTextCharFormat cellFormat;
    cellFormat.clearBackground();

    ui->calendarWidget->setDateTextFormat(date, cellFormat);
}

void MainWindow::markNotesInMonth(const int year, const int month, const int prevMonth)
{
    if(db.open())
    {
        QSqlQuery query(db);
        QString dateText;

        QDate date;
        int lenghtMonth;

        date.setDate(year, prevMonth, 1);

        lenghtMonth = date.daysInMonth();

        for(int i=1; i<=lenghtMonth; i++)
        {
            date.setDate(year, prevMonth, i+1);
            clearCellBackground(date);
        }

        date.setDate(year, month, 1);

        lenghtMonth = date.daysInMonth();

        daysWithNotes.clear();

        for(int i=1; i<=lenghtMonth; i++)
        {
            if(query.exec("SELECT date FROM events WHERE date='" + date.toString("yyyy-MM-dd") + "'"))
            {
                if(query.next())
                {
                    dateText = query.value(query.record().indexOf("date")).toString();

                    setCellBackground(date, backgroundColor);

                    daysWithNotes << date;
                }
            }

            date.setDate(year, month, i+1);
        }

        db.close();
    }
}

void MainWindow::unlockAddNote()
{
    ui->pushButton_add->setEnabled(true);
    ui->pushButton_edit->setEnabled(false);
    ui->pushButton_remove->setEnabled(false);
}

void MainWindow::unlockEditAndRemoveNote()
{
    ui->pushButton_add->setEnabled(false);
    ui->pushButton_edit->setEnabled(true);
    ui->pushButton_remove->setEnabled(true);
}

void MainWindow::showPageCalendar()
{
    ui->stackedWidget->setCurrentWidget(ui->page_calendar);
    ui->statusBar->show();
}

void MainWindow::showPageEvent()
{
    ui->stackedWidget->setCurrentWidget(ui->page_event);
    ui->statusBar->hide();
}

void MainWindow::on_pushButton_add_clicked()
{
    selectedDate = ui->calendarWidget->selectedDate();

    emptyRecord = true;

    ui->lineEdit->clear();
    ui->plainTextEdit->clear();

    ui->dateEdit->setDate(selectedDate);

    showPageEvent();
}

void MainWindow::on_pushButton_edit_clicked()
{
    selectedDate = ui->calendarWidget->selectedDate();

    if(db.open())
    {
        QSqlQuery query(db);
        QString titleText, noteText;

        if(query.exec("SELECT title, note FROM events WHERE date='" + selectedDate.toString("yyyy-MM-dd") + "'"))
        {
            if(query.next())
            {
                emptyRecord = false;

                titleText = query.value(query.record().indexOf("title")).toString();
                noteText = query.value(query.record().indexOf("note")).toString();

                ui->lineEdit->setText(titleText);
                ui->plainTextEdit->setPlainText(noteText);

                ui->dateEdit->setDate(selectedDate);
            }

            db.close();
        }
    }

    showPageEvent();
}

void MainWindow::on_pushButton_remove_clicked()
{
    selectedDate = ui->calendarWidget->selectedDate();

    if(db.open())
    {
        QSqlQuery query(db);

        if(query.exec("DELETE FROM events WHERE date='" + selectedDate.toString("yyyy-MM-dd") +"'"))
        {
            db.close();
        }
    }

    markNotesInMonth(selectedDate.year(), selectedDate.month(), previousMonth);

    unlockAddNote();
}

void MainWindow::on_pushButton_settings_clicked()
{
    ui->spinBox_fontSize->setValue(fontSize);

    ui->stackedWidget->setCurrentWidget(ui->page_settings);
    ui->statusBar->hide();
}

void MainWindow::on_buttonBox_event_accepted()
{
    QString titleText = ui->lineEdit->text();
    QString noteText = ui->plainTextEdit->toPlainText();

    QDate previousDate = selectedDate;

    selectedDate = ui->dateEdit->date();

    if(db.open())
    {
        QSqlQuery query(db);

        if(emptyRecord)
        {
            query.prepare("INSERT INTO events VALUES(null, '" + titleText + "', '" + noteText + "', '" + selectedDate.toString("yyyy-MM-dd") + "')");
            emptyRecord = false;
        }
        else
        {
            query.exec("SELECT id FROM events WHERE date='" + previousDate.toString("yyyy-MM-dd") + "'");
            query.next();

            int id = query.value(query.record().indexOf("id")).toInt();

            query.prepare("UPDATE events SET title='" + titleText + "', note='" + noteText + "', date='" + selectedDate.toString("yyyy-MM-dd") + "' WHERE id='" + QString::number(id) + "'");
        }

        if(query.exec())
        {
            db.close();
        }
    }

    markNotesInMonth(selectedDate.year(), selectedDate.month(), previousMonth);

    if(ui->pushButton_add->isEnabled())
        unlockEditAndRemoveNote();

    showPageCalendar();
}

void MainWindow::on_buttonBox_event_rejected()
{
    showPageCalendar();
}

void MainWindow::on_buttonBox_settings_accepted()
{
    fontSize = ui->spinBox_fontSize->value();

    int state = ui->comboBox_color->currentIndex();

    switch(state)
    {
        case 0:
            backgroundColor = Qt::yellow;
        break;

        case 1:
            backgroundColor = Qt::green;
        break;

        case 2:
            backgroundColor = Qt::darkBlue;
        break;

        case 3:
            backgroundColor = Qt::black;
        break;

        case 4:
            backgroundColor = Qt::gray;
        break;

        case 5:
            backgroundColor = Qt::darkMagenta;
        break;
    }

    font = QFont(fontName, fontSize);
    ui->calendarWidget->setFont(font);
    ui->label_title->setFont(font);

    markNotesInMonth(selectedDate.year(), selectedDate.month(), previousMonth);

    showPageCalendar();
}

void MainWindow::on_buttonBox_settings_rejected()
{
    showPageCalendar();
}

void MainWindow::on_calendarWidget_clicked(const QDate &date)
{
    unlockAddNote();

    for(int i=0; i<daysWithNotes.length(); i++)
    {
        if(daysWithNotes[i] == date)
        {
            unlockEditAndRemoveNote();
        }
    }

    selectedDate = date;
}

void MainWindow::on_calendarWidget_currentPageChanged(int year, int month)
{
    previousMonth = selectedDate.month();
    int day = selectedDate.day();

    QDate d;
    d.setDate(year, month, 1);

    if(day <= d.daysInMonth())
        selectedDate.setDate(year, month, day);
    else
        selectedDate.setDate(year, month, d.daysInMonth());

    ui->calendarWidget->setSelectedDate(selectedDate);

    markNotesInMonth(year, month, previousMonth);
}

void MainWindow::on_calendarWidget_activated(const QDate &date)
{
    selectedDate = date;

    if(ui->pushButton_edit->isEnabled())
    {
        if(db.open())
        {
            QSqlQuery query(db);
            QString titleText, noteText;

            if(query.exec("SELECT title, note FROM events WHERE date='" + selectedDate.toString("yyyy-MM-dd") + "'"))
            {
                if(query.next())
                {
                    ui->stackedWidget->setCurrentWidget(ui->page_note);

                    titleText = query.value(query.record().indexOf("title")).toString();
                    noteText = query.value(query.record().indexOf("note")).toString();


                    ui->label_title->setText(titleText);

                    ui->label_date->setText(selectedDate.toString("dddd, dd MMMM yyyy"));
                    ui->label_note->setText(noteText);
                }

                db.close();
            }
        }

        ui->statusBar->hide();
    }
}

void MainWindow::on_pushButton_ok_clicked()
{
    showPageCalendar();
}
