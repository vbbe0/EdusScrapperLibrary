//
// Created by vbbe on 1/23/2026.
//

#ifndef EDUSSCRAPPER_BACKEND_H
#define EDUSSCRAPPER_BACKEND_H

#endif //EDUSSCRAPPER_BACKEND_H
#include <QString>
#include <QDate>
#include <stdexcept>

uint32_t romanToArabic(QString roman);
class Grade
{
public:
    int value;
    QDate date;
    Grade(uint16_t v, QDate d);
    QString getGrade();
};
class Subject
{
public:
    Subject() = default;
    Subject(QString name);
    void setSubject(QString name){m_name = std::move(name);};
    void addGrade(uint16_t value, QDate date);
    void addGrade(uint16_t value, QString date);
    bool hasGrade(int value, QDate date);
    float getAverage();
    void pGrades();
private:
    QString m_name;
    QList<Grade> m_grades;

    QStringList gradeSeparator(QString const& unFormattedGrade);
    QDate formatDate(QString unFormattedDate);

};


class SubjectManager
{
public:
    int addSubject(QString subjectName);
private:
    QMap<QString, Subject> subjectMap;
};