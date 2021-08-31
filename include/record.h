#ifndef RECORD_H
#define RECORD_H

#include <string>
#include "serializable.h"

using namespace std;

template <class T>
class record : public serializable {
   static_assert(is_base_of<serializable, T>::value, "T must be serializable");
   public:
      record();
      record(T d);
      record(const record<T> &other);
      virtual ~record();
      record<T> operator=(const record<T> &other);
      bool operator==(const record<T> &other);
      T getData() const;
      void setData(T d);
      unsigned long long int getNext() const;
      void setNext(unsigned long long int n);
      bool isDeleted() const;
      void del();
      void undel();
      virtual string toString();
      virtual void fromString(string repr);
      virtual string toCSV() { return ""; }
      virtual void fromCSV(string repr) {}
      virtual string toJSON() { return ""; }
      virtual void fromJSON(string repr) {}
      virtual string toXML() { return ""; }
      virtual void fromXML(string repr) {}
      virtual unsigned long long int size() const;
   protected:
      T data;
      bool deleted;
      unsigned long long int next;
};

template <class T>
record<T>::record() : serializable() {
}

template <class T>
record<T>::record(T d) : serializable() {
    data = d;
    deleted = false;
    next = 0;
}

template <class T>
record<T>::record(const record<T> &other) {
    data = other.getData();
    deleted = other.isDeleted();
    next = other.getNext();
}

template <class T>
record<T>::~record() {

}

template <class T>
record<T> record<T>::operator=(const record<T> &other) {
    record<T> aux(other);
    if (this == &other) {
        return *this;
    }
    data = other.getData();
    deleted = other.isDeleted();
    next = other.getNext();

    return aux;
}

template <class T>
bool record<T>::operator==(const record<T> &other) {
    return data == other.getData() &&
            deleted == other.isDeleted() &&
                next == other.getNext();
}

template <class T>
T record<T>::getData() const {
    return data;
}

template <class T>
void record<T>::setData(T d) {
    data = d;
}

template <class T>
unsigned long long int record<T>::getNext() const {
    return next;
}

template <class T>
void record<T>::setNext(unsigned long long int n) {
    next = n;
}

template <class T>
bool record<T>::isDeleted() const {
    return deleted;
}

template <class T>
void record<T>::del() {
    deleted = true;
}

template <class T>
void record<T>::undel() {
    deleted = false;
}

template <class T>
string record<T>::toString() {
    string aux = "";
    aux += data.toString();
    aux += string(reinterpret_cast<char*>(&deleted), sizeof(deleted));
    aux += string(reinterpret_cast<char*>(&next), sizeof(next));
    return aux;
}

template <class T>
void record<T>::fromString(string repr) {
    int pos = 0;
    string substrData = repr.substr(pos, data.size());
    data.fromString(substrData);
    pos += data.size();
    repr.copy(reinterpret_cast<char*>(&deleted), sizeof(deleted), pos);
    pos += sizeof(deleted);
    repr.copy(reinterpret_cast<char*>(&next), sizeof(next), pos);
    pos += sizeof(next);
}

template <class T>
unsigned long long int record<T>::size() const {
    return data.size() + sizeof(deleted) + sizeof(next);
}

#endif // RECORD_H
