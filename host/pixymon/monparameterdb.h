#ifndef MONPARAMETERS_H
#define MONPARAMETERS_H

#include <QString>
#include "parameters.h"
#include "monmodule.h"

#define CONFIGFILE_FILENAME     "config.xml"
#define CONFIGFILE_TAG          "PixyMon_parameters"

class MonParameterDB : public ParameterDB
{
public:
    MonParameterDB();
    ~MonParameterDB();

    int load();
    int save();

    void addSlider(const QString &id, const QVariant &value, const QVariant &min, const QVariant &max, MonModule *mm,
                   const QString &help="", const QString &category="");
    void addBool(const QString &id, bool value, MonModule *mm, const QString &help="", const QString &category="");

    static QString docPath();
};

#endif // MONPARAMETERS_H