/********************************************************
 * tree- Tree b in disk                                 *
 *                                                      *
 * Author:  Lucas Batista dos Santos                    *
 *                                                      *
 ********************************************************/
#ifndef NODE_H
#define NODE_H

#include <iostream>
#include <vector>
#include "serializable.h"

using namespace std;

template <class T, const unsigned int MIN_DEGREE>
class node : public serializable {
    public:
        node();
        virtual ~node();
        bool isLeaf();
        void setLeaf(bool l);
        unsigned int sizeNode();
        void resize(unsigned int s);
        void insertKey(T k);
        void setKey(T k, unsigned int i);
        unsigned int hasKey(T k);
        T getKey(int i);
        void removeKey(unsigned int i);
        unsigned long long int getChild(unsigned int i);
        void setChild(unsigned long long int c, unsigned int i);
        void splitChild(node<T, MIN_DEGREE>& y,node<T, MIN_DEGREE>& z, unsigned int i, unsigned long long int iz);
        void mergeChildren(node<T, MIN_DEGREE>& y, node<T, MIN_DEGREE>& z, unsigned int i);
        void rotateKeys(node<T, MIN_DEGREE>& y, node<T, MIN_DEGREE>& z, unsigned int i);
        void printNode();
        //Serializable
        virtual string toString();
        virtual void fromString(string repr);
        virtual string toXML() { return ""; };
        virtual void fromXML(string repr) {};
        virtual string toCSV() { return ""; };
        virtual void fromCSV(string repr) {};
        virtual string toJSON() { return ""; };
        virtual void fromJSON(string repr) {};
        virtual unsigned long long int size() const;
        //Constantes
        static const unsigned int MAX = 2 * MIN_DEGREE - 1;
        static const unsigned int MIN = MIN_DEGREE - 1;
        static const unsigned int NOT_FOUND = -1;
    protected:
        vector<T> keys;
        vector<unsigned long long int> children;
        bool leaf;
        unsigned int n;
};

template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE>::node() {
    unsigned int i;
    leaf = true;
    n = 0;
    keys.resize(MAX);
    children.resize(MAX + 1);
    for (i = 0; i < children.size(); i++) {
        children[i] = NOT_FOUND;
    }
}

template <class T, const unsigned int MIN_DEGREE>
node<T, MIN_DEGREE>::~node() {}

template <class T, const unsigned int MIN_DEGREE>
bool node<T, MIN_DEGREE>::isLeaf() {
    return leaf;
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::setLeaf(bool l) {
    leaf = l;
}

template <class T, const unsigned int MIN_DEGREE>
unsigned int node<T, MIN_DEGREE>::sizeNode() {
    return n;
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::resize(unsigned int s) {
    unsigned int i;
    for(i = s + 1; i <= n; i++){ //Desaloca os filhos de (s + 1)
        children[i] = NOT_FOUND;
    }
    if(s == 0) {
        children[0] = NOT_FOUND;
    }
    n = s;
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::setKey(T k, unsigned int i) {
    keys[i] = k;
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::insertKey(T k) {
    int i = n - 1;

    while (i >= 0 && k < keys[i]) {
        keys[i + 1] = keys[i];
        i--;
    }
    keys[i + 1] = k;
    resize(n + 1);
}

template <class T, const unsigned int MIN_DEGREE>
unsigned int node<T, MIN_DEGREE>::hasKey(T k) {
    unsigned int i = 0;
    int pos = NOT_FOUND;

    while(i < n && pos == -1) {
        if(keys[i] == k) {
            pos = i;
        }
        i++;
    }
    return pos;
}

template <class T, const unsigned int MIN_DEGREE>
T node<T, MIN_DEGREE>::getKey(int i) {
    return keys[i];
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::removeKey(unsigned int i) {
    unsigned int j = i;
    while(j < n - 1) {
        keys[j] = keys[j + 1];
        j++;
    }
    if(!leaf) {
        j = i + 1;
        while (j < n) {
            children[j] = children[j + 1];
            j++;
        }
    }
    resize(n - 1);
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int node<T, MIN_DEGREE>::getChild(unsigned int i) {
    return children[i];
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::setChild(unsigned long long int c, unsigned int i) {
    children[i] = c;
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::splitChild(node<T, MIN_DEGREE>& y,node<T, MIN_DEGREE>& z, unsigned int i, unsigned long long int iz) {
    T middle = y.getKey(MIN_DEGREE - 1);
    unsigned int j = 0;

    for (j = 0; j < MIN_DEGREE - 1; j++) { //Insere chave de y em Z
        z.insertKey(y.getKey(j + MIN_DEGREE));
    }
    if (!y.isLeaf()) { //Atualiza os filhos
        z.setLeaf(false);
        for (j = 0; j < MIN_DEGREE; j++) {
            z.children[j] = y.getChild(j + MIN_DEGREE);
        }
    }
    for (j = n; j >= i + 1; j--) {
        children[j + 1] = children[j];//Aloca novo espaço para filho
    }
    children[i + 1] = iz;
    insertKey(middle);
    y.resize(MIN);//Define tamanho de y para min
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::mergeChildren(node<T, MIN_DEGREE>& y, node<T, MIN_DEGREE>& z, unsigned int i) {
    unsigned int j;

    y.insertKey(keys[i]);
    for(j = 0; j < MIN; j++) {
        y.insertKey(z.getKey(j));
    }
    if(!y.isLeaf()) {
        for (j = 0; j < MIN + 1; j++) {
            y.setChild(z.getChild(j), j + MIN + 1);
        }
    }
    if(n == 1) {
        keys[0] = keys[i + 1];
        n = 0;
    } else {
        removeKey(i);
    }
    z.resize(0);
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::rotateKeys(node<T, MIN_DEGREE>& y, node<T, MIN_DEGREE>& z, unsigned int i) {

    if(y.sizeNode() > MIN) {
        z.insertKey(keys[i]);
        keys[i] = y.getKey(y.sizeNode() - 1);

        if(!y.isLeaf()) {
            for(int j = z.sizeNode(); j >= 0; j--) {
                z.children[j+1] = z.children[j];
         }
         z.children[0] = y.getChild(y.sizeNode());
         z.setLeaf(false);
         y.setLeaf(false);
      }
      y.resize(y.sizeNode() - 1);
    } else {
        y.insertKey(keys[i]);
        keys[i] = z.getKey(0);

        if(!z.isLeaf()) {
            y.children[y.sizeNode()] = z.getChild(0);
        }
        z.removeKey(0);
    }
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::printNode(){
    cout << "[";
    for(unsigned int i = 0; i < n; i++){
        cout<< keys[i].getCharacter() << " ";
    }
    cout << "]";
}

template <class T, const unsigned int MIN_DEGREE>
string node<T, MIN_DEGREE>::toString() {
    string aux = "";
    aux += string(reinterpret_cast<char*>(&leaf), sizeof(leaf));
    aux += string(reinterpret_cast<char*>(&n), sizeof(n));
    for (unsigned int i = 0; i < keys.size(); i++) {
        aux += keys[i].toString(); //toString do charSerial
    }
    for (unsigned int i = 0; i < children.size(); i++) {
        aux += string(reinterpret_cast<char*>(&children[i]), sizeof(children[i]));
    }
    return aux;
}

template <class T, const unsigned int MIN_DEGREE>
void node<T, MIN_DEGREE>::fromString(string repr) {
    int pos = 0;
    string substrData;
    repr.copy(reinterpret_cast<char*>(&leaf), sizeof(leaf), pos);
    pos += sizeof(leaf);
    repr.copy(reinterpret_cast<char*>(&n), sizeof(n), pos);
    pos += sizeof(n);
    for (unsigned int i = 0; i < keys.size(); i++) {
        substrData = repr.substr(pos,keys[i].size());
        keys[i].fromString(substrData);
        pos += keys[i].size(); //size charSerial
        substrData = "";
    }
    for (unsigned int i = 0; i < children.size(); i++) {
        repr.copy(reinterpret_cast<char*>(&children[i]), sizeof(children[i]), pos);
        pos += sizeof(children[i]);
    }
}

template <class T, const unsigned int MIN_DEGREE>
unsigned long long int node<T, MIN_DEGREE>::size() const {
    return keys.size() * keys[0].size() + children.size() * sizeof(children[0]) + sizeof(leaf) + sizeof(n);
            //qtd de chaves * char + qtd filhos * unsigned long long int + bool + unsigned int
}

#endif // NODE_H
