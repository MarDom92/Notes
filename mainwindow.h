#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlRecord>
#include <QSqlQuery>
#include <QSqlError>
#include <QPushButton>
#include <QStatusBar>
#include <QStackedWidget>
#include <QBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QDateEdit>
#include <QCalendarWidget>
#include <QSpinBox>
#include <QMessageBox>

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow
{
        Q_OBJECT

    public:
        explicit MainWindow(QWidget *parent = 0);
        ~MainWindow();

        void setCellBackground(const QDate &date, const QColor &color);

        void clearCellBackground(const QDate &date);

        void markNotesInMonth(const int year, const int month, const int prevMonth);

        void unlockAddNote();

        void unlockEditAndRemoveNote();

        void showPageCalendar();

        void showPageEvent();

    private slots:
        void on_pushButton_add_clicked();

        void on_pushButton_edit_clicked();

        void on_pushButton_remove_clicked();

        void on_pushButton_settings_clicked();

        void on_buttonBox_event_accepted();

        void on_buttonBox_event_rejected();

        void on_buttonBox_settings_accepted();

        void on_buttonBox_settings_rejected();

        void on_calendarWidget_clicked(const QDate &date);

        void on_calendarWidget_currentPageChanged(int year, int month);

        void on_calendarWidget_activated(const QDate &date);

        void on_pushButton_ok_clicked();

    private:
        Ui::MainWindow *ui;

        QDate selectedDate;

        QFont font;
        QString fontName;
        unsigned int fontSize;

        QSqlDatabase db;

        bool emptyRecord;

        int previousMonth;

        QColor backgroundColor;

        QList<QDate> daysWithNotes;
};

#endif // MAINWINDOW_H
