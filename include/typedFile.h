/********************************************************
 * tree- Tree b in disk                                 *
 *                                                      *
 * Author:  Lucas Batista dos Santos                    *
 *                                                      *
 ********************************************************/
#ifndef TYPEDFILE_H
#define TYPEDFILE_H

#include <fstream>
#include <type_traits>
#include "record.h"
#include "header.h"

using namespace std;

const ios::openmode mode = ios::in | ios::out | ios::binary;

template <class T>
class typedFile : private fstream {
    static_assert(is_base_of<serializable, T>::value, "T must be serializable");

    public:
        typedFile();
        typedFile(const string name, const string type, const unsigned int version, ios::openmode openmode = mode);
        virtual ~typedFile();
        bool open(const string name, const string type, const unsigned int version, ios::openmode openmode = mode);
        bool isOpen();
        bool close();
        void clear();
        bool readRecord(record<T>& r, unsigned long long int i);
        bool writeRecord(record<T>& r, unsigned long long int i);
        bool insertRecord(record<T>& r);
        bool deleteRecord(record<T> rDel, unsigned long long int i);
        unsigned long long int getRoot();
        unsigned long long int getFirstDeleted();
        unsigned long long int search(T data);
        unsigned long long int allocatePosition();
        unsigned long long int index2pos(unsigned long long int i);
        unsigned long long int pos2index(unsigned long long int p);
      protected:
        header head;
        bool readHeader(header &h);
        bool writeHeader(header &h);
};

template <class T>
typedFile<T>::typedFile() : fstream() {}

template <class T>
typedFile<T>::typedFile(const string name, const string type, const unsigned int version, ios::openmode openmode) : fstream(name.c_str(), mode) {
    head.setType(type); //seta tipo e versão
    head.setVersion(version);
    if (isOpen()) { //Caso arquivo exista ele será aberto automtico
        readHeader(head);
    }
    else { //Caso não exista criar manual
        open(name, type, version, openmode);
    }
}

template <class T>
typedFile<T>::~typedFile() {}

template <class T>
bool typedFile<T>::open(const string name, const string type, const unsigned int version, ios::openmode openmode) {

    if(!isOpen()) {
        fstream::open(name, ios::out);
        fstream::close();
        fstream::open(name, openmode);
        return writeHeader(head); //Escreve novo cabeçalho
    }
    return isOpen();
}

template <class T>
bool typedFile<T>::isOpen() {
    return fstream::is_open();
}

template <class T>
bool typedFile<T>::close() {
    fstream::close();
    return !isOpen();
}

template <class T>
void typedFile<T>::clear() {
    fstream::clear();
}

template <class T>
bool typedFile<T>::readRecord(record<T>& r, unsigned long long int i) {
    if(isOpen()) {
        char *aux = new char[r.size()]; //Aloca ponteiro char tamanho registro
        seekp(index2pos(i)); //Posiciona na pos do i
        read(aux, r.size());  //lê record
        r.fromString(string(aux, r.size())); //serializa para r
        delete[] aux; // deleta ponteiro
        aux = nullptr;
        return good();
    }
    else {
        cout << "Arquivo não aberto!" << endl;
        return false;
    }
}

template <class T>
bool typedFile<T>::writeRecord(record<T>& r, unsigned long long int i) {

    if(isOpen()) {
        seekp(index2pos(i));//posiciona na pos i
        if(write(r.toString().c_str(), r.size())) {
            return good();
        }
    }
    return false;
}

template <class T>
bool typedFile<T>::insertRecord(record<T>& r) {
    unsigned long long int i = getFirstDeleted();
    if (i != 0) { //verifica se existe algum deletado
        record<T> aux;
        readRecord(aux, i);
        head.setFirstDeleted(aux.getNext());
        r.setNext(getRoot());
    }
    else {
        if (getRoot() != 0) { //verifica se a lista já possui registro
            seekp(0, ios::end);  //posiciona a cabeça no fim do arquivo
            i = pos2index(tellp());  //pego o valor index
            r.setNext(getRoot()); //seta apontador para o proximo
        }
        else { //se a lista estiver vazia
            i = 1;
        }
    }
    if(writeRecord(r, i)) {
        head.setRoot(i);
        return writeHeader(head);
    }
    return false;
}

template <class T>
bool typedFile<T>::deleteRecord(record<T> rDel, unsigned long long int i) {
    rDel.del();
    rDel.setNext(getFirstDeleted());
    head.setFirstDeleted(i);
    writeHeader(head);
    return writeRecord(rDel, i);
}

template <class T>
unsigned long long int typedFile<T>::getRoot() {
    return head.getRoot();
}

template <class T>
unsigned long long int typedFile<T>::getFirstDeleted() {
    return head.getFirstDeleted();
}

template <class T>
unsigned long long int typedFile<T>::search(T data) {
    unsigned long long int i = getRoot();
    record<T> r;
    while(i != 0) {
        readRecord(r, i);
        if(data == r.getData()) {
            return i;
        }
        i = r.getNext();
    }
    return 0;
}

template <class T>
bool typedFile<T>::readHeader(header &h) {
    if(isOpen()) {
        char *aux = new char[head.size()];
        header hAux;
        seekg(0,  ios::beg);
        read(aux, head.size());
        hAux.fromString(string(aux, h.size()));
        delete[] aux;
        aux = nullptr;
        if (hAux.getVersion() == h.getVersion()) {
            h = hAux;
            return good();
        }
        else {
            cout << "Different file version!" << endl;
            cout << "Não será possivel salvar ou atualizar no arquivo!" << endl;
            return close();
        }
    }
    return false;
}

template <class T>
bool typedFile<T>::writeHeader(header &h) {
    if(isOpen()) {
        string serializedHead = h.toString();
        seekp(0, ios::beg);
        write(serializedHead.c_str(), h.size());
        return good();
    }
    return false;
}

template <class T>
unsigned long long int typedFile<T>::allocatePosition() {
    unsigned long long int pos = getFirstDeleted();
    if(isOpen()) {
        if(pos == 0) {
            seekp(0, ios::end);
            pos = tellp();
            return pos2index(pos);
            return pos;
        } else {
            record<T> aux;
            readRecord(aux, pos);
            head.setFirstDeleted(aux.getNext());
            writeHeader(head);
            return pos;
        }
    }
    return -1;
}

template <class T>
unsigned long long int typedFile<T>::index2pos(unsigned long long int i) {
    unsigned long long int pos;
    record<T> aux;
    pos = head.size() + (i - 1) * aux.size();
    return pos;
}

template <class T>
unsigned long long int typedFile<T>::pos2index(unsigned long long int p) {
    unsigned long long int n;
    record<T> aux;
    n = (p - head.size()) / aux.size() + 1;
    return n;
}

#endif // TYPEDFILE_H
