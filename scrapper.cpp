//
// Created by vbbe on 2/8/2026.
//

#include "scrapper.h"


Scrapper::Scrapper(QUrl const edusUrl, QUrl const gradeUrl, QString user, QString password)
{
    USERNAME = user;
    PASSWORD = password;

    manager = new QNetworkAccessManager(this);

    cookieJar = new QNetworkCookieJar(this);

    manager->setCookieJar(cookieJar);

    mainUrl = edusUrl;
    gradesFilePageUrl = gradeUrl;

    pageTable.insert("Login - Edus", loginPage);
    pageTable.insert("Pagina principala - Edus", mainPage);
    pageTable.insert("Situatie scolara - Edus", gradesPage);
    pageTable.insert("Fisa progres - Edus", gradesFilePage);
}
void Scrapper::getToken(QString reply)
{
    QRegularExpression re("name=\"csrfmiddlewaretoken\" value=\"([^\"]+)\"");
    QRegularExpressionMatch match = re.match(reply);

    if(match.hasMatch())
    {
        token = match.captured(1);
        qDebug() << token;
    }
    else
    {
        qDebug() << "Can't find CSRF token";
        qDebug() << reply;
        token = nullptr;
    }
}
Scrapper::pageTypes Scrapper::handleReply(QNetworkReply* unFormattedReply)
{
    QString errorString = unFormattedReply->readAll();
    const QString reply = errorString;

    QRegularExpression re("<div id=\"summary\">(.*?)</div>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch match = re.match(errorString);

    uint16_t const errorCode = unFormattedReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    switch (errorCode)
    {
    case 200:
        qDebug() << "Server reached succefully";
        break;
    case 403:
        errorString.clear();
        errorString = match.captured(1);
        errorString.remove(QRegularExpression("<[^>]*>"));
        errorString = errorString.simplified();
        qDebug() << "Server reach failed with error" << errorString;
        break;
    default:
        qDebug() << "Unexpected error";
        break;
    };

//==============================================================================================================

    getToken(reply);

    re = QRegularExpression("<title>(.*?)</title>");
    match = re.match(reply);
    return pageTable.value(match.captured());
}
void Scrapper::login(QString const& username, QString const& password, const QNetworkReply* reply)
{
    QUrlQuery postData;
    postData.addQueryItem("csrfmiddlewaretoken", token);
    postData.addQueryItem("username", username);
    postData.addQueryItem("password", password);

    QByteArray const data = postData.toString(QUrl::FullyEncoded).toUtf8();

    QNetworkRequest loginRequest(QUrl("https://app.edus.ro/cont/login/"));
    loginRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");
    loginRequest.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");
    loginRequest.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8");
    loginRequest.setRawHeader("Referer", reply->url().toString().toUtf8());
    loginRequest.setRawHeader("Origin", "https://app.edus.ro");

    QNetworkReply* loginReply = manager->post(loginRequest, data);

    QEventLoop loop;
    connect (loginReply, &QNetworkReply::finished, &loop ,  &QEventLoop::quit);
    loop.exec();

    QString errorString = loginReply->readAll();

    QRegularExpression const re("<div id=\"summary\">(.*?)</div>", QRegularExpression::DotMatchesEverythingOption);
    QRegularExpressionMatch const match = re.match(errorString);

    uint16_t errorCode = loginReply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    switch (errorCode)
    {
    case 200:
        qDebug() << "Login executed succefully";
        break;
    case 401:
        errorString.clear();
        errorString = match.captured(1);
        errorString.remove(QRegularExpression("<[^>]*>"));
        errorString = errorString.simplified();
        qDebug() << "Login failed with error" << errorString;
        break;
    case 403:
        errorString.clear();
        errorString = match.captured(1);
        errorString.remove(QRegularExpression("<[^>]*>"));
        errorString = errorString.simplified();
        qDebug() << "Login forbidden with error" << errorString;
        break;
    default:
        qDebug() << "Unexpected error";
        break;
    };
}
QStringList Scrapper::gradeSeparator(QString const& unFormattedGrade)
{
    QString formattedGrade = unFormattedGrade.trimmed();
    return formattedGrade.split('/');
}
QDate Scrapper::formatDate(QString unFormattedDate)
{
    unFormattedDate = unFormattedDate.trimmed();
    unFormattedDate.replace('/', ' ');
    unFormattedDate.replace('\\', ' ');
    unFormattedDate.replace(',', ' ');
    unFormattedDate.replace('.', ' ');
    if (!unFormattedDate.isEmpty())
    {
        QStringList dateParts= unFormattedDate.split(' ');

        uint8_t const day = dateParts[0].toInt();
        uint8_t const month = romanToArabic(dateParts[1]);
        QDate const localDate = QDate :: currentDate();
        uint16_t year = localDate.year();
        if (localDate.month() == month)
        {
            if (localDate.day() >= day)
            {
                year = localDate.year();
            }
            else
            {
                year = localDate.year()-1;
            }
        }
        else if (localDate.month() > month)
        {
            year = localDate.year();
        }
        else
        {
            year = localDate.year()-1;
        }
        return QDate{year, month, day};
    }
    return {0, 0, 0};
}
void Scrapper::getGrades()
{
    QNetworkRequest request;
    request.setUrl(gradesFilePageUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozzila/5.0");

    QNetworkReply* reply = manager->get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    QString const unFormattedGradeData = reply->readAll();
    QString gradeData = unFormattedGradeData.simplified();

    QRegularExpression rowTray("<tr>(.*?)</tr>");
    QRegularExpression subject("<td[^>]*>(.*?)</td>");
    QRegularExpression grade("<span class=\"single-container\"[^>]*>(.*?)</span>");

    QRegularExpressionMatchIterator rowTrayIt = rowTray.globalMatch(gradeData);
    while (rowTrayIt.hasNext())
        {

            QRegularExpressionMatch rowTrayMatch = rowTrayIt.next();
            QString rowTrayString = rowTrayMatch.captured(1);

            QString SUBJECT;
            QRegularExpressionMatch subjectMatch = subject.match(rowTrayString);
            if (subjectMatch.hasMatch())
            {
                SUBJECT = subjectMatch.captured(1).simplified();
            }
            else
            {
                continue;
            }

            QStringList GRADES;

            QRegularExpressionMatchIterator gradeIt = grade.globalMatch(rowTrayString);
            while (gradeIt.hasNext())
            {
                QRegularExpressionMatch gradeMatch = gradeIt.next();
                QString gradeString = gradeMatch.captured(1).remove(",").simplified();
                if (!gradeString.isEmpty())
                {
                    GRADES << gradeString;
                }
            }
            gradesTable.insert(SUBJECT, GRADES);
        }
}
void Scrapper::pGrades(QMap<QString, Subject> &subjects)
{
    QMapIterator gradesTableIt(gradesTable);

    qDebug() << gradesTable["Matematica"];

    while (gradesTableIt.hasNext())
    {
        gradesTableIt.next();
        //qDebug() << "Subject" << gradesTableIt.key() << "Grades:" << gradesTableIt.value().join(',');
        QString const subjectName = gradesTableIt.key();

        if (!subjects.contains(subjectName))
        {
            subjects.insert(subjectName, Subject(subjectName));
        }

        Subject &currentSubject = subjects[subjectName];

        QStringListIterator gradesListIt(gradesTableIt.value());

        while (gradesListIt.hasNext())
        {
            qDebug() << "ok2";
            QString data(gradesListIt.next());

            QStringList dataList = gradeSeparator(data);

            uint16_t const value = dataList.first().simplified().toInt();
            QDate const date(formatDate(dataList.last()));

            if (!currentSubject.hasGrade(value, date)) {
                currentSubject.addGrade(value, date);
            }

        }
    }
}

template <typename>
constexpr auto Scrapper::qt_create_metaobjectdata()
{
}

QMap<QString, QStringList> Scrapper::grades()
{
    return gradesTable;
}
void Scrapper::init()
{
    QNetworkRequest request;

    request.setUrl(mainUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

    QNetworkReply* reply = manager->get(request);

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    loop.exec();

    if (handleReply(reply) == loginPage)
    {
        login(USERNAME, PASSWORD, reply);
    }

    getGrades();
}

