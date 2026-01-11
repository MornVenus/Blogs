#ifndef STUDENT_H
#define STUDENT_H

#include <QObject>

class Student
{
public:
    Student(QString name, int age): m_name{name}, m_age{age}
    {

    }

    QString getName()
    {
        return m_name;
    }

    int getAge()
    {
        return m_age;
    }

private:
    QString m_name;
    int m_age;
};

#endif // STUDENT_H
