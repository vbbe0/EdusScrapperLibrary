//
// Created by vbbe on 1/23/2026.
//

#include "Backend.h"
#include <QString>
#include <QHash>
#include<QDate>
#include<iostream>
#include<stdexcept>
#include <utility>

QString Class;
uint8_t Value;
QDate Date;



uint32_t romanToArabic(QString roman)
{
    uint32_t arabic = 0;
    QHash<QChar, uint16_t> romanNumbers;
    QHash<QChar, uint16_t> :: iterator it;

    romanNumbers.insert('I', 1);
    romanNumbers.insert('V', 5);
    romanNumbers.insert('X', 10);
    romanNumbers.insert('L', 50);
    romanNumbers.insert('C', 100);
    romanNumbers.insert('D', 500);
    romanNumbers.insert('M', 1000);

    uint16_t const n = roman.length();
    for (uint16_t i; i<=n-1;i++)
    {
        if (romanNumbers[roman[i]] >= romanNumbers[roman[i+1]])
        {
            arabic += romanNumbers[roman[i]];
        }
        else
        {
           arabic -= romanNumbers[roman[i]];
        }
    }

    return arabic;
}

Grade::Grade(uint16_t const v, QDate const d) : value(v), date(d) {};
QString Grade::getGrade()
{
    QString grade;
    grade.append(std::to_string(value) + " on ");
    grade.append(date.toString());
    return grade;
}

QStringList Subject::gradeSeparator(QString const& unFormattedGrade)
 {
        QString formattedGrade = unFormattedGrade.trimmed();
        return formattedGrade.split('/');
 }

Subject::Subject(QString name): m_name(std::move(name)){};

void Subject::addGrade(uint16_t value, QDate date)
{
    m_grades.append(Grade(value, date));
}
void Subject::addGrade(uint16_t value, QString unFormattedDate)
{
    m_grades.append(Grade(value,formatDate(unFormattedDate)));
}
float Subject::getAverage()
{
    int sum = 0;
    int n = 0;
    for (const Grade& grade : m_grades)
    {
        sum += grade.value;
        n++;
    }

    return sum / n;
}
QDate Subject::formatDate(QString unFormattedDate)
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
void Subject::pGrades()
{
    qDebug() << m_name;
    QListIterator<Grade> it(m_grades);
    while (it.hasNext())
    {
        Grade grade = it.next();
        qDebug() << grade.getGrade();
    }
}
bool Subject::hasGrade(int value, QDate date) {
    for (const Grade &g : m_grades) {
        if (g.value == value && g.date == date) {
            return true; // Found it!
        }
    }
    return false; // Not found
}
int SubjectManager::addSubject(const QString subjectName)
{
    if (subjectMap.contains(subjectName))
    {
        return 101;
    }

    subjectMap.insert(subjectName, Subject(subjectName));
    return 0;
}