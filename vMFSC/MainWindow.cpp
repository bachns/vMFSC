#include "MainWindow.h"
#include "Scanner.h"
#include "Copier.h"
#include "Vietnamese.h"
#include <QFile>
#include <QFileDialog>
#include <QThread>
#include <QDateTime>
#include <QStandardItemModel>
#include <QMessageBox>
#include <QTextStream>
#include <QProcess>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);
	progressBar->hide();
	connect(scanDirBrowserButton, &QPushButton::clicked, this, &MainWindow::selectScanDir);
	connect(copyDirBrowserButton, &QPushButton::clicked, this, &MainWindow::selectCopyDir);
	connect(addButton, &QPushButton::clicked, this, &MainWindow::add);
	connect(addFromFileButton, &QPushButton::clicked, this, &MainWindow::addFromFile);
	connect(addAllButton, &QPushButton::clicked, this, &MainWindow::addAll);
	connect(removeButton, &QPushButton::clicked, this, &MainWindow::remove);
	connect(removeAllButton, &QPushButton::clicked, this, &MainWindow::removeAll);
	connect(regExpComboBox, &QComboBox::currentTextChanged, this, &MainWindow::regExpComboBoxChanged);
	connect(suffixComboBox, &QComboBox::currentTextChanged, this, &MainWindow::suffixComboBoxChanged);
	connect(regExpLineEdit, &QLineEdit::textChanged, this, &MainWindow::regExpLineEditChanged);
	connect(resultGotoButton, &QPushButton::clicked, this, &MainWindow::resultExplorer);
	connect(copyGotoButton, &QPushButton::clicked, this, &MainWindow::copyExplorer);
	connect(scanButton, &QPushButton::clicked, this, &MainWindow::scan);
	connect(copyButton, &QPushButton::clicked, this, &MainWindow::copy);
	
	mResultModel = new QStandardItemModel(0, 3, resultTableView);
	mResultModel->setHeaderData(0, Qt::Horizontal, Vietnamese::str(L"Số hiệu"));
	mResultModel->setHeaderData(1, Qt::Horizontal, Vietnamese::str(L"Thời gian"));
	mResultModel->setHeaderData(2, Qt::Horizontal, Vietnamese::str(L"Đường dẫn"));
	resultTableView->setModel(mResultModel);
	resultTableView->setSortingEnabled(true);
	resultTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	resultTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	resultTableView->horizontalHeader()->setStretchLastSection(true);

	mCopyModel = new QStandardItemModel(0, 3, copyTableView);
	mCopyModel->setHeaderData(0, Qt::Horizontal, Vietnamese::str(L"Số hiệu"));
	mCopyModel->setHeaderData(1, Qt::Horizontal, Vietnamese::str(L"Thời gian"));
	mCopyModel->setHeaderData(2, Qt::Horizontal, Vietnamese::str(L"Đường dẫn"));
	copyTableView->setModel(mCopyModel);
	copyTableView->setSortingEnabled(true);
	copyTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
	copyTableView->setSelectionMode(QAbstractItemView::SingleSelection);
	copyTableView->horizontalHeader()->setStretchLastSection(true);

	mRegExp25 = "\\d{4}(\\s+|_+|-+)*(i{0,3}I{0,3}|1|2|3|4|iv|IV)(\\s+|_+|-+)*(DB|\\x0110B|TB|DN|\\x0110N|TN)";
	mRegExp50 = "\\d{4}(\\s+|_+|-+)*(i{0,3}I{0,3}|1|2|3|4|iv|IV)";
	mRegExp100 = "\\d{4}";
	mRegExp250 = "[A-Za-z]{2}(\\s+|_+|-+)*\\d{1,2}(\\s+|_+|-+)*\\d{1,2}";
	mRegExp500 = "[A-Za-z]{2}(\\s+|_+|-+)*\\d{1,2}(\\s+|_+|-+)*(a|A|b|B|c|C|d|D)";
	mRegExp1M = "[A-Za-z]{2}(\\s+|_+|-+)*\\d{1,2}";

	mSuffixRegExp.insert("tt.pdf", "(\\s+|_+|-+)*(TT|tt)?\\.pdf");
	mSuffixRegExp.insert("cb.pdf", "(\\s+|_+|-+)*(CB|cb)?\\.pdf");
	mSuffixRegExp.insert("cs.dgn", "(\\s+|_+|-+)*(CS|cs)?\\.dgn");
	mSuffixRegExp.insert("mdb", "\\.mdb");
	mSuffixRegExp.insert("gdb", "\\.gdb");
	mSuffixRegExp.insert(Vietnamese::str(L"Khác"), "");

	suffixComboBox->addItem("tt.pdf");
	suffixComboBox->addItem("cb.pdf");
	suffixComboBox->addItem("cs.dgn");
	suffixComboBox->addItem("mdb");
	suffixComboBox->addItem("gdb");
	suffixComboBox->addItem(Vietnamese::str(L"Khác"));
}

void MainWindow::scan()
{
	Scanner* scanner = new Scanner;
	scanner->mFolder = scanDirLineEdit->text();
	scanner->mRegExp = regExpLineEdit->text() + mSuffixRegExp.value(suffixComboBox->currentText(), "");
	scanner->mFileFilter = fileRadioButton->isChecked();
	scanner->mParentDirLevel = parentLevelSpinBox->value();

	QThread* thread = new QThread;
	scanner->moveToThread(thread);

	connect(scanner, SIGNAL(started()), this, SLOT(scannerStarted()));
	connect(scanner, SIGNAL(finished()), this, SLOT(scannerFinished()));
	connect(scanner, SIGNAL(notice(const QString&)), this, SLOT(scannerNotice(const QString&)));
	connect(scanner, SIGNAL(detection(const QString&, const QString&, const QDateTime&)),
		this, SLOT(scannerDetection(const QString&, const QString&, const QDateTime&)));

	connect(scanner, SIGNAL(finished()), thread, SLOT(quit()));
	connect(scanner, SIGNAL(finished()), scanner, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), scanner, SLOT(run()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	thread->start();
}

void MainWindow::copy()
{
	QString dstDir = copyDirLineEdit->text().trimmed();
	if (dstDir.isEmpty())
	{
		noticeLabel->setText(Vietnamese::red(L"Chưa chọn thư mục để sao chép đến"));
		return;
	}

	QList<QPair<QString, QString>> files;
	for (int r = 0; r < mCopyModel->rowCount(); ++r)
	{
		QString path = mCopyModel->item(r, 2)->text();
		QString name = QFileInfo(path).fileName();
		files.append(QPair<QString, QString>(name, path));
	}

	Copier* copier = new Copier(files, dstDir);

	QThread* thread = new QThread;
	copier->moveToThread(thread);

	connect(copier, SIGNAL(started()), this, SLOT(copierStarted()));
	connect(copier, SIGNAL(finished()), this, SLOT(copierFinished()));
	connect(copier, SIGNAL(copied(const QString&)), this, SLOT(copySuccess(const QString&)));
	connect(copier, SIGNAL(error(const QString&)), this, SLOT(copyFailed(const QString&)));
	connect(copier, SIGNAL(progress(int)), progressBar, SLOT(setValue(int)));
	connect(copier, SIGNAL(progress(const QString&)), this, SLOT(copyProgress(const QString&)));

	connect(copier, SIGNAL(finished()), thread, SLOT(quit()));
	connect(copier, SIGNAL(finished()), copier, SLOT(deleteLater()));
	connect(thread, SIGNAL(started()), copier, SLOT(run()));
	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	thread->start();
}

void MainWindow::scannerStarted()
{
	mNameDateTimesMap.clear();
	mResultRowMap.clear();
	while (mResultModel->rowCount() > 0)
		mResultModel->removeRows(0, 1);
	removeAll();
}

void MainWindow::scannerFinished()
{
	noticeLabel->setText(Vietnamese::green(L"Đã quét xong"));
	resultGroupBox->setTitle(Vietnamese::str(L"Kết quả quét [ %1 ]").arg(mResultModel->rowCount()));
}

void MainWindow::scannerNotice(const QString& message)
{
	noticeLabel->setText(message);
}

void MainWindow::scannerDetection(const QString& name, const QString& path, const QDateTime& dateTime)
{
	QString correctName = processName(name);
	if (mNameDateTimesMap.contains(correctName))
	{
		QPair<QString, QDateTime>& pair = mNameDateTimesMap[correctName];
		if (dateTime > pair.second)
		{
			pair.first = path;
			pair.second = dateTime;
			int row = mResultRowMap[correctName];
			mResultModel->setItem(row, 1, new QStandardItem(dateTime.toString("dd/MM/yyyy hh:mm:ss")));
			mResultModel->setItem(row, 2, new QStandardItem(path));
		}
	}
	else
	{
		mNameDateTimesMap.insert(correctName, QPair<QString, QDateTime>(path, dateTime));
		int row = mResultModel->rowCount();
		mResultModel->insertRow(row);
		mResultModel->setItem(row, 0, new QStandardItem(correctName));
		mResultModel->setItem(row, 1, new QStandardItem(dateTime.toString("dd/MM/yyyy hh:mm:ss")));
		mResultModel->setItem(row, 2, new QStandardItem(path));
		mResultRowMap.insert(correctName, row);
		resultGroupBox->setTitle(Vietnamese::str(L"Kết quả quét [ %1 ]").arg(mResultModel->rowCount()));
	}
}

void MainWindow::copierStarted()
{
	progressBar->show();
	progressBar->setValue(0);
	plainTextEdit->clear();
	plainTextEdit->appendPlainText(Vietnamese::str(L"Bắt đầu sao chép"));
}

void MainWindow::copierFinished()
{
	progressBar->hide();
	noticeLabel->setText(Vietnamese::green(L"Đã sao chép xong"));
	plainTextEdit->appendPlainText(Vietnamese::str(L"Hoàn thành sao chép"));
}

void MainWindow::copySuccess(const QString& name)
{
	plainTextEdit->appendPlainText("   OK    | " + name);
}

void MainWindow::copyFailed(const QString& name)
{
	plainTextEdit->appendPlainText(" FAILED  | " + name);
}

void MainWindow::copyProgress(const QString& path)
{
	noticeLabel->setText(Vietnamese::str(L"Đang sao chép: %1").arg(path));
}

void MainWindow::selectScanDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, Vietnamese::str(L"Chọn thư mục cần quét"));
	if (!dir.isEmpty())
	{
		scanDirLineEdit->setText(dir);
	}
}

void MainWindow::selectCopyDir()
{
	QString dir = QFileDialog::getExistingDirectory(this, Vietnamese::str(L"Chọn thư mục chứa các tệp tin sao chép"));
	if (!dir.isEmpty())
	{
		copyDirLineEdit->setText(dir);
	}
}

void MainWindow::add()
{
	QModelIndex index = resultTableView->currentIndex();
	if (!index.isValid())
		return;

	int row = index.row();
	const QString name = mResultModel->item(row, 0)->text();
	if (mCopyNameMap[name] == true)
		return;

	mCopyNameMap.insert(name, true);
	int count = mCopyModel->rowCount();
	mCopyModel->insertRow(count);
	mCopyModel->setItem(count, 0, mResultModel->item(row, 0)->clone());
	mCopyModel->setItem(count, 1, mResultModel->item(row, 1)->clone());
	mCopyModel->setItem(count, 2, mResultModel->item(row, 2)->clone());
	copyGroupBox->setTitle(Vietnamese::str(L"Kết quả chọn [ %1 ]").arg(mCopyModel->rowCount()));
}

void MainWindow::addFromFile()
{
	QString fileName = QFileDialog::getOpenFileName(this,
		Vietnamese::str(L"Chọn file chứa số hiệu mảnh"), QString(), "Text (*.txt)");
	if (!fileName.isEmpty())
	{
		QFile file(fileName);
		if (file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			plainTextEdit->clear();
			plainTextEdit->appendPlainText(Vietnamese::str(L"Bắt đầu chọn từ file"));
			QTextStream stream(&file);
			while (stream.atEnd() == false)
			{
				QString name = stream.readLine().trimmed();
				addNameCopy(name);
			}
			file.close();
			plainTextEdit->appendPlainText(Vietnamese::str(L"Kết thúc chọn từ file"));
			copyGroupBox->setTitle(Vietnamese::str(L"Kết quả chọn [ %1 ]").arg(mCopyModel->rowCount()));
		}
	}
}

void MainWindow::addAll()
{
	removeAll();
	for (int i = 0; i < mResultModel->rowCount(); i++)
	{
		int count = mCopyModel->rowCount();
		mCopyModel->insertRow(count);
		mCopyModel->setItem(count, 0, mResultModel->item(i, 0)->clone());
		mCopyModel->setItem(count, 1, mResultModel->item(i, 1)->clone());
		mCopyModel->setItem(count, 2, mResultModel->item(i, 2)->clone());
		mCopyNameMap.insert(mResultModel->item(i, 0)->text(), true);
	}
	copyGroupBox->setTitle(Vietnamese::str(L"Kết quả chọn [ %1 ]").arg(mCopyModel->rowCount()));
}

void MainWindow::remove()
{
	QModelIndex index = copyTableView->currentIndex();
	if (!index.isValid())
		return;

	int row = index.row();
	mCopyNameMap.remove(mCopyModel->item(row, 0)->text());
	mCopyModel->takeRow(row);
	copyGroupBox->setTitle(Vietnamese::str(L"Kết quả chọn [ %1 ]").arg(mCopyModel->rowCount()));
}

void MainWindow::removeAll()
{
	mCopyNameMap.clear();
	while (mCopyModel->rowCount() > 0)
		mCopyModel->removeRows(0, 1);
	copyGroupBox->setTitle(Vietnamese::str(L"Kết quả chọn [ %1 ]").arg(mCopyModel->rowCount()));
}

void MainWindow::regExpComboBoxChanged(const QString& text)
{
	mUserFilter = text.compare("User", Qt::CaseInsensitive) == 0;

	if (text.compare("25K", Qt::CaseInsensitive) == 0)
	{
		regExpLineEdit->setText(mRegExp25);
	}

	else if (text.compare("50K", Qt::CaseInsensitive) == 0)
	{
		regExpLineEdit->setText(mRegExp50);
	}

	else if (text.compare("100K", Qt::CaseInsensitive) == 0)
	{
		regExpLineEdit->setText(mRegExp100);
	}

	else if (text.compare("250K", Qt::CaseInsensitive) == 0)
	{
		regExpLineEdit->setText(mRegExp250);
	}

	else if (text.compare("500K", Qt::CaseInsensitive) == 0)
	{
		regExpLineEdit->setText(mRegExp500);
	}

	else if (text.compare("1M", Qt::CaseInsensitive) == 0)
	{
		regExpLineEdit->setText(mRegExp1M);
	}
}

void MainWindow::regExpLineEditChanged(const QString& text)
{
	if (text == mRegExp25)
		regExpComboBox->setCurrentIndex(0);
	else if (text == mRegExp50)
		regExpComboBox->setCurrentIndex(1);
	else if (text == mRegExp100)
		regExpComboBox->setCurrentIndex(2);
	else if (text == mRegExp250)
		regExpComboBox->setCurrentIndex(3);
	else if (text == mRegExp500)
		regExpComboBox->setCurrentIndex(4);
	else if (text == mRegExp1M)
		regExpComboBox->setCurrentIndex(5);
	else
		regExpComboBox->setCurrentIndex(6);
}

void MainWindow::suffixComboBoxChanged(const QString& text)
{
	if (text == "gdb")
	{
		folderRadioButton->setChecked(true);
	}
	else
	{
		fileRadioButton->setChecked(true);
	}
}

void MainWindow::resultExplorer()
{
	auto index = resultTableView->currentIndex();
	if (index.isValid())
	{
		auto text = mResultModel->item(index.row(), 2)->text();
		explorer(text);
	}
}

void MainWindow::copyExplorer()
{
	auto index = copyTableView->currentIndex();
	if (index.isValid())
	{
		auto text = mCopyModel->item(index.row(), 2)->text();
		explorer(text);
	}
}

QString MainWindow::processName(const QString& name)
{
	if (mUserFilter)
		return name;
		
	static QRegExp re("\\s+|_+|-+");
	QStringList parts = name.toUpper().split(re);
	if (parts.last() == "TT")
		parts.removeLast();

	//Các tỉ lệ từ 250K đến 1M
	if (regExpComboBox->currentIndex() >= 3)
		return parts.join("-");

	//từ 25k đến 100k
	QStringList result;
	static QMap<QString, QString> m50{
		{"I", "I"}, {"1", "I"},
		{"II", "II"}, {"2", "II"},
		{"III", "III"}, {"3", "III"},
		{"IV", "IV"}, {"4", "IV"} };

	static QMap<QString, QString> m25{
		{"DB", "DB"}, {Vietnamese::str(L"ĐB"), "DB"},
		{"DN", "DN"}, {Vietnamese::str(L"ĐN"), "DN"},
		{"TB", "TB"}, {"TN", "TN"} };

	if (parts.size() >= 1)
		result << parts.at(0);

	if (parts.size() >= 2)
	{
		const QString& str = parts.at(1);
		if (m50.contains(str))
			result << m50[str];
		else
		{
			//lấy 2 kí tự cuối
			const QString& last = str.right(2);
			const QString& first = str.left(str.length() - 2);
			if (m50.contains(first))
			{
				result << m50[first];
				if (m25.contains(last))
				{
					result << m25[last];
				}
			}
		}
	}
	if (parts.size() >= 3 && result.size() == 2)
	{
		const QString& str = parts.at(2);
		if (m25.contains(str))
		{
			result << m25[str];
		}
	}
	return result.join("-");
	
}

void MainWindow::addNameCopy(const QString& name)
{
	if (mResultRowMap.contains(name))
	{
		int row = mResultRowMap[name];
		if (mCopyNameMap[name] == true)
			return;
		plainTextEdit->appendPlainText("   CO    | " + name);

		mCopyNameMap.insert(name, true);
		int count = mCopyModel->rowCount();
		mCopyModel->insertRow(count);
		mCopyModel->setItem(count, 0, mResultModel->item(row, 0)->clone());
		mCopyModel->setItem(count, 1, mResultModel->item(row, 1)->clone());
		mCopyModel->setItem(count, 2, mResultModel->item(row, 2)->clone());
	}
	else
	{
		plainTextEdit->appendPlainText("KHONG_CO | " + name);
	}
}

void MainWindow::explorer(const QString& fileName)
{
	const QFileInfo fileInfo(fileName);
	QStringList param;
	if (!fileInfo.isDir())
		param += QLatin1String("/select,");
	param += QDir::toNativeSeparators(fileInfo.canonicalFilePath());
	QProcess::startDetached("explorer.exe", param);
}