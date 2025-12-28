#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QPushButton>
#include <QMessageBox>
#include <QList>
#include <QPair>
#include <QPoint>
#include <QGraphicsDropShadowEffect>
#include <QLabel>
#include <QLayout>
#include <QWidget>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QDebug>
#include <QProcess>
#include <QTcpSocket>
#include <QTimer>
#include <QSqlRecord>
#include <QRegularExpression>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVector>
#include <QObject>
#include <QMap>
#include <QInputDialog>
#include <QQueue>


QString LastQuery;
bool UseLastQuery = false;
bool FastResgionSelectMode = false;
QString LastBaseTable;
QStringList LastListOfTables;
QString LastCondition;
QString LastColumns;


QMap<QPair<QString, QString>, QString> tableRelationships = {
    { qMakePair(QString("Villes"),     QString("Régions")),      "Id_Région" },
    { qMakePair(QString("Gaz"),        QString("Type_Gaz")),     "Id_Type_Gaz" },
    { qMakePair(QString("Villes"),     QString("Agences")),      "Id_Ville" },
    { qMakePair(QString("Villes"),     QString("Capteurs")),     "Id_Ville" },
    { qMakePair(QString("Capteurs"),   QString("Gaz")),          "Id_Gaz" },
    { qMakePair(QString("Gaz"),        QString("Type_Gaz")),     "Id_Type_Gaz" },
    { qMakePair(QString("Capteurs"),   QString("Données")),      "Id_Capteur" },
    { qMakePair(QString("Données"),    QString("Rapports")),     "Id_Rapport" },
    { qMakePair(QString("Capteurs"),   QString("Techniques")),   "Id_Personnel" },
    { qMakePair(QString("Rapports"),   QString("Admins")),       "Id_Personnel" },
    { qMakePair(QString("Agences"),    QString("Chefs")),        "Id_Agence" },
    { qMakePair(QString("Agences"),    QString("Personnel")),    "Id_Agence" },
    { qMakePair(QString("Chefs"),      QString("Personnel")),    "Id_Personnel" },
    { qMakePair(QString("Techniques"), QString("Personnel")),    "Id_Personnel" },
    { qMakePair(QString("Admins"),     QString("Personnel")),    "Id_Personnel" }
};



bool Connected = false;
QString RegionTables = "Villes,Capteurs,Gaz,Type_Gaz,Agences";
QString CapteursTables = "Gaz,Type_Gaz,Données,Villes";


// Apply a small shadow to all QLabel widgets
void applyShadowToAllLabels(QWidget *widget) {
    if (!widget) return;

    QLabel *label = qobject_cast<QLabel *>(widget);
    if (label) {
        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(label);
        shadow->setBlurRadius(0);
        shadow->setOffset(2, 2);
        shadow->setColor(QColor(67, 85, 112));
        label->setGraphicsEffect(shadow);
    }

    const auto children = widget->findChildren<QWidget *>();
    for (QWidget *child : children) {
        applyShadowToAllLabels(child);
    }
}

// Apply a custom shadow to any widget
void applyShadowIndiv(QWidget *widget, float BlurRadius, float Offset) {
    if (!widget) return;

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(widget);
    shadow->setBlurRadius(BlurRadius);
    shadow->setOffset(Offset, Offset);
    shadow->setColor(Qt::black);
    widget->setGraphicsEffect(shadow);
}

// Database connection function
void ConnectToDB() {
    if (QSqlDatabase::contains("qt_sql_default_connection"))
        QSqlDatabase::removeDatabase("qt_sql_default_connection");

    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setPort(3306);
    db.setDatabaseName("cleargaz");
    db.setUserName("root");
    db.setPassword("@2007");
    db.setConnectOptions("MYSQL_OPT_SSL_MODE=SSL_MODE_DISABLED");

    if (!db.open()) {
        qDebug() << "❌ Database Error:" << db.lastError().text();
        Connected = false;
    } else {
        qDebug() << "✅ Database connected successfully!";
        Connected = true;
    }
}

// Update the stats (this function needs the ui pointer)
void MainWindow::UpdateStats() {
    if (Connected) {
        QList<QPair<QString, QLabel*>> Elements = {
            {"Personnel", ui->Npers},
            {"ChefS", ui->Nchefs},
            {"Admins",ui->Nadmins},
            {"Techniques",ui->Ntechs},
            {"Agences", ui->Nagences},
            {"Capteurs",ui->SensD}
        };

        for (const auto &data : Elements) {
            QSqlQuery query;
            QString queryString = QString("SELECT COUNT(*) FROM %1").arg(data.first);

            if (!query.exec(queryString)) {
                qDebug() << "Query error:" << query.lastError().text();
            }

            if (query.next()) {
                QString Count = query.value(0).toString();
                data.second->setText(Count);
            }
        }
    }
}

// Check for socket data and update the GUI
void MainWindow::checkSocket() {
    if (!socket) return;

    if (socket->canReadLine()) {
        QByteArray line = socket->readLine();
        qDebug() << "Socket received:" << line.trimmed();
        if (line.trimmed() == "DB_CHANGED") {
            UpdateStats();
        }
    }
}

// Handle socket errors
void MainWindow::handleSocketError(QTcpSocket::SocketError error) {
    qDebug() << "Socket error:" << error;

}

QPair<QStringList, QStringList> MainWindow::GetColCheckBox() {
    QStringList checkedItems;
    QStringList uncheckedItems;

    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->ColChoose->model());

    for (int row = 0; row < model->rowCount(); ++row) {
        QStandardItem* item = model->item(row);
        if (item->checkState() == Qt::Checked) {
            checkedItems << item->text();
        } else {
            uncheckedItems << item->text();
        }
    }
    return qMakePair(checkedItems,uncheckedItems);

}

void MainWindow::SetColCheckBox(bool State,QStringList Name){
    QStandardItemModel* model = qobject_cast<QStandardItemModel*>(ui->ColChoose->model());
    qDebug()<<"Called";
    for (int row = 0; row < model->rowCount(); ++row) {
        for (int raw = 0;raw < Name.size(); ++raw){
            QStandardItem* item = model->item(row);
            if (item->text() == Name[raw]) {
                if (State){
                    item->setCheckState(Qt::Checked); // or Qt::Unchecked

                }
                else {
                    item->setCheckState(Qt::Unchecked);
                }

                break;
            }
        }

    }
}

struct TableCellSnapshot {
    QString text;
    QString widgetType;   // e.g., "button", "label", etc.
    QString widgetValue;  // e.g., button text
    QString Tables;
};
using TableSnapshot = QVector<QVector<TableCellSnapshot>>;

TableSnapshot snapshotTable(QTableWidget *table) {
    TableSnapshot snapshot;
    for (int row = 0; row < table->rowCount(); ++row) {
        QVector<TableCellSnapshot> rowSnapshot;
        for (int col = 0; col < table->columnCount(); ++col) {
            TableCellSnapshot cell;

            if (auto item = table->item(row, col)) {
                cell.text = item->text();
            }

            if (auto widget = table->cellWidget(row, col)) {
                if (auto btn = qobject_cast<QPushButton*>(widget)) {
                    cell.widgetType = "button";
                    cell.widgetValue = btn->text();
                    cell.Tables = btn->property("Tables").toString();
                }
                if (auto lbl = qobject_cast<QLabel*>(widget)) {
                    cell.widgetType = "label";
                    cell.widgetValue = lbl->text();

                }
            }

            rowSnapshot.append(cell);
        }
        snapshot.append(rowSnapshot);
    }
    return snapshot;
}


QStringList parseWords(const QString& input) {
    return input.split(',', Qt::SkipEmptyParts);
}


void restoreSnapshot(QTableWidget *table, const TableSnapshot &snapshot,MainWindow *mainWin) {
    table->clearContents();
    table->setRowCount(snapshot.size());
    if (!snapshot.isEmpty())
        table->setColumnCount(snapshot[0].size());

    for (int row = 0; row < snapshot.size(); ++row) {
        for (int col = 0; col < snapshot[row].size(); ++col) {
            const auto &cell = snapshot[row][col];

            if (!cell.text.isEmpty()) {
                qDebug()<<"hello";
                auto *item = new QTableWidgetItem(cell.text);
                table->setItem(row, col, item);
            }

            if (cell.widgetType == "button") {
                auto *btn = new QPushButton(cell.widgetValue);
                btn->setStyleSheet("background-color: #eee; font-weight: bold;");
                table->setCellWidget(row, col, btn);

                // Capture 'this' explicitly in the lambda
                if (cell.Tables == "Villes NATURAL JOIN Régions") {
                    QObject::connect(btn, &QPushButton::clicked, mainWin, [=]() {

                        QString Condition = "Villes.Nom_Ville = '" + btn->text() + "'";  // Use btn->text() for the button value
                        mainWin->PopulateTable("Villes",QStringList()<<"Capteurs"<<"Gaz"<<"Type_Gaz"<<"Agences",Condition,"Nom_Ville,Nom_Capteur,Etat_Capteur,Nom_Gaz,Type,Nom_Agence,Adresse_Agence");
                        mainWin->SetColCheckBox(true,QStringList()<<"Villes"<<"Capteurs"<<"Gaz"<<"Type_Gaz"<<"Agences");

                    });
                }
                else if (cell.Tables == CapteursTables){
                    QObject::connect(btn, &QPushButton::clicked, mainWin, [=]() {

                        QString Condition =  "Nom_Capteur = '" + btn->text() + "'";  // Use btn->text() for the button value
                        QStringList tables = parseWords(cell.Tables);
                        mainWin->PopulateTable("Capteurs",tables,Condition,"");
                        mainWin->SetColCheckBox(true,QStringList()<<"Capteurs"<<"Gaz"<<"Type_Gaz"<<"Données");

                    });
                }


            }
            else if (cell.widgetType == "label"){

                QLabel *Label = new QLabel(cell.widgetValue);

                table->setCellWidget(row, col, Label);
            }
        }
    }
}


QVector<TableSnapshot> undoStack;
QVector<TableSnapshot> redoStack;

void SaveState(QTableWidget *table) {
    undoStack.append(snapshotTable(table));
    redoStack.clear(); // reset redo history on new action
}

void undo(QTableWidget *table,MainWindow *mainWin) {
    if (undoStack.size() <= 1) return;
    auto current = undoStack.takeLast();
    redoStack.append(current);
    restoreSnapshot(table, undoStack.last(),mainWin);
}

void redo(QTableWidget *table,MainWindow *mainWin) {
    if (redoStack.isEmpty()) return;
    auto state = redoStack.takeLast();
    undoStack.append(state);
    restoreSnapshot(table, state,mainWin);
}



void MainWindow::PopulateTable(QString BaseTable, QStringList ListOfTablesWanted, QString Condition, QString Columns) {
    qDebug() << "PopulateTable called!";
    ui->table->clear();  // Full clear including headers and filters
    ui->table->setRowCount(0);
    headerFilters.clear();  // Reset old filter widgets

    QString finalBaseTable = (UseLastQuery && !LastQuery.isEmpty() && BaseTable.isEmpty()) ? LastBaseTable : BaseTable;
    QStringList finalTargetTables = (UseLastQuery && !LastQuery.isEmpty() && ListOfTablesWanted.isEmpty()) ? LastListOfTables : ListOfTablesWanted;
    QString finalColumns = (UseLastQuery && !LastQuery.isEmpty() && Columns.isEmpty()) ? LastColumns : (Columns.isEmpty() ? "*" : Columns);
    QString finalCondition = (UseLastQuery && !LastQuery.isEmpty() && Condition.isEmpty()) ? LastCondition : Condition;

    // Reconstruct join paths
    QMap<QString, QList<QStringList>> allPaths;
    QQueue<QStringList> queue;
    queue.enqueue({finalBaseTable});

    while (!queue.isEmpty()) {
        QStringList path = queue.dequeue();
        QString current = path.last();

        for (auto it = tableRelationships.begin(); it != tableRelationships.end(); ++it) {
            QString a = it.key().first;
            QString b = it.key().second;

            QString neighbor;
            if (a == current) neighbor = b;
            else if (b == current) neighbor = a;
            else continue;

            if (path.contains(neighbor)) continue;

            QStringList newPath = path;
            newPath.append(neighbor);
            allPaths[neighbor].append(newPath);
            queue.enqueue(newPath);
        }
    }

    QSet<QString> allJoinedTables;
    QStringList joinClauses;

    for (const QString& target : finalTargetTables) {
        if (!allPaths.contains(target)) continue;

        QStringList chosenPath;
        QList<QStringList> paths = allPaths[target];

        if (paths.size() == 1) {
            chosenPath = paths.first();
        } else {
            // Prompt user to choose a path
            QStringList pathOptions;
            for (const QStringList& p : paths) {
                pathOptions << p.join(" → ");
            }

            bool ok;
            QString selected = QInputDialog::getItem(this, "Select Join Path",
                                                     "Multiple join paths found from " + finalBaseTable + " to " + target + ":\n\nChoose one:",
                                                     pathOptions, 0, false, &ok);
            if (!ok || selected.isEmpty()) return;

            for (const QStringList& p : paths) {
                if (p.join(" → ") == selected) {
                    chosenPath = p;
                    break;
                }
            }
        }

        for (int i = 1; i < chosenPath.size(); ++i) {
            QString from = chosenPath[i - 1];
            QString to = chosenPath[i];
            QString joinKey;

            QPair<QString, QString> pair1(from, to);
            QPair<QString, QString> pair2(to, from);

            if (tableRelationships.contains(pair1)) joinKey = tableRelationships[pair1];
            else if (tableRelationships.contains(pair2)) joinKey = tableRelationships[pair2];

            QString joinClause = "LEFT JOIN " + to + " ON " + from + "." + joinKey + " = " + to + "." + joinKey;
            if (!allJoinedTables.contains(to)) {
                joinClauses.append(joinClause);
                allJoinedTables.insert(to);
            }
        }
    }

    QString queryStr = "SELECT " + finalColumns + " FROM " + finalBaseTable;
    if (!joinClauses.isEmpty()) queryStr += " " + joinClauses.join(" ");
    if (!finalCondition.isEmpty()) queryStr += " WHERE " + finalCondition;
    queryStr += ";";

    qDebug() << "Final Query:" << queryStr;

    LastQuery = queryStr;
    LastBaseTable = finalBaseTable;
    LastListOfTables = finalTargetTables;
    LastColumns = finalColumns;
    LastCondition = finalCondition;

    QSqlQuery query(queryStr);
    QSqlRecord record = query.record();

    QList<int> visibleColumnIndexes;
    QStringList visibleHeaders;

    for (int i = 0; i < record.count(); ++i) {
        QString fieldName = record.fieldName(i);
        if (!fieldName.contains("Id", Qt::CaseInsensitive)) {
            visibleColumnIndexes.append(i);
            visibleHeaders.append(fieldName);
        }
    }

    int colCount = visibleColumnIndexes.size();
    ui->table->setColumnCount(colCount);
    ui->table->setRowCount(1); // row 0 will hold filters
    ui->table->setHorizontalHeaderLabels(visibleHeaders);

    // Insert filter widgets into row 0
    for (int i = 0; i < colCount; ++i) {
        QLineEdit *filterEdit = new QLineEdit;
        filterEdit->setPlaceholderText("Filter...");
        filterEdit->setStyleSheet(
            "QLineEdit {"
            "  background-color: white;"
            "  color: black;"
            "  border: 1px solid gray;"
            "  padding: 2px;"
            "}"
            );
        headerFilters.append(filterEdit);
        ui->table->setCellWidget(0, i, filterEdit);

        connect(filterEdit, &QLineEdit::returnPressed, this, [=]() {
            QStringList conditions;
            for (int j = 0; j < headerFilters.size(); ++j) {
                QString input = headerFilters[j]->text().trimmed();
                QString colName = visibleHeaders[j];
                if (!input.isEmpty()) {
                    if (input.contains("(", Qt::CaseInsensitive) || input.contains(")") || input.contains(" BETWEEN ", Qt::CaseInsensitive)) {
                        conditions << input;
                    } else if (input.startsWith("=") || input.startsWith(">") || input.startsWith("<") || input.startsWith("!=")) {
                        conditions << colName + " " + input;
                    } else {
                        conditions << colName + " LIKE '%" + input.replace("'", "''") + "%'";
                    }
                }
            }
            QString combinedCond = conditions.join(" AND ");
            PopulateTable(finalBaseTable, finalTargetTables, combinedCond, finalColumns);
        });
    }

    // Insert data starting from row 1
    int row = 1;
    while (query.next()) {
        ui->table->insertRow(row);
        for (int vi = 0; vi < visibleColumnIndexes.size(); ++vi) {
            int col = visibleColumnIndexes[vi];
            QString value = query.value(col).toString();
            QLabel *label = new QLabel(value);
            QFont font = label->font();
            //font.setPointSize(12); // Adjust the number as needed
            //label->setFont(font);

            ui->table->setCellWidget(row, vi, label);
        }
        ++row;
    }

    FastResgionSelectMode = false;
    SaveState(ui->table);
}








MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , socket(new QTcpSocket(this)) // Initialize the socket
{
    ui->setupUi(this);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(0);
    shadow->setColor(QColor(67, 85, 112));
    shadow->setOffset(0.75, 0.75);

    QList<QPair<QString, QPoint>> buttonData = {
                                                {"Hauts-de-\nFrance", QPoint(330, 50)},
                                                {"Ile-de-\nFrance", QPoint(320, 140)},
                                                {"Centre-Val\nde Loire", QPoint(290, 210)},
                                                {"Nouvelle\nAquitaine", QPoint(250, 320)},
                                                {"Auvergne-\nRhone-\nAlpes", QPoint(450, 330)},
                                                {"Provence-\nAlphes-\nCote d'azur", QPoint(460, 400)},
                                                {"Occitanie", QPoint(260, 410)},
                                                {"Bourgogne-\nFranche-\nComté", QPoint(400, 200)},
                                                {"Pays de\nla Loire", QPoint(170, 190)},
                                                {"Bretagne", QPoint(100, 150)},
                                                {"Normandie", QPoint(200, 110)},
                                                {"Grand-Est", QPoint(450, 120)},
                                                };


    QLabel *selectedRegion = ui->SelectedRegion;


    for (int i = 0; i < buttonData.size(); ++i) {
        QString buttonName = buttonData[i].first;
        QPoint position = buttonData[i].second;

        QPushButton *button = new QPushButton(buttonName, ui->MapContainer_2);
        button->setGeometry(position.x(), position.y(), 63, 63);
        button->setStyleSheet("background-color: rgba(255, 0, 0, 150);"
                              "border-radius: 31px;"
                              "color: white;"
                              "font-weight: bold;"
                              "font-size: 8pt;");
        button->setCursor(Qt::PointingHandCursor);
        applyShadowIndiv(button, 20, 3);

        connect(button, &QPushButton::clicked, this, [this, selectedRegion, buttonName]() {
            selectedRegion->setText(buttonName);
            static const QRegularExpression onlyLetters("[^a-zA-Z]");  // compiled once

            // Normalize to decompose accents
            QString cleaned = buttonName.normalized(QString::NormalizationForm_D);

            // Remove non-letter characters
            cleaned.remove(onlyLetters);

            // Return first 3 uppercase characters
            QString KEY = cleaned.left(3).toUpper();

            QString Condition = "UPPER(LEFT(Régions.Nom_Région, 3)) = '" + KEY + "'";
            FastResgionSelectMode = true;
            PopulateTable("Villes",QStringList()<<"Régions",Condition,"Nom_Ville");
        });
    }
    //SETUP SELECT BUTTON
    connect(ui->SELECT, &QPushButton::clicked, this, [this]() {
        QPair<QStringList,QStringList> CheckBoxState = GetColCheckBox();
        QStringList Enabled = CheckBoxState.first;
        //QString TableRequest;
        if (!Enabled.isEmpty()) {

            PopulateTable(ui->BaseTable->currentText(),Enabled,"","");

        }


    });
    //SETUP UNDO/REDO
    connect(ui->undo, &QPushButton::clicked, this, [this]() {
        undo(ui->table,this);
    });
    connect(ui->redo, &QPushButton::clicked, this, [this]() {
        redo(ui->table,this);
    });

    // COLLUMN CHECKBOXES
    // Create a model for the combo box
    QStandardItemModel* model = new QStandardItemModel(this);

    // Example labels (can come from a map, table, etc.)
    QStringList gasLabels = {"Agences", "Admins", "Capteurs", "Chefs","Données","Gaz","Personnel","Régions","Rapports","Techniques","Type_Gaz","Villes"};

    // Add each label with a checkbox
    for (const QString& label : gasLabels) {
        QStandardItem* item = new QStandardItem(label);
        item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled); // Enable checkbox
        item->setData(Qt::Unchecked, Qt::CheckStateRole);            // Start unchecked
        model->appendRow(item);
    }

    // Use this model in the combo box
    ui->ColChoose->setModel(model);
    ui->ColChoose->setEditable(true); // So we can write custom text
    ui->ColChoose->lineEdit()->setReadOnly(true); // Prevent user typing
    ui->ColChoose->lineEdit()->setPlaceholderText("Select gases");


    applyShadowToAllLabels(ui->Ginfo->parentWidget());

    ConnectToDB();
    UpdateStats();

    // Connect to the Python socket server
    socket->connectToHost("127.0.0.1", 65432);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::checkSocket);
    connect(socket, &QTcpSocket::errorOccurred, this, &MainWindow::handleSocketError);

    // Start Python DB watcher process
    QProcess *pythonWatcher = new QProcess(this);
    QString scriptPath = "C:/Users/NITRO/Documents/ClearDATAsoftware/ClearGAZ/db_watcher.py";
    pythonWatcher->start("python", QStringList() << scriptPath);

    // Log output from the Python process
    connect(pythonWatcher, &QProcess::readyReadStandardError, [pythonWatcher]() {
        qDebug() << "Python STDERR:" << pythonWatcher->readAllStandardError();
    });
    connect(pythonWatcher, &QProcess::readyReadStandardOutput, [pythonWatcher]() {
        qDebug() << "Python STDOUT:" << pythonWatcher->readAllStandardOutput();
    });

    // Use a timer to periodically check the socket
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::checkSocket);
    timer->start(500); // check every 500 ms
}

MainWindow::~MainWindow() {
    if (socket) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }

    delete ui;
}
