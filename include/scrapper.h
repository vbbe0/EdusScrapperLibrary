//
// Created by vbbe on 2/8/2026.
//

#ifndef EDUSSCRAPPERLIBRARY_SCRAPPER_H
#define EDUSSCRAPPERLIBRARY_SCRAPPER_H

#endif //EDUSSCRAPPERLIBRARY_SCRAPPER_H

#include <Backend.h>

#include <QCoreApplication>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkCookieJar>
#include <QNetworkCookie>
#include <QUrlQuery>
#include <QList>
#include <QUrl>
#include <QString>
#include <QObject>
#include <QDebug>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QFile>
#include <QFileInfo>
#include <QDir>


class MY_EXPORT PersistentCookieJar : public QNetworkCookieJar
{
    Q_OBJECT
public:
    PersistentCookieJar(QString cookiePath);
    ~PersistentCookieJar();
    bool saveToDisk();
    bool loadFromDisk();
    void setSavePath(QString cookiePath);
private:
    QString m_cookiePath;
};
class MY_EXPORT Scrapper : public QObject
{
    Q_OBJECT
public:
    enum pageTypes
    {
        loginPage,
        mainPage,
        gradesPage,
        gradesFilePage
    };
    Scrapper(QUrl const edusUrl, QUrl const gradeUrl, QString user, QString password);
    void init();
    pageTypes handleReply(QNetworkReply* unFormattedReply);
    void login(QString const& username, QString const& password, QNetworkReply* reply);
    void getToken(const QString &reply);
    void getGrades();
    void pGrades(QMap<QString,Subject> &subjects);
    QMap<QString, QStringList> grades();
    QStringList gradeSeparator(QString const& unFormattedGrade);
    QDate formatDate(QString unFormattedDate);

private:
    QNetworkAccessManager* manager;
    PersistentCookieJar* cookieJar;
    QUrl mainUrl;
    QUrl gradesFilePageUrl;
    QString token;
    QHash<QString, pageTypes> pageTable;
    QMap<QString, QStringList> gradesTable;
    QString USERNAME;
    QString PASSWORD;
};
