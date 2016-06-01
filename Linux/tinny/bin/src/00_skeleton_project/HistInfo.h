#ifndef HISTINFO_H
#define HISTINFO_H

class HistInfo {

public:

    virtual ~HistInfo(){};
    virtual void update() = 0;
    virtual void display() = 0;

};

#endif
