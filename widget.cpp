#include "widget.h"
#include "ui_widget.h"

QString logsName = "logs";

QSettings settings("apkholding", "kickme-app");

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->userName->setText(settings.value("lineEditUserName").toString());
    ui->filepath->setText(settings.value("fileName").toString());
}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QIcon icon = style()->standardIcon(QStyle::SP_BrowserStop);
    QIcon connIcon = style()->standardIcon(QStyle::SP_CommandLink);

    QString filepath = ui->filepath->text();
    QString username = ui->userName->text().trimmed();

    if (filepath.trimmed()==""){
        log("Error: EMPTY FILEPATH!");
        return;
    }

    if (username==""){
        log("Error: EMPTY USERNAME!");
        return;
    }

    ui->progressBar->setValue(0);
    int countLines = countLinesInFile(filepath);
    if (countLines >= 1) {
        ui->progressBar->setMaximum(countLinesInFile(filepath));
    } else {
        log("Error: EMPTY FILE!");
    }

    QStandardItemModel *modelTable = new QStandardItemModel(0, 5, this);
    QStringList horizontalHeader;
    horizontalHeader.append("SERVER");
    horizontalHeader.append("USER ID");
    horizontalHeader.append("LOGON DATE");
    horizontalHeader.append("OFF");
    horizontalHeader.append("CONN");
    modelTable->setHorizontalHeaderLabels(horizontalHeader);
    ui->tableView->setModel(modelTable);

    QStringList lines = readFile(filepath);

    for (int i = 0; i < lines.count(); ++i) {
        QStringList args;
        args << username << "/server:"+lines[i];
        QString output = runCommand("quser", args);
        // QString str;
        // str.resize(2);
        // for (int s = 0; s < 2 ; ++s) str[s] = QChar('0' + char(qrand() % ('9' - '0')));
        // output = "USERNAME          SESSIONNAME              ID  STATE  IDLE TIME LOGON TIME\nadmin                                   " + str + "  console      2+00:17  20-02-20" + str + " 9:21";

        modelTable->insertRows(modelTable->rowCount(), 1);
        modelTable->setData(modelTable->index(i, 0), lines[i]);

        QAction *action = new QAction(icon, "LOGOFF");
        QPushButton *button  = new QPushButton ("");
        button->setIcon(action->icon());
        button->setText(action->text());
        button->setEnabled(action->isEnabled());
        QObject::connect(button, &QPushButton::clicked, action, &QAction::triggered);
        connect(action, &QAction::triggered, this, [=]()->void{ term(i); });
        ui->tableView->setIndexWidget(modelTable->index(i, 3), button);
        button->setDisabled(true);

        QAction *actionConnect = new QAction(connIcon, "CONNECT");
        QPushButton *buttonConnect  = new QPushButton ("");
        buttonConnect->setIcon(actionConnect->icon());
        buttonConnect->setText(actionConnect->text());
        buttonConnect->setEnabled(actionConnect->isEnabled());
        QObject::connect(buttonConnect, &QPushButton::clicked, actionConnect, &QAction::triggered);
        connect(actionConnect, &QAction::triggered, this, [=]()->void{ conn(i); });
        ui->tableView->setIndexWidget(modelTable->index(i, 4), buttonConnect);

        QStringList outputLines = output.split("\n");
        if (outputLines.size()>=2) {
            QString date = re("\\d{2,4}[\\.-]\\d{2}[\\.-]\\d{2,4}", outputLines[1]);
            QString id = re("\\s{2,4}\\d{1,6}\\s{2,4}", outputLines[1]);

            modelTable->setData(modelTable->index(i, 1), id.trimmed());
            modelTable->setData(modelTable->index(i, 2), date);

            if (id.trimmed()!=""){
                button->setDisabled(false);
            }
        }
        ui->progressBar->setValue(i+1);
    }
}

void Widget::term(const int row)
{
    if (questionBox("Close session")) {
        QAbstractItemModel *model = ui->tableView->model();
        QString server = model->data(model->index(row, 0)).toString();
        QString id = model->data(model->index(row, 1)).toString();
        QStringList args;
        args << id << "/server:"+server;
        runCommand("Logoff", args);
        QPushButton *button = qobject_cast<QPushButton *>(ui->tableView->indexWidget(model->index(row, 3)));
        button->setDisabled(true);
    }
}

void Widget::conn(const int row)
{
    if (questionBox("Connect")) {
        QAbstractItemModel *model = ui->tableView->model();
        QString server = model->data(model->index(row, 0)).toString();
        QStringList args;
        args << "/prompt" << "/v:"+server;
        runCommand("mstsc", args);
    }
}

QListView* Widget::find(const QString value)
{
    QListView *found = nullptr;
    for (int i = 0; i < ui->horizontalLayout->count(); ++i) {
        QListView *widget = qobject_cast<QListView*>(ui->horizontalLayout->itemAt(i)->widget());
        if (widget && widget->objectName() == value) {
            found = widget;
        }
    }
    return found;
}

void Widget::log(const QString value)
{
    QListView *found = find(logsName);
    if (found) {
        found->setHidden(false);
        QStandardItemModel *model = qobject_cast<QStandardItemModel *>(found->model());
        model->appendRow(new QStandardItem(QTime::currentTime().toString("hh:mm:ss: ") + value));
    } else {
        found = new QListView();
        found->setObjectName(logsName);
        found->setModel(new QStandardItemModel());
        ui->horizontalLayout->addWidget(found);
        QStandardItemModel *model = qobject_cast<QStandardItemModel *>(found->model());
        model->appendRow(new QStandardItem(QTime::currentTime().toString("hh:mm:ss: ") + value));
    }
}

void Widget::on_userName_editingFinished()
{
    settings.setValue("lineEditUserName", ui->userName->text());
}

void Widget::on_pushButton_2_clicked()
{
    if (questionBox("Open new file")) {
        QFileDialog dialog;
        QString fileName = dialog.getOpenFileName();
        ui->filepath->setText(fileName);
        settings.setValue("fileName", ui->filepath->text());
    }
}

void Widget::on_pushButton_3_clicked()
{
    QString add = ui->adding->text().trimmed();
    QString filepath = ui->filepath->text();
    if (questionBox("Add "+add) && add != "") {
        addToFile(filepath,add);
    }
}

void Widget::on_pushButton_4_clicked()
{
    QString filepath = ui->filepath->text();
    if (questionBox("Print file to logs")) {
        QStringList lines = readFile(filepath);
        for (int i = 0; i < lines.count(); ++i) {
            log(lines[i]);
        }
    }
}

bool findInsideFile(QString filepath, QString find) {
    QString trimmedFind = find.trimmed();
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) qDebug() << "open " + filepath + " error";
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        if (stream.readLine().trimmed() == trimmedFind) {
            return true;
        }
    }
    file.close();
    return false;
}

void Widget::addToFile(QString filepath, QString text) {
    QString ext = "\n";
    if (countLinesInFile(filepath)==0) {
        ext = "";
    }
    QString trimmedText = text.trimmed();
    if (!findInsideFile(filepath,trimmedText)) {
        QFile file(filepath);
        if (file.open(QIODevice::Append)) {
            QTextStream stream(&file);
            stream << ext << trimmedText;
        }
        file.close();
    }
}

QString Widget::re(QString mask, QString text) {
    QRegExp rx(mask);
    int pos = rx.indexIn(text, 0);
    if (pos != -1) {
        return rx.cap(0);
    }
    return "";
}

QStringList Widget::readFile(QString filepath) {
    QStringList lines;
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) qDebug() << "open " + filepath + " error";
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        lines << stream.readLine().trimmed();
    }
    file.close();
    return lines;
}

int Widget::countLinesInFile(QString filepath) {
    int count=0;
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) qDebug() << "open " + filepath + " error";
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        stream.readLine();
        count++;
    }
    file.close();
    return count;
}

QString Widget::runCommand(QString cmd, QStringList args) {
    QTextCodec *codec = QTextCodec::codecForName("IBM866");
    QProcess process;
    process.start(cmd, args);
    process.waitForFinished();
    QString output = codec->toUnicode(process.readAllStandardOutput());
    QString errorOutput = codec->toUnicode(process.readAllStandardError());
    log("COMMAND: " + process.program()+" "+process.arguments().join(" "));
    log("OUTPUT : " + output+"\n"+errorOutput);
    return output;
}

bool Widget::questionBox(QString text) {
    QMessageBox::StandardButton q = QMessageBox::question(this, "<?>", text + "?", QMessageBox::Yes | QMessageBox::Cancel);
    return q == QMessageBox::Yes;
}
