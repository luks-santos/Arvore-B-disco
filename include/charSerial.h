#ifndef CHARSERIAL_H
#define CHARSERIAL_H

#include<string>
#include "serializable.h"

using namespace std;

class charSerial : public serializable {
    public:
        charSerial();
        charSerial(char character);
        charSerial(const charSerial& other);
        virtual ~charSerial();
        charSerial operator=(const charSerial& other);
        bool operator==(const charSerial& other) const;
        bool operator>=(const charSerial& other) const;
        bool operator<=(const charSerial& other) const;
        bool operator!=(const charSerial& other) const;
        bool operator>(const charSerial& other) const;
        bool operator<(const charSerial& other) const;
        char upperCase(const char character)const;
        void setCharacter(char character);
        char getCharacter() const;

        virtual string toString();
        virtual void fromString(string repr);
        virtual string toXML() { return ""; };
        virtual void fromXML(string repr) {};
        virtual string toCSV() { return ""; };
        virtual void fromCSV(string repr) {};
        virtual string toJSON() { return ""; };
        virtual void fromJSON(string repr) {};
        virtual unsigned long long int size() const;
    protected:
        char character;
};

#endif // CHARSERIAL_H
