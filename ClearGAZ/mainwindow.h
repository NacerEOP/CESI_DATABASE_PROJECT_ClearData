#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>   // Include QTcpSocket for socket handling
#include <QTimer>// Include QTimer for periodic checks
#include <QList>
#include <QLineEdit>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void PopulateTable(QString BaseTable, QStringList ListOfTablesWanted, QString Condition, QString Columns);
    void SetColCheckBox(bool State,QStringList Name);

private slots:
    void checkSocket();   // Declaration of the checkSocket method
    void handleSocketError(QTcpSocket::SocketError error);   // Declaration of the error handler method

    void UpdateStats();

    QPair<QStringList, QStringList> GetColCheckBox();


private:
    Ui::MainWindow *ui;
    QTcpSocket *socket;  // Declare the socket as a class member to use it in other methods
    QList<QLineEdit*> headerFilters;
};

#endif // MAINWINDOW_H
