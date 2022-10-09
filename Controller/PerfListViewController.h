#pragma once
#include "../CommonDefines.h"

class PerfListViewController : public QObject
{
    Q_OBJECT
    
public:
    PerfListViewController(QObject* parent = nullptr);
    virtual ~PerfListViewController();
};